// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.stable_vector;

import containers.algorithms.destroy_range;
import containers.assign;
import containers.assign_to_empty;
import containers.assign_to_empty_into_capacity;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.data;
import containers.initializer_range;
import containers.uninitialized_dynamic_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
export template<typename T, std::size_t capacity_>
struct [[clang::trivial_abi]] stable_vector : private lexicographical_comparison::base {
	// Allocates the full capacity
	constexpr stable_vector() = default;

	constexpr explicit stable_vector(constructor_initializer_range<stable_vector> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size> requires(source_size <= capacity_)
	constexpr stable_vector(c_array<T, source_size> && source) {
		::containers::assign_to_empty_into_capacity(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr stable_vector(Source) {
	}
	
	// TODO: Support trivial relocatability
	constexpr stable_vector(stable_vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}
	constexpr stable_vector(stable_vector const & other) {
		::containers::assign_to_empty_into_capacity(*this, other);
	}

	constexpr ~stable_vector() {
		::containers::destroy_range(*this);
	}

	constexpr auto operator=(stable_vector && other) & noexcept -> stable_vector & {
		::containers::destroy_range(*this);
		m_storage = std::move(other.m_storage);
		m_size = other.m_size;
		other.m_size = 0_bi;
		return *this;
	}
	constexpr auto operator=(stable_vector const & other) & -> stable_vector & {
		if (this == std::addressof(other)) {
			return *this;
		}
		if (!m_storage.data()) {
			BOUNDED_ASSERT(m_size == 0_bi);
			m_storage = storage_type(capacity());
		}
		containers::assign(*this, other);
		return *this;
	}

	friend constexpr auto swap(stable_vector & lhs, stable_vector & rhs) noexcept -> void {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto begin() const {
		return contiguous_iterator<T const, bounded::constant<capacity_>>(m_storage.data());
	}
	constexpr auto begin() {
		return contiguous_iterator<T, bounded::constant<capacity_>>(m_storage.data());
	}
	constexpr auto size() const {
		return m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	static constexpr auto capacity() {
		return bounded::constant<capacity_>;
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto set_size(auto const new_size) -> void {
		BOUNDED_ASSERT(new_size <= capacity());
		m_size = new_size;
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	using storage_type = uninitialized_dynamic_array<T, decltype(capacity())>;
	storage_type m_storage{capacity()};
	[[no_unique_address]] bounded::integer<0, bounded::normalize<capacity_>> m_size = 0_bi;
};

} // namespace containers
