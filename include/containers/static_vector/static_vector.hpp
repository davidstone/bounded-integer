// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/array/array.hpp>
#include <containers/append.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/pop_back.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/concepts.hpp>

#include <operators/bracket.hpp>

namespace containers {
namespace detail {

template<typename T, std::size_t capacity_, bool = std::is_trivially_destructible_v<T>>
struct static_vector_data {
	using value_type = T;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::ptrdiff_t>(capacity_)>
	>;
	using const_iterator = contiguous_iterator<T const, static_cast<std::ptrdiff_t>(capacity_)>;
	using iterator = contiguous_iterator<T, static_cast<std::ptrdiff_t>(capacity_)>;

	static_vector_data() = default;

	static_vector_data(static_vector_data &&) requires std::is_trivially_move_constructible_v<T> = default;
	static_vector_data(static_vector_data const &) requires std::is_trivially_copy_constructible_v<T> = default;
	static_vector_data & operator=(static_vector_data &&) & requires std::is_trivially_move_assignable_v<T> = default;
	static_vector_data & operator=(static_vector_data const &) & requires std::is_trivially_copy_assignable_v<T> = default;

	constexpr static_vector_data(static_vector_data && other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		std::uninitialized_move(other.begin(), other.end(), begin());
		this->m_size = other.m_size;
	}
	constexpr static_vector_data(static_vector_data const & other) {
		containers::uninitialized_copy(other, begin());
		this->m_size = other.m_size;
	}

	constexpr auto & operator=(static_vector_data && other) & noexcept(std::is_nothrow_move_assignable_v<T> and std::is_nothrow_move_constructible_v<T>) {
		assign(*this, std::move(other));
		return *this;
	}
	constexpr auto & operator=(static_vector_data const & other) & {
		assign(*this, other);
		return *this;
	}

	constexpr explicit static_vector_data(range auto && source) {
		::containers::append(*this, OPERATORS_FORWARD(source));
	}
	
	constexpr static_vector_data(std::initializer_list<T> init) {
		::containers::append(*this, init);
	}
	
	constexpr auto begin() const & {
		return const_iterator(::containers::detail::static_or_reinterpret_cast<T const *>(::containers::data(m_storage)));
	}
	constexpr auto begin() & {
		return iterator(::containers::detail::static_or_reinterpret_cast<T *>(::containers::data(m_storage)));
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}

	constexpr auto end() const & {
		return begin() + m_size;
	}
	constexpr auto end() & {
		return begin() + m_size;
	}
	constexpr auto end() && {
		return std::move(*this).begin() + m_size;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
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

	array<trivial_storage<T>, capacity_> m_storage = {};
	size_type m_size = 0_bi;
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
		::containers::detail::destroy_range(*this);
	}
};

} // namespace detail

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector : private detail::static_vector_data<T, capacity_> {
private:
	using base = detail::static_vector_data<T, capacity_>;
public:

	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using base::base;
	using base::operator=;
	
	using base::operator[];

	using base::begin;
	using base::end;

	using base::capacity;
	using base::append_from_capacity;
};

}	// namespace containers
