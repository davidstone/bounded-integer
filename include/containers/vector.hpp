// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/assign.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/initializer_range.hpp>
#include <containers/is_container.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/range_value_t.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

namespace containers {

// TODO: max_size should be an array_size_type<T> instead of a size_t
template<typename T, std::size_t max_size = containers::detail::maximum_array_size<T>>
struct vector : private lexicographical_comparison::base {
	using size_type = bounded::integer<0, bounded::normalize<max_size>>;

	constexpr vector() = default;

	constexpr explicit vector(initializer_range<vector> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr vector(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr vector(Source) {
	}

	// TODO: Support trivial relocatability
	constexpr vector(vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}

	constexpr vector(vector const & other) {
		::containers::assign_to_empty(*this, other);
	}

	constexpr ~vector() {
		::containers::destroy_range(*this);
	}

	constexpr auto operator=(vector && other) & noexcept -> vector & {
		::containers::destroy_range(*this);
		m_storage = std::move(other.m_storage);
		m_size = other.m_size;
		other.m_size = 0_bi;
		return *this;
	}
	constexpr auto operator=(vector const & other) & -> vector & {
		if (this == std::addressof(other)) {
			return *this;
		}
		if (!m_storage.data()) {
			BOUNDED_ASSERT(m_size == 0_bi);
			m_storage = storage_type();
		}
		containers::assign(*this, other);
		return *this;
	}

	friend constexpr auto swap(vector & lhs, vector & rhs) noexcept -> void {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto begin() const {
		return contiguous_iterator<T const, max_size>(m_storage.data());
	}
	constexpr auto begin() {
		return contiguous_iterator<T, max_size>(m_storage.data());
	}
	constexpr auto size() const {
		return m_size;
	}
	
	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto capacity() const {
		return m_storage.capacity();
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto append_from_capacity(auto const count) -> void {
		BOUNDED_ASSERT(m_size + count <= capacity());
		m_size += count;
	}

	constexpr auto reserve(size_type const requested_capacity) -> void {
		if (requested_capacity <= capacity()) {
			return;
		}
		auto temp = storage_type(requested_capacity);
		containers::uninitialized_relocate_no_overlap(
			*this,
			temp.data()
		);
		m_storage = std::move(temp);
		// m_size remains the same
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	using storage_type = uninitialized_dynamic_array<T, size_type>;
	[[no_unique_address]] storage_type m_storage;
	[[no_unique_address]] size_type m_size = 0_bi;
};

template<typename Range>
vector(Range &&) -> vector<std::decay_t<range_value_t<Range>>>;

template<typename T, std::size_t max_size>
inline constexpr auto is_container<vector<T, max_size>> = true;

} // namespace containers
