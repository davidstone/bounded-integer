// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.associative_container;

import containers.array;
import containers.associative_container;
import containers.c_array;
import containers.string;
import containers.vector;

import bounded;
import std_module;

namespace {

using namespace bounded::literal;

template<typename... Ts>
constexpr auto all_qualifications_are_map_helper = (... and containers::map_range<Ts>);

template<typename T>
constexpr auto all_qualifications_are_map = all_qualifications_are_map_helper<T, T const, T &, T const &, T &&>;

template<typename... Ts>
constexpr auto no_qualifications_are_map_helper = (... and !containers::map_range<Ts>);

template<typename T>
constexpr auto no_qualifications_are_map = no_qualifications_are_map_helper<T, T const, T &, T const &, T &&>;

template<typename... Ts>
constexpr auto all_qualifications_are_set_helper = (... and containers::set_range<Ts>);

template<typename T>
constexpr auto all_qualifications_are_set = all_qualifications_are_set_helper<T, T const, T &, T const &, T &&>;

template<typename... Ts>
constexpr auto no_qualifications_are_set_helper = (... and !containers::set_range<Ts>);

template<typename T>
constexpr auto no_qualifications_are_set = no_qualifications_are_set_helper<T, T const, T &, T const &, T &&>;

static_assert(no_qualifications_are_map<int>);
static_assert(no_qualifications_are_map<int *>);
static_assert(no_qualifications_are_set<int>);
static_assert(no_qualifications_are_set<int *>);

static_assert(no_qualifications_are_map<containers::c_array<int, 5>>);
static_assert(no_qualifications_are_set<containers::c_array<int, 5>>);

static_assert(no_qualifications_are_map<containers::array<int, 5_bi>>);
static_assert(no_qualifications_are_map<containers::array<int, 0_bi>>);
static_assert(no_qualifications_are_map<containers::vector<int>>);
static_assert(no_qualifications_are_map<containers::string>);

static_assert(no_qualifications_are_set<containers::array<int, 5_bi>>);
static_assert(no_qualifications_are_set<containers::array<int, 0_bi>>);
static_assert(no_qualifications_are_set<containers::vector<int>>);
static_assert(no_qualifications_are_set<containers::string>);

static_assert(no_qualifications_are_map<std::array<int, 5>>);
static_assert(no_qualifications_are_map<std::array<int, 0>>);
static_assert(no_qualifications_are_map<std::vector<int>>);
static_assert(all_qualifications_are_map<std::map<int, int>>);
static_assert(no_qualifications_are_map<std::set<int, int>>);
static_assert(all_qualifications_are_map<std::unordered_map<int, int>>);
static_assert(no_qualifications_are_map<std::unordered_set<int, int>>);

static_assert(no_qualifications_are_set<std::array<int, 5>>);
static_assert(no_qualifications_are_set<std::array<int, 0>>);
static_assert(no_qualifications_are_set<std::vector<int>>);
static_assert(no_qualifications_are_set<std::map<int, int>>);
static_assert(all_qualifications_are_set<std::set<int, int>>);
static_assert(no_qualifications_are_set<std::unordered_map<int, int>>);
static_assert(all_qualifications_are_set<std::unordered_set<int, int>>);

}	// namespace
