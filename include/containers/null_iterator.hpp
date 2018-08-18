// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

struct null_iterator {
	struct value_type {
		template<typename T>
		operator T() const noexcept {
			std::terminate();
		}
	};
	using difference_type = bounded::constant_t<0>;
	using iterator_category = std::random_access_iterator_tag;
	using pointer = value_type *;
	using reference = value_type &;
};

inline null_iterator::value_type & operator*(null_iterator) noexcept {
	std::terminate();
}

template<typename Integer>
constexpr auto operator+(null_iterator, Integer const &) noexcept {
	return null_iterator{};
}
constexpr auto operator-(null_iterator, null_iterator) noexcept {
	return bounded::constant<0>;
}
constexpr auto compare(null_iterator, null_iterator) noexcept {
	return bounded::strong_ordering_equal;
}

} // namespace detail
} // namespace containers

namespace std {

template<typename T>
struct common_type<containers::detail::null_iterator::value_type, T> {
	using type = T;
};

template<typename T>
struct common_type<T, containers::detail::null_iterator::value_type> {
	using type = T;
};

template<>
struct common_type<containers::detail::null_iterator::value_type, containers::detail::null_iterator::value_type> {
	using type = containers::detail::null_iterator::value_type;
};

} // namespace std
