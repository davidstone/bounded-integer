// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/is_iterable.hpp>
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
constexpr auto all_qualifications_are_iterable_helper = (... and containers::is_iterable<Ts>);

template<typename T>
constexpr auto all_qualifications_are_iterable = all_qualifications_are_iterable_helper<T, T const, T &, T const &, T &&>;

static_assert(!containers::is_iterable<int>, "Incorrectly detects a non-iterable as iterable.");
static_assert(!containers::is_iterable<int *>, "Incorrectly detects a pointer as iterable.");

static_assert(containers::is_iterable<c_array<int, 5> &>, "Incorrectly detects c-arrays as non-iterable.");
static_assert(containers::is_iterable<c_array<int, 5> const &>, "Incorrectly detects c-arrays as non-iterable.");

static_assert(all_qualifications_are_iterable<containers::array<int, 5>>, "Incorrectly detects containers::array as non-iterable.");
static_assert(all_qualifications_are_iterable<containers::array<int, 0>>, "Incorrectly detects empty containers::array as non-iterable.");
static_assert(all_qualifications_are_iterable<containers::vector<int>>, "Incorrectly detects containers::vector as non-iterable.");
static_assert(all_qualifications_are_iterable<containers::string>, "Incorrectly detects containers::vector as non-iterable.");

static_assert(all_qualifications_are_iterable<std::array<int, 5>>, "Incorrectly detects std::array as non-iterable.");
static_assert(all_qualifications_are_iterable<std::array<int, 0>>, "Incorrectly detects empty std::array as non-iterable.");
static_assert(all_qualifications_are_iterable<std::vector<int>>, "Incorrectly detects std::vector as non-iterable.");
static_assert(all_qualifications_are_iterable<std::map<int, int>>, "Incorrectly detects std::map as non-iterable.");
static_assert(all_qualifications_are_iterable<std::unordered_set<int>>, "Incorrectly detects std::unordered_set as non-iterable.");

}	// namespace
