// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/array/array.hpp>
#include <containers/assign.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/data.hpp>
#include <containers/initializer_range.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/pop_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/size.hpp>
#include <containers/uninitialized_array.hpp>

#include <bounded/concepts.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/bracket.hpp>

#include <span>

namespace containers {
namespace detail {

template<typename T, std::size_t capacity_, bool = std::is_trivially_destructible_v<T>>
struct static_vector_data : private lexicographical_comparison::base {
	static_vector_data() = default;

	static_vector_data(static_vector_data &&) requires std::is_trivially_move_constructible_v<T> = default;
	static_vector_data(static_vector_data const &) requires std::is_trivially_copy_constructible_v<T> = default;
	static_vector_data & operator=(static_vector_data &&) & requires std::is_trivially_move_assignable_v<T> = default;
	static_vector_data & operator=(static_vector_data const &) & requires std::is_trivially_copy_assignable_v<T> = default;

	constexpr static_vector_data(static_vector_data && other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		// Ensure that a move is not more expensive than a copy
		if constexpr (std::is_trivially_copyable_v<T>) {
			containers::assign_to_empty(*this, std::move(other));
		} else {
			containers::uninitialized_relocate_no_overlap(other, begin());
			this->m_size = std::exchange(other.m_size, 0_bi);
		}
	}
	constexpr static_vector_data(static_vector_data const & other) {
		containers::assign_to_empty(*this, other);
	}

	constexpr auto & operator=(static_vector_data && other) & noexcept(std::is_nothrow_move_assignable_v<T> and std::is_nothrow_move_constructible_v<T>) {
		containers::assign(*this, std::move(other));
		return *this;
	}
	constexpr auto & operator=(static_vector_data const & other) & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}

	constexpr explicit static_vector_data(initializer_range<static_vector_data> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr static_vector_data(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	constexpr static_vector_data(empty_c_array_parameter) {
	}
	
	constexpr auto begin() const & {
		using const_iterator = contiguous_iterator<T const, static_cast<std::ptrdiff_t>(capacity_)>;
		return const_iterator(m_storage.data());
	}
	constexpr auto begin() & {
		using iterator = contiguous_iterator<T, static_cast<std::ptrdiff_t>(capacity_)>;
		return iterator(m_storage.data());
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}

	constexpr auto size() const {
		return m_size;
	}

	static constexpr auto capacity() {
		return bounded::constant<capacity_>;
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}

	[[no_unique_address]] uninitialized_array<T, capacity_> m_storage = {};
	[[no_unique_address]] bounded::integer<0, bounded::normalize<capacity_>> m_size = 0_bi;
};

template<typename T, std::size_t capacity>
struct static_vector_data<T, capacity, false> : static_vector_data<T, capacity, true> {
private:
	using base = static_vector_data<T, capacity, true>;
public:

	using base::base;
	using base::operator=;

	static_vector_data(static_vector_data &&) = default;
	static_vector_data(static_vector_data const &) = default;
	static_vector_data & operator=(static_vector_data &&) & = default;
	static_vector_data & operator=(static_vector_data const &) & = default;

	constexpr ~static_vector_data() {
		::containers::destroy_range(*this);
	}
};

} // namespace detail

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector : private detail::static_vector_data<T, capacity_> {
private:
	using base = detail::static_vector_data<T, capacity_>;
public:

	using base::base;
	
	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	using base::begin;
	using base::size;

	using base::capacity;
	using base::append_from_capacity;

	constexpr operator std::span<T const>() const & {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() & {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}
};

template<range Source>
constexpr auto make_static_vector(Source && source) {
	using value_type = range_value_t<Source>;
	constexpr auto size = static_cast<std::size_t>(numeric_traits::max_value<range_size_t<Source>>);
	return static_vector<value_type, size>(OPERATORS_FORWARD(source));
}

} // namespace containers
