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
#include <containers/maximum_array_size.hpp>
#include <containers/range_value_t.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace containers {

// TODO: max_size should be an array_size_type<T> instead of a size_t
template<typename T, std::size_t max_size = containers::detail::maximum_array_size<T>>
struct vector : private lexicographical_comparison::base {
	static_assert(max_size <= containers::detail::maximum_array_size<T>, "Cannot actually allocate that many elements");
	using value_type = T;
	using size_type = bounded::integer<0, bounded::normalize<max_size>>;

	using const_iterator = contiguous_iterator<value_type const, bounded::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::builtin_max_value<size_type>>;

	constexpr vector() = default;

	template<range Range> requires(
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit vector(Range && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr vector(c_array<T, source_size> && init) {
		::containers::assign_to_empty(*this, std::move(init));
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

	constexpr auto & operator=(vector && other) & noexcept {
		::containers::destroy_range(*this);
		m_storage = std::move(other.m_storage);
		m_size = other.m_size;
		other.m_size = 0_bi;
		return *this;
	}
	constexpr auto & operator=(vector const & other) & {
		if (!m_storage.data()) {
			BOUNDED_ASSERT(m_size == 0_bi);
			m_storage = storage_type();
		}
		containers::assign(*this, OPERATORS_FORWARD(other));
		return *this;
	}

	friend constexpr auto swap(vector & lhs, vector & rhs) noexcept {
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
		containers::uninitialized_relocate_no_overlap(
			*this,
			temp.data()
		);
		m_storage = std::move(temp);
		// m_size remains the same
	}

private:
	using storage_type = uninitialized_dynamic_array<T, size_type>;
	storage_type m_storage;
	size_type m_size = 0_bi;
};

template<typename Range>
vector(Range &&) -> vector<std::decay_t<range_value_t<Range>>>;

} // namespace containers
