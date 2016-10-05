// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/add_remove_const.hpp>
#include <containers/array/array.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/dynamic_array/dynamic_array.hpp>
#include <containers/dynamic_resizable_array.hpp>
#include <containers/index_type.hpp>
#include <containers/repeat_n.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/uninitialized_storage.hpp>

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


template<typename T, typename Allocator>
struct allocator_aware_storage {
	allocator_aware_storage(Allocator & allocator_, dynamic_array_data_t<T> storage_) noexcept:
		allocator(allocator_),
		storage(std::move(storage_)),
		active(true)
	{
	}
	allocator_aware_storage(allocator_aware_storage && other) noexcept:
		allocator(other.allocator),
		storage(other.storage),
		active(std::exchange(other.active, false))
	{
	}
	~allocator_aware_storage() {
		if (active) {
			::containers::detail::deallocate_storage(allocator, storage);
		}
	}
	
	Allocator & allocator;
	dynamic_array_data_t<T> storage;
	bool active;
};


template<typename T, typename Allocator>
constexpr auto data(allocator_aware_storage<T, Allocator> & container) noexcept {
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
constexpr auto minimum_small_capacity = (2_bi * bounded::size_of<typename detail::dynamic_array_data_t<T>::size_type> + bounded::size_of<T *> - bounded::size_of<unsigned char>) / bounded::size_of<T>;


template<typename T, std::size_t requested_small_capacity, typename Allocator>
struct sbo_vector_base : private detail::rebound_allocator<T, Allocator> {
	using value_type = T;
	using allocator_type = detail::rebound_allocator<T, Allocator>;
	static_assert(std::is_nothrow_move_constructible<value_type>::value);
	static_assert(std::is_nothrow_move_assignable<value_type>::value);
	using size_type = typename detail::dynamic_array_data_t<value_type>::size_type;

	static_assert(
		size_type::max() <= bounded::constant<(1ULL << (CHAR_BIT * sizeof(value_type *) - 1)) - 1>,
		"Maximum possible size is too large -- will use bit reserved for small-buffer optimization."
	);
	static_assert(requested_small_capacity > 0, "Must request at least one element for the minimum capacity, otherwise you should use vector.");

	using const_iterator = detail::basic_array_iterator<value_type const, sbo_vector_base>;
	using iterator = detail::basic_array_iterator<value_type, sbo_vector_base>;

	constexpr auto && get_allocator() const & noexcept {
		return static_cast<allocator_type const &>(*this);
	}
	constexpr auto && get_allocator() & noexcept {
		return static_cast<allocator_type &>(*this);
	}
	constexpr auto && get_allocator() && noexcept {
		return static_cast<allocator_type &&>(*this);
	}
	
	constexpr sbo_vector_base() noexcept:
		m_small()
	{
	}
	constexpr sbo_vector_base(allocator_type allocator) noexcept(std::is_nothrow_move_constructible<allocator_type>::value):
		allocator_type(std::move(allocator)),
		m_small()
	{
	}
	
	~sbo_vector_base() {
		deallocate_large();
	}
	
	// The elements are destroyed, but any storage may still remain
	auto move_assign_to_empty(sbo_vector_base && other) & noexcept {
		deallocate_large();
		if (other.is_small()) {
			::bounded::construct(m_small);
			::containers::uninitialized_move_destroy(begin(other), end(other), begin(m_small.data), get_allocator());
			m_small.size = other.m_small.size;
			other.m_small.size = 0;
		} else {
			::bounded::construct(m_large, other.m_large);
			// It is safe to skip the destructor call of other.m_large because
			// we do not rely on its side-effects
			::bounded::construct(other.m_small);
		}
	}

	friend constexpr auto begin(sbo_vector_base const & container) noexcept {
		auto const result = container.is_small() ?
			reinterpret_cast<value_type const *>(data(container.m_small.data)) :
			container.m_large.pointer;
		assert(result != nullptr);
		return const_iterator(result, detail::iterator_constructor);
	}
	friend constexpr auto begin(sbo_vector_base & container) noexcept {
		auto const result = container.is_small() ?
			reinterpret_cast<value_type *>(data(container.m_small.data)) :
			container.m_large.pointer;
		assert(result != nullptr);
		return iterator(result, detail::iterator_constructor);
	}
	
	friend constexpr auto end(sbo_vector_base const & container) noexcept {
		return begin(container) + container.size();
	}
	friend constexpr auto end(sbo_vector_base & container) noexcept {
		return begin(container) + container.size();
	}

	auto capacity() const noexcept {
		return is_small() ? static_cast<size_type>(bounded::constant<small_capacity>) : m_large.capacity;
	}


	template<typename Size>
	auto make_storage(Size const new_capacity) & {
		return allocator_aware_storage<value_type, allocator_type>(
			get_allocator(),
			::containers::detail::make_storage<value_type>(get_allocator(), new_capacity)
		);
	}

	auto relocate_preallocated(allocator_aware_storage<value_type, allocator_type> new_large) noexcept {
		deallocate_large();
		// TODO: static_assert noexcept
		::bounded::construct(
			m_large,
			size(),
			static_cast<typename large_t::capacity_type>(new_large.storage.size),
			reinterpret_cast<value_type *>(new_large.storage.pointer)
		);
		assert(is_large());
		new_large.active = false;
	}


	// Assumes that requested_capacity is large enough
	template<typename Size>
	auto relocate(Size const requested_capacity) {
		if (requested_capacity <= bounded::constant<small_capacity>) {
			relocate_to_small();
		} else {
			auto temp = make_storage(requested_capacity);
			::containers::uninitialized_move_destroy(begin(*this), end(*this), data(temp), get_allocator());
			relocate_preallocated(std::move(temp));
		}
	}
	
	template<typename Size>
	auto set_size(Size const new_size) noexcept {
		if (is_small()) {
			m_small.size = static_cast<typename small_t::size_type::underlying_type>(new_size);
		} else {
			m_large.size = static_cast<typename size_type::underlying_type>(new_size);
		}
	}

private:
	auto size() const noexcept {
		return is_small() ? static_cast<size_type>(m_small.size) : m_large.size;
	}
	auto relocate_to_small() noexcept {
		if (is_small()) {
			return;
		}
		auto && allocator = get_allocator();
		auto const temp =  std::move(m_large);
		auto const guard = scope_guard([&]{ ::containers::detail::deallocate_storage(allocator, temp.pointer, temp.capacity); });
		// It is safe to skip the destructor call of m_large
		// because we do not rely on its side-effects
		::bounded::construct(m_small);
		::containers::uninitialized_move_destroy(temp.pointer, temp.pointer + temp.size, begin(m_small.data), allocator);
		assert(is_small());
	}
	
	constexpr auto is_small() const noexcept {
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
		return m_small.force_small;
	}
	constexpr auto is_large() const noexcept {
		return !is_small();
	}
	
	auto deallocate_large() noexcept {
		if (is_large()) {
			::containers::detail::deallocate_storage(get_allocator(), m_large.pointer, m_large.capacity);
		}
	}
	
	static constexpr auto small_capacity = bounded::max(minimum_small_capacity<value_type>, bounded::constant<requested_small_capacity>).value();

	struct small_t {
		using size_type = bounded::integer<0, small_capacity>;
		static_assert(sizeof(size_type) == sizeof(unsigned char));

		// force_small exists just to be a bit that's always 1. This allows
		// is_small to return the correct answer even for 0-size containers.
		constexpr small_t() noexcept:
			force_small(true),
			size(0_bi)
		{
		}

		bool force_small : 1;
		typename size_type::underlying_type size : (bounded::size_of_bits<size_type> - 1_bi).value();
		array<trivial_storage<value_type>, small_capacity> data;
	};

	struct large_t {
		using capacity_type = bounded::integer<small_capacity + 1, size_type::max().value()>;

		constexpr large_t(size_type size_, capacity_type capacity_, value_type * pointer_) noexcept:
			force_small(false),
			size(size_),
			capacity(capacity_),
			pointer(pointer_)
		{
			assert(pointer != nullptr);
		}

		bool force_small : 1;
		typename size_type::underlying_type size : (bounded::size_of_bits<size_type> - 1_bi).value();
		capacity_type capacity;
		value_type * pointer;
	};
	
	static_assert(sizeof(small_t) >= sizeof(large_t), "Incorrect buffer sizes.");
	static_assert(std::is_standard_layout<small_t>::value);
	static_assert(std::is_standard_layout<large_t>::value);

	union {
		small_t m_small;
		large_t m_large;
	};
};


}	// namespace detail

template<typename T, std::size_t requested_capacity, typename Allocator>
struct is_container_c<detail::sbo_vector_base<T, requested_capacity, Allocator>> : std::true_type {};

template<typename T, std::size_t requested_small_capacity, typename Allocator = allocator<T>>
using small_buffer_optimized_vector = detail::dynamic_resizable_array<detail::sbo_vector_base<T, requested_small_capacity, Allocator>>;

}	// namespace containers
