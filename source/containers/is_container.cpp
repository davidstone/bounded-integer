// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/is_container.hpp>

#include <array>
#include <map>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

static_assert(!containers::is_container<void>);
static_assert(!containers::is_container<int>);
static_assert(!containers::is_container<std::span<int>>);
static_assert(!containers::is_container<std::string_view>);
static_assert(containers::is_container<std::array<int, 1>>);
static_assert(containers::is_container<std::string>);
static_assert(containers::is_container<std::vector<int>>);
static_assert(containers::is_container<std::set<int>>);
static_assert(containers::is_container<std::map<int, int>>);

} // namespace