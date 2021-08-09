// Copyright David Stone 2019.
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
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/forward.hpp>

#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
template<typename T, std::size_t capacity_>
struct stable_vector : private lexicographical_comparison::base {
	// Allocates the full capacity
	constexpr stable_vector() = default;

	constexpr explicit stable_vector(initializer_range<stable_vector> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr stable_vector(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
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
		::containers::assign_to_empty(*this, other);
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
		if (std::addressof(other) == this) {
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

	constexpr auto begin() const & {
		return contiguous_iterator<T const, static_cast<std::ptrdiff_t>(capacity_)>(m_storage.data());
	}
	constexpr auto begin() & {
		return contiguous_iterator<T, static_cast<std::ptrdiff_t>(capacity_)>(m_storage.data());
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}
	constexpr auto size() const {
		return m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	static constexpr auto capacity() {
		return bounded::constant<bounded::normalize<capacity_>>;
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto append_from_capacity(auto const count) -> void {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}

	constexpr operator std::span<T const>() const & {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() & {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	using storage_type = uninitialized_dynamic_array<T, decltype(capacity())>;
	storage_type m_storage{capacity()};
	[[no_unique_address]] bounded::integer<0, bounded::normalize<capacity_>> m_size = 0_bi;
};

} // namespace containers