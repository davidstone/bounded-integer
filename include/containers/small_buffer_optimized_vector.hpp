// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/array.hpp>
#include <containers/assign.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array_data.hpp>
#include <containers/initializer_range.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/uninitialized_array.hpp>

#include <bounded/assert.hpp>
#include <bounded/concepts.hpp>
#include <bounded/integer.hpp>
#include <bounded/scope_guard.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <cstddef>
#include <span>
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
// size of 0.
template<typename T, typename Size>
inline constexpr auto minimum_small_capacity = (bounded::size_of<std::pair<Size, dynamic_array_data<T, Size>>> - bounded::size_of<unsigned char>) / bounded::size_of<T>;

} // namespace detail

template<typename T, std::size_t requested_small_capacity, std::size_t max_size = containers::detail::maximum_array_size<T>>
struct small_buffer_optimized_vector : private lexicographical_comparison::base {
	using size_type = bounded::integer<0, bounded::normalize<max_size>>;

	struct small_t {
		constexpr small_t() = default;
		
		constexpr auto is_large() const {
			return m_force_large;
		}
		static constexpr auto capacity() {
			return bounded::max(detail::minimum_small_capacity<T, small_buffer_optimized_vector::size_type>, bounded::constant<requested_small_capacity>);
		}

		using size_type = bounded::integer<0, bounded::normalize<capacity()>>;
		static_assert(sizeof(size_type) == sizeof(unsigned char));

		constexpr auto size() const {
			return ::bounded::assume_in_range<size_type>(m_size);
		}
		constexpr auto set_size(size_type const size_) {
			m_size = size_.value();
		}

		constexpr auto begin() const {
			return m_storage.data();
		}
		constexpr auto begin() {
			return m_storage.data();
		}

	private:
		// m_force_large exists just to be a bit that's always 0. This allows
		// is_large to return the correct answer even for 0-size containers.
		bool m_force_large : 1 = false;
		typename size_type::underlying_type m_size : (bounded::size_of_bits<size_type> - 1_bi).value() = 0;
		uninitialized_array<T, capacity()> m_storage;
	};

	struct large_t {
		using capacity_type = bounded::integer<
			bounded::normalize<small_t::capacity() + bounded::constant<1>>,
			bounded::normalize<max_size>
		>;

		// m_force_large exists just to be a bit that's always 1.
		constexpr large_t(size_type size_, capacity_type capacity_, T * pointer_):
			m_size(size_),
			m_data(pointer_, capacity_)
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
			return ::bounded::assume_in_range<size_type>(m_size);
		}
		constexpr auto set_size(size_type const size_) {
			m_size = size_.value();
		}

		constexpr auto begin() const {
			return m_data.pointer;
		}
		constexpr auto begin() {
			return m_data.pointer;
		}

	private:
		bool m_force_large : 1 = true;
		typename size_type::underlying_type m_size : (bounded::size_of_bits<size_type> - 1_bi).value();
		detail::dynamic_array_data<T, size_type> m_data;
	};
	
	static_assert(
		numeric_traits::max_value<size_type> <= ((1_bi << (bounded::size_of_bits<T *> - 1_bi)) - 1_bi),
		"Maximum possible size is too large -- would use bit reserved for small-buffer optimization."
	);

	constexpr small_buffer_optimized_vector():
		m_small()
	{
	}
	
	constexpr explicit small_buffer_optimized_vector(constructor_initializer_range<small_buffer_optimized_vector> auto && source):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr small_buffer_optimized_vector(c_array<T, source_size> && source):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr small_buffer_optimized_vector(Source):
		small_buffer_optimized_vector()
	{
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector const & other):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, other);
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector && other) noexcept:
		small_buffer_optimized_vector()
	{
		move_assign_to_empty(std::move(other));
	}

	constexpr auto & operator=(small_buffer_optimized_vector const & other) & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}
	constexpr auto & operator=(small_buffer_optimized_vector && other) & noexcept {
		::containers::destroy_range(*this);
		deallocate_large();
		move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr ~small_buffer_optimized_vector() {
		::containers::destroy_range(*this);
		deallocate_large();
	}

	constexpr auto begin() const {
		auto const result = is_small() ? containers::begin(m_small) : containers::begin(m_large);
		return contiguous_iterator<T const, bounded::builtin_max_value<size_type>>(result);
	}
	constexpr auto begin() {
		auto const result = is_small() ? containers::begin(m_small) : containers::begin(m_large);
		return contiguous_iterator<T, bounded::builtin_max_value<size_type>>(result);
	}
	
	constexpr auto size() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.size(), m_large.size());
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto capacity() const {
		return BOUNDED_CONDITIONAL(is_small(), m_small.capacity(), m_large.capacity());
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		auto const new_size = size() + count;
		if (is_small()) {
			if constexpr (bounded::constructible_from<range_size_t<small_t>, decltype(new_size), bounded::unchecked_t>) {
				m_small.set_size(::bounded::assume_in_range<range_size_t<small_t>>(new_size));
			} else {
				std::unreachable();
			}
		} else {
			m_large.set_size(::bounded::assume_in_range<range_size_t<large_t>>(new_size));
		}
		BOUNDED_ASSERT(size() == new_size);
	}

	constexpr auto reserve(size_type const requested_capacity) -> void {
		if (requested_capacity <= capacity()) {
			return;
		}
		if (requested_capacity <= small_t::capacity()) {
			relocate_to_small();
		} else {
			relocate_to_large(requested_capacity);
		}
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	constexpr auto relocate_to_small() {
		if (is_small()) {
			return;
		}
		auto temp = std::move(m_large);
		auto const guard = bounded::scope_guard([&]{
			detail::deallocate_storage(detail::dynamic_array_data<T, size_type>(containers::data(temp), temp.capacity()));
		});
		// It is safe to skip the destructor call of m_large
		// because we do not rely on its side-effects
		::bounded::construct(m_small, []{ return small_t(); });
		containers::uninitialized_relocate_no_overlap(temp, containers::begin(m_small));
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
			detail::deallocate_storage(detail::dynamic_array_data<T, size_type>(containers::data(m_large), m_large.capacity()));
		}
	}
	
	constexpr auto move_assign_to_empty(small_buffer_optimized_vector && other) & {
		auto make_small = [](small_t & small) {
			::bounded::construct(small, bounded::construct_return<small_t>);
		};
		if (other.is_small()) {
			make_small(m_small);
			containers::uninitialized_relocate_no_overlap(other, containers::begin(m_small));
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
		auto temp = detail::allocate_storage<T, size_type>(requested_capacity);
		containers::uninitialized_relocate_no_overlap(*this, temp.pointer);
		deallocate_large();
		::bounded::construct(
			m_large,
			[&] {
				return large_t(
					size(),
					::bounded::assume_in_range<typename large_t::capacity_type>(temp.size),
					reinterpret_cast<T *>(temp.pointer)
				);
			}
		);
		BOUNDED_ASSERT(is_large());
	}


	static_assert(sizeof(small_t) >= sizeof(large_t), "Incorrect buffer sizes.");
	static_assert(std::is_standard_layout_v<small_t>);
	static_assert(std::is_standard_layout_v<large_t>);

	union {
		small_t m_small;
		large_t m_large;
	};
};

} // namespace containers
