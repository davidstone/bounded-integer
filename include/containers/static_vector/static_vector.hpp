// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>
#include <containers/array/array.hpp>
#include <containers/array/iterator.hpp>

#include <bounded/integer_range.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

// literal types cannot have destructors. If T is trivially destructible, we
// can skip calling their destructors, which could allow static_vector<T, n> to
// be a literal type.
//
// This should also improve debug-mode performance.

template<typename T, std::size_t capacity, bool trivial = std::is_trivially_destructible<T>::value>
struct static_vector_data;

template<typename T, std::size_t capacity>
struct static_vector_data<T, capacity, true> {
	array<trivial_storage<T>, capacity> m_container = {{}};
	bounded::integer<0, capacity> m_size = 0_bi;
};

template<typename T, std::size_t capacity>
struct static_vector_data<T, capacity, false> : static_vector_data<T, capacity, true> {
	~static_vector_data() {
		auto const first = begin(this->m_container);
		auto const last = first + this->m_size;
		::containers::detail::destroy(allocator<T>{}, first, last);
	}
};


}	// namespace detail

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector : private detail::static_vector_data<T, capacity_> {
	using value_type = T;
	using size_type = bounded::integer<0, capacity_>;
	using const_iterator = detail::basic_array_iterator<value_type const, static_vector>;
	using iterator = detail::basic_array_iterator<value_type, static_vector>;

	constexpr static_vector() = default;
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr explicit static_vector(Count const count) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count)
	) {}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr static_vector(Count const count, value_type const & value) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count, value)
	) {}
	
	template<typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator, Sentinel>)>
	constexpr static_vector(InputIterator first, Sentinel const last) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(emplace_back(*first))) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	
	constexpr static_vector(static_vector const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(other), end(other))
	) {}
	constexpr static_vector(static_vector && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(std::move(other)), end(std::move(other)))
	) {}

	constexpr static_vector(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(init), end(init))
	) {}

	constexpr auto & operator=(static_vector const & other) & noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable<value_type>::value) {
		assign(*this, begin(std::move(other)), end(std::move(other)));
		return *this;
	}
	constexpr auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<static_vector &>(), containers::begin(init), containers::end(init)))) {
		assign(*this, begin(init), end(init));
		return *this;
	}


	friend constexpr auto begin(static_vector const & container) noexcept {
		return const_iterator(data(container.m_container), detail::iterator_constructor);
	}
	friend constexpr auto begin(static_vector & container) noexcept {
		return iterator(data(container.m_container), detail::iterator_constructor);
	}
	friend constexpr auto end(static_vector const & container) noexcept {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(static_vector & container) noexcept {
		return begin(container) + container.m_size;
	}


	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS
	
	static constexpr auto capacity() noexcept {
		return bounded::constant<capacity_>;
	}

	template<typename... Args>
	constexpr auto emplace_back(Args && ... args) {
		assert(size(*this) != capacity());
		::containers::detail::construct(get_allocator(), data(*this) + size(*this), std::forward<Args>(args)...);
		++this->m_size;
	}
	
	template<typename... Args>
	constexpr auto emplace(const_iterator const position, Args && ... args) {
		if (position == end(*this)) {
			emplace_back(std::forward<Args>(args)...);
		} else {
			detail::emplace_in_middle_no_reallocation(*this, position, get_allocator(), std::forward<Args>(args)...);
		}
		return detail::mutable_iterator(*this, position);
	}
	template<typename ForwardIterator, typename Sentinel>
	constexpr auto insert(const_iterator const position, ForwardIterator first, Sentinel last) {
		if (position == end(*this)) {
			return append(*this, first, last);
		}

		auto const range_size = bounded::throw_policy<std::out_of_range>{}.assignment(
			::containers::distance(first, last),
			0_bi,
			static_vector::size_type::max()
		);
		
		auto const mutable_position = detail::put_in_middle_no_reallocation(*this, position, first, last, range_size, get_allocator());
		this->m_size += range_size;
		return mutable_position;
	}
	
	constexpr auto pop_back() {
		::containers::detail::destroy(get_allocator(), ::bounded::addressof(back(*this)));
		--this->m_size;
	}


private:
	static constexpr auto get_allocator() noexcept {
		return allocator<value_type>{};
	}

	enum class count_constructor{};
	template<typename Count, typename... MaybeInitializer>
	constexpr explicit static_vector(count_constructor, Count const count, MaybeInitializer && ... args) noexcept(std::is_nothrow_constructible<value_type, MaybeInitializer && ...>::value) {
		static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
		for (auto const n : bounded::integer_range(count)) {
			static_cast<void>(n);
			emplace_back(std::forward<MaybeInitializer>(args)...);
		}
	}
};


}	// namespace containers
