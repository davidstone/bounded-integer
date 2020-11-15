// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/assign.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T>
struct vector {
	using value_type = T;
	using size_type = detail::array_size_type<T>;

	using const_iterator = contiguous_iterator<value_type const, bounded::detail::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::detail::builtin_max_value<size_type>>;

	constexpr vector() = default;

	template<range Range> requires(
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit vector(Range && source) {
		::containers::append(*this, OPERATORS_FORWARD(source));
	}
	
	constexpr vector(std::initializer_list<T> init) {
		::containers::append(*this, init);
	}

	// TODO: Support trivial relocatability
	constexpr vector(vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}

	constexpr vector(vector const & other) {
		::containers::append(*this, other);
	}

	constexpr ~vector() {
		::containers::detail::destroy_range(*this);
	}

	constexpr auto & operator=(vector && other) & noexcept {
		::containers::detail::destroy_range(*this);
		m_storage = std::move(other.m_storage);
		m_size = other.m_size;
		other.m_size = 0_bi;
		return *this;
	}
	constexpr auto & operator=(vector const & other) & {
		assign_range(other);
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign_range(init);
		return *this;
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

	constexpr auto capacity() const {
		return m_storage.capacity();
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		BOUNDED_ASSERT(m_size + count <= capacity());
		m_size += count;
	}

	constexpr auto reserve(size_type const requested_capacity) {
		if (requested_capacity <= capacity()) {
			return;
		}
		auto temp = storage_type(requested_capacity);
		containers::uninitialized_move_destroy(
			*this,
			temp.data()
		);
		m_storage = std::move(temp);
		// m_size remains the same
	}

private:
	constexpr auto assign_range(auto && other) & {
		if (!m_storage.data()) {
			BOUNDED_ASSERT(m_size == 0_bi);
			m_storage = storage_type();
		}
		containers::assign(*this, other);
	}
	
	using storage_type = uninitialized_dynamic_array<T, size_type>;
	storage_type m_storage;
	size_type m_size = 0_bi;
};

template<typename Range>
vector(Range &&) -> vector<std::decay_t<typename std::decay_t<Range>::value_type>>;

}	// namespace containers
