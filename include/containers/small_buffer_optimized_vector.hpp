// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/array.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array.hpp>
#include <containers/dynamic_resizable_array.hpp>
#include <containers/index_type.hpp>
#include <containers/repeat_n.hpp>
#include <containers/scope_guard.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/assert.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <cstring>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

// Explanation of SSO (small / short string optimization)
// https://stackoverflow.com/a/10319672/852254


template<typename T>
struct owning_storage {
	owning_storage(dynamic_array_data<T> storage_):
		storage(std::move(storage_)),
		active(true)
	{
	}
	owning_storage(owning_storage && other) noexcept:
		storage(other.storage),
		active(std::exchange(other.active, false))
	{
	}
	~owning_storage() {
		if (active) {
			::containers::detail::deallocate_storage(storage);
		}
	}
	
	dynamic_array_data<T> storage;
	bool active;
};


template<typename T>
constexpr auto data(owning_storage<T> & container) {
	return container.storage.pointer;
}


// If size remains below requested_small_capacity, there will be no allocations.
// requested_small_capacity is a guaranteed minimum. The implementation may
// reserve more space if it is less than minimum_small_capacity.
// minimum_small_capacity is equal to the minimum amount of overhead required to
// support the small buffer optimization, so there is no point going below it.
//
// Therefore, to request the smallest possible buffer, the user can request a
// size of 1
template<typename T>
inline constexpr auto minimum_small_capacity = (bounded::size_of<std::pair<typename dynamic_array_data<T>::size_type, dynamic_array_data<T>>> - bounded::size_of<unsigned char>) / bounded::size_of<T>;


template<typename T, std::size_t requested_small_capacity>
struct sbo_vector_base {
	using value_type = T;

	struct small_t {
		// m_force_large exists just to be a bit that's always 0. This allows
		// is_large to return the correct answer even for 0-size containers.
		constexpr small_t():
			m_force_large(false),
			m_size(0_bi)
		{
		}
		
		constexpr auto is_large() const {
			return m_force_large;
		}
		static constexpr auto capacity() {
			return bounded::max(minimum_small_capacity<T>, bounded::constant<requested_small_capacity>);
		}

		using size_type = bounded::integer<0, bounded::detail::normalize<capacity().value()>>;
		static_assert(sizeof(size_type) == sizeof(unsigned char));

		constexpr auto size() const {
			return static_cast<size_type>(m_size);
		}
		constexpr auto set_size(size_type const size_) {
			m_size = size_.value();
		}

		constexpr auto data() const {
			return reinterpret_cast<T const *>(containers::data(m_data));
		}
		constexpr auto data() {
			return reinterpret_cast<T *>(containers::data(m_data));
		}

	private:
		bool m_force_large : 1;
		typename size_type::underlying_type m_size : (bounded::size_of_bits<size_type> - 1_bi).value();
		array<trivial_storage<T>, capacity().value()> m_data;
	};

	struct large_t {
		using size_type = typename detail::dynamic_array_data<T>::size_type;
		using capacity_type = bounded::integer<
			bounded::detail::normalize<(small_t::capacity() + bounded::constant<1>).value()>,
			bounded::detail::builtin_max_value<size_type>
		>;

		// m_force_large exists just to be a bit that's always 1.
		constexpr large_t(size_type size_, capacity_type capacity_, T * pointer_):
			m_force_large(true),
			m_size(size_),
			m_data{reinterpret_cast<trivial_storage<T> *>(pointer_), capacity_}
		{
			BOUNDED_ASSERT_OR_ASSUME(pointer_ != nullptr);
		}

		constexpr auto is_large() const {
			return m_force_large;
		}
		constexpr auto capacity() const {
			return m_data.size;
		}

		constexpr auto size() const {
			return static_cast<size_type>(m_size);
		}
		constexpr auto set_size(size_type const size_) {
			m_size = size_.value();
		}

		constexpr auto data() const {
			return reinterpret_cast<T const *>(containers::data(m_data));
		}
		constexpr auto data() {
			return reinterpret_cast<T *>(containers::data(m_data));
		}

	private:
		bool m_force_large : 1;
		typename size_type::underlying_type m_size : (bounded::size_of_bits<size_type> - 1_bi).value();
		dynamic_array_data<trivial_storage<T>> m_data;
	};
	
	static_assert(std::is_nothrow_move_constructible_v<value_type>);
	static_assert(std::is_nothrow_move_assignable_v<value_type>);
	using size_type = typename large_t::size_type;

	static_assert(
		bounded::max_value<size_type> <= bounded::constant<(1ULL << (CHAR_BIT * sizeof(value_type *) - 1)) - 1>,
		"Maximum possible size is too large -- would use bit reserved for small-buffer optimization."
	);
	static_assert(requested_small_capacity > 0, "Must request at least one element for the minimum capacity, otherwise you should use vector.");

	using const_iterator = contiguous_iterator<value_type const, bounded::detail::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::detail::builtin_max_value<size_type>>;

	constexpr sbo_vector_base():
		m_small()
	{
	}
	
	~sbo_vector_base() {
		deallocate_large();
	}
	
	// The elements are destroyed, but any storage may still remain
	auto move_assign_to_empty(sbo_vector_base && other) & {
		deallocate_large();
		if (other.is_small()) {
			::bounded::construct(m_small);
			containers::uninitialized_move_destroy(other, m_small.data());
			m_small.set_size(other.m_small.size());
			other.m_small.set_size(0_bi);
		} else {
			::bounded::construct(m_large, other.m_large);
			// It is safe to skip the destructor call of other.m_large because
			// we do not rely on its side-effects
			::bounded::construct(other.m_small);
		}
	}

	friend constexpr auto begin(sbo_vector_base const & container) {
		auto const result = container.is_small() ?
			container.m_small.data() :
			container.m_large.data();
		BOUNDED_ASSERT_OR_ASSUME(result != nullptr);
		return const_iterator(result);
	}
	friend constexpr auto begin(sbo_vector_base & container) {
		return iterator(const_cast<value_type *>(pointer_from(begin(std::as_const(container)))));
	}
	
	friend constexpr auto end(sbo_vector_base const & container) {
		return begin(container) + container.size();
	}
	friend constexpr auto end(sbo_vector_base & container) {
		return begin(container) + container.size();
	}

	auto capacity() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.capacity(), m_large.capacity());
	}


	auto make_storage(auto const new_capacity) & {
		return owning_storage<value_type>(detail::make_storage<value_type>(new_capacity));
	}

	auto relocate_preallocated(owning_storage<value_type> new_large) {
		deallocate_large();
		::bounded::construct(
			m_large,
			size(),
			static_cast<typename large_t::capacity_type>(new_large.storage.size),
			reinterpret_cast<value_type *>(new_large.storage.pointer)
		);
		BOUNDED_ASSERT(is_large());
		new_large.active = false;
	}


	// Assumes that requested_capacity is large enough
	auto relocate(auto const requested_capacity) {
		if (requested_capacity <= small_t::capacity()) {
			relocate_to_small();
		} else {
			auto temp = make_storage(requested_capacity);
			containers::uninitialized_move_destroy(*this, data(temp));
			relocate_preallocated(std::move(temp));
		}
	}
	
	auto set_size(auto const new_size) {
		if (is_small()) {
			if constexpr (std::is_constructible_v<typename small_t::size_type, decltype(new_size)>) {
				m_small.set_size(static_cast<typename small_t::size_type>(new_size));
			} else {
				bounded::assert_or_assume_unreachable();
			}
		} else {
			m_large.set_size(static_cast<typename large_t::size_type>(new_size));
		}
	}

private:
	auto size() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.size(), m_large.size());
	}
	auto relocate_to_small() {
		if (is_small()) {
			return;
		}
		auto temp = std::move(m_large);
		auto const guard = scope_guard([&]{
			detail::deallocate_storage(dynamic_array_data(temp.data(), temp.capacity()));
		});
		// It is safe to skip the destructor call of m_large
		// because we do not rely on its side-effects
		::bounded::construct(m_small);
		containers::uninitialized_move_destroy(temp.data(), temp.data() + temp.size(), m_small.data());
		BOUNDED_ASSERT(is_small());
	}
	
	constexpr auto is_large() const {
		// The original design involved casting to unsigned char const &.
		// A reference to either member of a union is a reference to the union.
		// https://stackoverflow.com/questions/891471/union-element-alignment
		// You can treat any object as an array of char or unsigned char.
		// Therefore, we can use this cast to access the first byte.
		//
		// Instead, it seems that using a bit-field is a simpler design. You can
		// examine the common initial sequence of two standard-layout union
		// members, even if those members are bit-fields.
		// https://stackoverflow.com/a/18564719/852254
		return m_small.is_large();
	}
	constexpr auto is_small() const {
		return !is_large();
	}
	
	auto deallocate_large() {
		if (is_large()) {
			detail::deallocate_storage(dynamic_array_data(m_large.data(), m_large.capacity()));
		}
	}
	
	static_assert(sizeof(small_t) >= sizeof(large_t), "Incorrect buffer sizes.");
	static_assert(std::is_standard_layout_v<small_t>);
	static_assert(std::is_standard_layout_v<large_t>);

	union {
		small_t m_small;
		large_t m_large;
	};
};


}	// namespace detail

template<typename T, std::size_t requested_capacity>
inline constexpr auto is_container<detail::sbo_vector_base<T, requested_capacity>> = true;

template<typename T, std::size_t requested_small_capacity>
using small_buffer_optimized_vector = detail::dynamic_resizable_array<detail::sbo_vector_base<T, requested_small_capacity>>;

}	// namespace containers
