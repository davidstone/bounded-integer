// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/is_range.hpp>
#include <containers/array/array.hpp>
#include <containers/string.hpp>
#include <containers/vector/vector.hpp>

#include <array>
#include <map>
#include <unordered_set>
#include <vector>

namespace {

template<typename T, std::size_t size>
using c_array = T[size];

template<typename... Ts>
constexpr auto all_qualifications_are_ranges_helper = (... and containers::is_range<Ts>);

template<typename T>
constexpr auto all_qualifications_are_ranges = all_qualifications_are_ranges_helper<T, T const, T &, T const &, T &&>;

static_assert(!containers::is_range<int>);
static_assert(!containers::is_range<int *>);

static_assert(containers::is_range<c_array<int, 5> &>, "Incorrectly detects c-arrays as non-ranges.");
static_assert(containers::is_range<c_array<int, 5> const &>, "Incorrectly detects c-arrays as non-ranges.");

static_assert(all_qualifications_are_ranges<containers::array<int, 5>>, "Incorrectly detects containers::array as a non-range.");
static_assert(all_qualifications_are_ranges<containers::array<int, 0>>, "Incorrectly detects empty containers::array as a non-range.");
static_assert(all_qualifications_are_ranges<containers::vector<int>>, "Incorrectly detects containers::vector as a non-range.");
static_assert(all_qualifications_are_ranges<containers::string>, "Incorrectly detects containers::string as a non-range.");

static_assert(all_qualifications_are_ranges<std::array<int, 5>>, "Incorrectly detects std::array as a non-range.");
static_assert(all_qualifications_are_ranges<std::array<int, 0>>, "Incorrectly detects empty std::array as a non-range.");
static_assert(all_qualifications_are_ranges<std::vector<int>>, "Incorrectly detects std::vector as a non-range.");
static_assert(all_qualifications_are_ranges<std::map<int, int>>, "Incorrectly detects std::map as a non-range.");
static_assert(all_qualifications_are_ranges<std::unordered_set<int>>, "Incorrectly detects std::unordered_set as a non-range.");

}	// namespace
