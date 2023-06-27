// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.empty_range;

import containers.common_functions;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename T>
struct empty_iterator {
	empty_iterator() = default;
	
	constexpr decltype(auto) operator*() const {
		std::unreachable();
		using fptr = T(*)();
		return fptr()();
	}

	friend constexpr auto operator+(empty_iterator, bounded::constant_t<1>) {
		std::unreachable();
		return empty_iterator();
	}
	friend constexpr auto operator+(empty_iterator, bounded::constant_t<-1>) {
		std::unreachable();
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

export template<typename T>
struct empty_range {
	static constexpr auto begin() {
		return empty_iterator<T>();
	}
	static constexpr auto end() {
		return empty_iterator<T>();
	}
};

} // namespace containers

constexpr auto range = containers::empty_range<int>();

static_assert(range.begin() == range.end());

static_assert(requires(containers::empty_iterator<int> it) {
	++it;
	it - it;
	it + 0_bi;
});