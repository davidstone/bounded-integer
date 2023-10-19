// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.associative_container;

import containers.array;
import containers.c_array;
import containers.is_container;
import containers.range;
import containers.string;
import containers.vector;

import bounded;
import std_module;

namespace containers {

export template<typename T>
concept associative_range =
	forward_range<T> and
	requires {
		typename std::remove_reference_t<T>::key_type;
		typename std::remove_reference_t<T>::mapped_type;
	};

export template<typename T>
concept associative_container = associative_range<T> and is_container<T>;

} // namespace containers

namespace {

using namespace bounded::literal;

template<typename... Ts>
constexpr auto all_qualifications_are_associative_helper = (... and containers::associative_range<Ts>);

template<typename T>
constexpr auto all_qualifications_are_associative = all_qualifications_are_associative_helper<T, T const, T &, T const &, T &&>;

template<typename... Ts>
constexpr auto no_qualifications_are_associative_helper = (... and !containers::associative_range<Ts>);

template<typename T>
constexpr auto no_qualifications_are_associative = no_qualifications_are_associative_helper<T, T const, T &, T const &, T &&>;

static_assert(no_qualifications_are_associative<int>);
static_assert(no_qualifications_are_associative<int *>);

static_assert(no_qualifications_are_associative<containers::c_array<int, 5>>);

static_assert(no_qualifications_are_associative<containers::array<int, 5_bi>>);
static_assert(no_qualifications_are_associative<containers::array<int, 0_bi>>);
static_assert(no_qualifications_are_associative<containers::vector<int>>);
static_assert(no_qualifications_are_associative<containers::string>);

static_assert(no_qualifications_are_associative<std::array<int, 5>>);
static_assert(no_qualifications_are_associative<std::array<int, 0>>);
static_assert(no_qualifications_are_associative<std::vector<int>>);
static_assert(all_qualifications_are_associative<std::map<int, int>>);
static_assert(all_qualifications_are_associative<std::unordered_map<int, int>>);

}	// namespace
