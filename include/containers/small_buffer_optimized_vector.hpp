// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/array.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array.hpp>
#include <containers/index_type.hpp>
#include <containers/repeat_n.hpp>
#include <containers/scope_guard.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/assert.hpp>
#include <bounded/concepts.hpp>
#include <bounded/integer.hpp>

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

// Explanation of SSO (small / short string optimization)
// https://stackoverflow.com/a/10319672/852254

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

} // namespace detail

template<typename T, std::size_t requested_small_capacity>
struct small_buffer_optimized_vector {
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
			return bounded::max(detail::minimum_small_capacity<T>, bounded::constant<requested_small_capacity>);
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
		detail::dynamic_array_data<trivial_storage<T>> m_data;
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

	constexpr small_buffer_optimized_vector():
		m_small()
	{
	}
	
	template<range Range> requires(
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit small_buffer_optimized_vector(Range && source):
		small_buffer_optimized_vector()
	{
		append(*this, OPERATORS_FORWARD(source));
	}
	
	constexpr small_buffer_optimized_vector(std::initializer_list<value_type> init):
		small_buffer_optimized_vector()
	{
		append(*this, init);
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector const & other):
		small_buffer_optimized_vector()
	{
		append(*this, other);
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector && other) noexcept:
		small_buffer_optimized_vector()
	{
		move_assign_to_empty(std::move(other));
	}

	constexpr auto & operator=(small_buffer_optimized_vector const & other) & {
		assign(*this, other);
		return *this;
	}
	constexpr auto & operator=(small_buffer_optimized_vector && other) & noexcept {
		::containers::detail::destroy_range(*this);
		deallocate_large();
		move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
	}

	constexpr ~small_buffer_optimized_vector() {
		::containers::detail::destroy_range(*this);
		deallocate_large();
	}

	friend constexpr auto begin(small_buffer_optimized_vector const & container) {
		auto const result = container.is_small() ?
			container.m_small.data() :
			container.m_large.data();
		BOUNDED_ASSERT_OR_ASSUME(result != nullptr);
		return const_iterator(result);
	}
	friend constexpr auto begin(small_buffer_optimized_vector & container) {
		return iterator(const_cast<value_type *>(pointer_from(begin(std::as_const(container)))));
	}
	
	friend constexpr auto end(small_buffer_optimized_vector const & container) {
		return begin(container) + container.size();
	}
	friend constexpr auto end(small_buffer_optimized_vector & container) {
		return begin(container) + container.size();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	constexpr auto capacity() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.capacity(), m_large.capacity());
	}
	constexpr auto reserve(size_type const requested_capacity) {
		if (requested_capacity > capacity()) {
			relocate(requested_capacity);
		}
	}
	constexpr auto shrink_to_fit() {
		auto const s = size();
		if (s != capacity()) {
			relocate(s);
		}
	}

	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		auto const new_size = size() + count;
		if (is_small()) {
			if constexpr (bounded::constructible_from<typename small_t::size_type, decltype(new_size)>) {
				m_small.set_size(static_cast<typename small_t::size_type>(new_size));
			} else {
				bounded::assert_or_assume_unreachable();
			}
		} else {
			m_large.set_size(static_cast<typename large_t::size_type>(new_size));
		}
		BOUNDED_ASSERT(size() == new_size);
	}


private:
	constexpr auto size() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.size(), m_large.size());
	}
	constexpr auto relocate_to_small() {
		if (is_small()) {
			return;
		}
		auto temp = std::move(m_large);
		auto const guard = scope_guard([&]{
			detail::deallocate_storage(detail::dynamic_array_data(temp.data(), temp.capacity()));
		});
		// It is safe to skip the destructor call of m_large
		// because we do not rely on its side-effects
		::bounded::construct(m_small, []{ return small_t(); });
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
	
	constexpr auto deallocate_large() {
		if (is_large()) {
			detail::deallocate_storage(detail::dynamic_array_data(m_large.data(), m_large.capacity()));
		}
	}
	
	constexpr auto move_assign_to_empty(small_buffer_optimized_vector && other) & {
		auto make_small = [](small_t & small) {
			::bounded::construct(small, bounded::construct_return<small_t>);
		};
		if (other.is_small()) {
			make_small(m_small);
			containers::uninitialized_move_destroy(other, m_small.data());
			m_small.set_size(other.m_small.size());
			other.m_small.set_size(0_bi);
		} else {
			::bounded::construct(m_large, bounded::value_to_function(other.m_large));
			// It is safe to skip the destructor call of other.m_large because
			// we do not rely on its side-effects
			make_small(other.m_small);
		}
	}

	constexpr auto relocate_to_large(auto const requested_capacity) {
		auto temp = detail::make_storage<value_type>(requested_capacity);
		containers::uninitialized_move_destroy(*this, temp.pointer);
		deallocate_large();
		::bounded::construct(
			m_large,
			[&] {
				return large_t(
					size(),
					static_cast<typename large_t::capacity_type>(temp.size),
					reinterpret_cast<value_type *>(temp.pointer)
				);
			}
		);
		BOUNDED_ASSERT(is_large());
	}


	// Assumes that requested_capacity is large enough
	constexpr auto relocate(auto const requested_capacity) {
		if (requested_capacity <= small_t::capacity()) {
			relocate_to_small();
		} else {
			relocate_to_large(requested_capacity);
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


template<typename T, std::size_t requested_capacity>
inline constexpr auto is_container<small_buffer_optimized_vector<T, requested_capacity>> = true;

}	// namespace containers
