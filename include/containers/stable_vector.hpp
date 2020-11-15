// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/assign.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/forward.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct stable_vector {
	using value_type = T;
	using size_type = bounded::integer<0, bounded::detail::normalize<capacity_>>;
	
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(capacity_)>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(capacity_)>;

	// Allocates the full capacity
	constexpr stable_vector() = default;

	template<range Range> requires(
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit stable_vector(Range && source) {
		::containers::append(*this, OPERATORS_FORWARD(source));
	}
	
	constexpr stable_vector(std::initializer_list<value_type> init) {
		::containers::append(*this, init);
	}
	
	// TODO: Support trivial relocatability
	constexpr stable_vector(stable_vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}
	constexpr stable_vector(stable_vector const & other) {
		::containers::append(*this, other);
	}

	constexpr ~stable_vector() {
		::containers::detail::destroy_range(*this);
	}

	constexpr auto & operator=(stable_vector && other) & noexcept {
		::containers::detail::destroy_range(*this);
		m_storage = std::move(other.m_storage);
		m_size = other.m_size;
		other.m_size = 0_bi;
		return *this;
	}
	constexpr auto & operator=(stable_vector const & other) & {
		assign_range(other);
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign_range(init);
		return *this;
	}

	friend constexpr auto swap(stable_vector & lhs, stable_vector & rhs) noexcept {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto begin() const & {
		return const_iterator(m_storage.data());
	}
	constexpr auto begin() & {
		return iterator(m_storage.data());
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}
	constexpr auto size() const {
		return m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	static constexpr auto capacity() {
		return bounded::constant<bounded::detail::normalize<capacity_>>;
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}

private:
	constexpr auto assign_range(auto && other) & {
		if (!m_storage.data()) {
			BOUNDED_ASSERT(m_size == 0_bi);
			m_storage = storage_type(capacity());
		}
		containers::assign(*this, other);
	}
	using storage_type = uninitialized_dynamic_array<T, decltype(capacity())>;
	storage_type m_storage{capacity()};
	[[no_unique_address]] size_type m_size = 0_bi;
};

}	// namespace containers