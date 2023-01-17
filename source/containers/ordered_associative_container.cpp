// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <map>
#include <unordered_map>

export module containers.ordered_associative_container;

import containers.associative_container;
import containers.array;
import containers.c_array;
import containers.flat_map;
import containers.string;
import containers.vector;

import bounded;
import std_module;

namespace containers {

template<typename T>
concept has_key_comp = requires(T const & map) {
	map.key_comp();
};

template<typename T>
concept has_extract_key = requires(T const & map) {
	map.extract_key();
};

export template<typename T>
concept ordered_associative_range =
	associative_range<T> and
	(has_key_comp<T> or has_extract_key<T>);

export template<typename T>
concept ordered_associative_container = ordered_associative_range<T> and associative_container<T>;

} // namespace containers

using namespace bounded::literal;

template<typename... Ts>
constexpr auto all_qualifications_are_ordered_associative_helper = (... and containers::ordered_associative_range<Ts>);

template<typename T>
constexpr auto all_qualifications_are_ordered_associative = all_qualifications_are_ordered_associative_helper<T, T const, T &, T const &, T &&>;

template<typename... Ts>
constexpr auto no_qualifications_are_ordered_associative_helper = (... and !containers::ordered_associative_range<Ts>);

template<typename T>
constexpr auto no_qualifications_are_ordered_associative = no_qualifications_are_ordered_associative_helper<T, T const, T &, T const &, T &&>;

static_assert(no_qualifications_are_ordered_associative<int>);
static_assert(no_qualifications_are_ordered_associative<int *>);

static_assert(no_qualifications_are_ordered_associative<containers::c_array<int, 5>>);

static_assert(no_qualifications_are_ordered_associative<containers::array<int, 5_bi>>);
static_assert(no_qualifications_are_ordered_associative<containers::array<int, 0_bi>>);
static_assert(no_qualifications_are_ordered_associative<containers::vector<int>>);
static_assert(no_qualifications_are_ordered_associative<containers::string>);

static_assert(all_qualifications_are_ordered_associative<containers::flat_map<int, int>>);

static_assert(no_qualifications_are_ordered_associative<std::array<int, 5>>);
static_assert(no_qualifications_are_ordered_associative<std::array<int, 0>>);
static_assert(no_qualifications_are_ordered_associative<std::vector<int>>);
static_assert(all_qualifications_are_ordered_associative<std::map<int, int>>);
static_assert(no_qualifications_are_ordered_associative<std::unordered_map<int, int>>);
