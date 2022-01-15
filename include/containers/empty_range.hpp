// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <iterator>

namespace containers {
namespace detail {
using namespace bounded::literal;

template<typename T>
struct empty_iterator {
	empty_iterator() = default;
	
	constexpr decltype(auto) operator*() const {
		bounded::unreachable();
		using fptr = T(*)();
		return fptr()();
	}

	friend constexpr auto operator+(empty_iterator, bounded::constant_t<1>) {
		bounded::unreachable();
		return empty_iterator();
	}
	friend constexpr auto operator+(empty_iterator, bounded::constant_t<0>) {
		return empty_iterator();
	}

	friend constexpr auto operator-(empty_iterator, bounded::constant_t<0>) {
		return empty_iterator();
	}
	friend constexpr auto operator-(empty_iterator, empty_iterator) {
		return 0_bi;
	}

	friend auto operator<=>(empty_iterator, empty_iterator) = default;
};

} // namespace detail

template<typename T>
struct empty_range {
	static constexpr auto begin() {
		return detail::empty_iterator<T>();
	}
	static constexpr auto end() {
		return detail::empty_iterator<T>();
	}
};

} // namespace containers
