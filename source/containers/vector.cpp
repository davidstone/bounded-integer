// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.vector;

import containers.algorithms.destroy_range;
import containers.algorithms.uninitialized;
import containers.assign;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.data;
import containers.initializer_range;
import containers.maximum_array_size;
import containers.range_value_t;
import containers.uninitialized_dynamic_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// TODO: max_size should be an array_size_type<T> instead of a size_t
export template<typename T, std::size_t max_size = containers::maximum_array_size<T>>
struct [[clang::trivial_abi]] vector : private lexicographical_comparison::base {
	template<typename U, std::size_t other_max_size>
	friend struct vector;

	using size_type = bounded::integer<0, bounded::normalize<max_size>>;

	constexpr vector() = default;

	constexpr explicit vector(constructor_initializer_range<vector> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size> requires(source_size <= max_size)
	constexpr vector(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr vector(Source) {
	}

	template<std::size_t other_max_size>
	constexpr explicit vector(vector<T, other_max_size> && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(bounded::assume_in_range<size_type>(std::exchange(other.m_size, 0_bi)))
	{
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
		m_size = other.m_size;
		other.m_size = 0_bi;
		m_storage = std::move(other.m_storage);
		return *this;
	}
	constexpr auto operator=(vector const & other) & -> vector & {
		if (this == std::addressof(other)) {
			return *this;
		}
		containers::assign(*this, other);
		return *this;
	}

	friend constexpr auto swap(vector & lhs, vector & rhs) noexcept -> void {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto begin() const {
		return contiguous_iterator<T const, bounded::constant<max_size>>(m_storage.data());
	}
	constexpr auto begin() {
		return contiguous_iterator<T, bounded::constant<max_size>>(m_storage.data());
	}
	constexpr auto size() const {
		return m_size;
	}
	
	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto capacity() const {
		return m_storage.capacity();
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto set_size(auto const new_size) -> void {
		BOUNDED_ASSERT(new_size <= capacity());
		m_size = new_size;
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

} // namespace containers
