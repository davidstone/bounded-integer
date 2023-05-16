// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <map>
#include <set>

export module containers.is_container_test;

import containers.is_container;

import std_module;

static_assert(containers::is_container<std::array<int, 1>>);
static_assert(!containers::is_container<std::array<int, 1> &>);
static_assert(containers::is_container<std::string>);
static_assert(!containers::is_container<std::string &>);
static_assert(containers::is_container<std::vector<int>>);
static_assert(!containers::is_container<std::vector<int> &>);
static_assert(containers::is_container<std::set<int>>);
static_assert(!containers::is_container<std::set<int> &>);
static_assert(containers::is_container<std::map<int, int>>);
static_assert(!containers::is_container<std::map<int, int> &>);