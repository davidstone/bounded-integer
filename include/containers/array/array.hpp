// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/common_functions.hpp>

#include <bounded/integer.hpp>

#include <operators/bracket.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T, std::size_t size, std::size_t... sizes>
struct array;

namespace detail {

template<typename T, std::size_t... sizes>
struct array_value_type {
	using type = array<T, sizes...>;
};
template<typename T>
struct array_value_type<T> {
	using type = T;
};

struct monostate{};

template<std::size_t size>
struct array_trait {
	template<typename T>
	using type = T[size];
};

template<>
struct array_trait<0> {
	template<typename>
	using type = monostate;
};

template<typename T, std::size_t size>
using array_type = typename array_trait<size>::template type<T>;

}	// namespace detail

template<typename T, std::size_t size_, std::size_t... sizes>
struct array {
	static_assert(size_ <= static_cast<std::size_t>(bounded::max_value<std::ptrdiff_t>));
	using value_type = typename detail::array_value_type<T, sizes...>::type;

	using size_type = bounded::constant_t<bounded::normalize<size_>>;
	
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(size_)>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(size_)>;

	constexpr auto begin() const & {
		if constexpr (size() != 0) {
			return const_iterator(m_value);
		} else {
			return const_iterator();
		}
	}
	constexpr auto begin() & {
		if constexpr (size() != 0) {
			return iterator(m_value);
		} else {
			return iterator();
		}
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}
	static constexpr auto size() {
		return bounded::constant<size_>;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator<=>(array const &, array const &) = default;

	// Consider this private. It must be public for the class to be an
	// aggregate
	[[no_unique_address]] detail::array_type<value_type, size_> m_value;
};

template<typename... Args>
array(Args && ...) -> array<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>;

}	// namespace containers
