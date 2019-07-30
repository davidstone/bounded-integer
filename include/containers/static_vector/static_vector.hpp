// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/array/array.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/emplace_back.hpp>
#include <containers/index_type.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

namespace containers {

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector {
	static constexpr auto capacity() noexcept {
		return bounded::constant<bounded::detail::normalize<capacity_>>;
	}

	using value_type = T;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::ptrdiff_t>(capacity_)>
	>;
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(capacity())>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(capacity())>;

	constexpr static_vector() = default;
	
	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit static_vector(Range && range) {
		for (decltype(auto) value : BOUNDED_FORWARD(range)) {
			::containers::emplace_back(*this, BOUNDED_FORWARD(value));
		}
	}
	
	constexpr static_vector(std::initializer_list<value_type> init) {
		for (auto const & value : init) {
			::containers::emplace_back(*this, value);
		}
	}
	
	static_vector(static_vector && other) requires(std::is_trivially_move_constructible_v<value_type>) = default;
	static_vector(static_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) {
		std::uninitialized_move(begin(other), end(other), begin(*this));
		m_size = other.m_size;
	}
	static_vector(static_vector const & other) requires(std::is_trivially_copy_constructible_v<value_type>) = default;
	static_vector(static_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) {
		std::uninitialized_copy(begin(other), end(other), begin(*this));
		m_size = other.m_size;
	}

	~static_vector() requires(std::is_trivially_destructible_v<static_vector>) = default;
	~static_vector() {
		detail::destroy_range(*this);
	}

	static_vector & operator=(static_vector && other) & requires(std::is_trivially_move_assignable_v<value_type>) = default;
	auto & operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable_v<T>) {
		assign(*this, begin(std::move(other)), end(std::move(other)));
		return *this;
	}

	static_vector & operator=(static_vector const & other) & requires(std::is_trivially_copy_assignable_v<value_type>) = default;
	auto & operator=(static_vector const & other) & noexcept(std::is_nothrow_copy_assignable_v<T>) {
		assign(*this, begin(other), end(other));
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<static_vector &>(), containers::begin(init), containers::end(init)))) {
		assign(*this, begin(init), end(init));
		return *this;
	}


	friend constexpr auto begin(static_vector const & container) noexcept {
		return const_iterator(detail::static_or_reinterpret_cast<T const *>(data(container.m_storage)));
	}
	friend constexpr auto begin(static_vector & container) noexcept {
		return iterator(detail::static_or_reinterpret_cast<T *>(data(container.m_storage)));
	}

	friend constexpr auto end(static_vector const & container) noexcept {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(static_vector & container) noexcept {
		return begin(container) + container.m_size;
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(static_vector)
	
	// Assumes that elements are already constructed in the spare capacity
	template<typename Integer>
	constexpr void append_from_capacity(Integer const count) noexcept {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}

private:
	array<trivial_storage<T>, capacity_> m_storage = {};
	size_type m_size = 0_bi;
};

static_assert(std::is_trivially_copy_constructible_v<static_vector<int, 15>>);
static_assert(std::is_trivially_move_constructible_v<static_vector<int, 15>>);
static_assert(std::is_trivially_copy_assignable_v<static_vector<int, 15>>);
static_assert(std::is_trivially_move_assignable_v<static_vector<int, 15>>);
static_assert(std::is_trivially_destructible_v<static_vector<int, 15>>);

template<typename T, std::size_t capacity>
inline constexpr auto is_container<static_vector<T, capacity>> = true;

}	// namespace containers
