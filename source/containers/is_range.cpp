// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <unordered_map>
#include <unordered_set>

export module containers.is_range;

import containers.begin_end;
import containers.c_array;
import containers.is_iterator_sentinel;

import bounded;
import std_module;

namespace containers {

export template<typename Range>
using iterator_t_impl = decltype(containers::begin(bounded::declval<Range>()));

export template<typename Range>
using sentinel_t = decltype(containers::end(bounded::declval<Range>()));

export template<typename T>
concept range = sentinel_for<sentinel_t<T>, iterator_t_impl<T>>;

export template<typename T>
concept forward_range = range<T> and forward_iterator<iterator_t_impl<T>>;

export template<typename T>
concept bidirectional_range = forward_range<T> and bidirectional_iterator<iterator_t_impl<T>>;

export template<typename T>
concept forward_random_access_range = forward_range<T> and forward_random_access_iterator<iterator_t_impl<T>>;

export template<typename T>
concept random_access_range = bidirectional_range<T> and forward_random_access_range<T>;

} // namespace containers

using namespace bounded::literal;

template<typename... Ts>
concept all_qualifications_are_ranges_helper = (... and containers::range<Ts>);

template<typename T>
concept all_qualifications_are_ranges = all_qualifications_are_ranges_helper<T, T const, T &, T const &, T &&>;

static_assert(!containers::range<int>);
static_assert(!containers::range<int *>);

static_assert(containers::range<containers::c_array<int, 5> &>, "Incorrectly detects c-arrays as non-ranges.");
static_assert(containers::range<containers::c_array<int, 5> const &>, "Incorrectly detects c-arrays as non-ranges.");

static_assert(all_qualifications_are_ranges<std::array<int, 5>>, "Incorrectly detects std::array as a non-range.");
static_assert(all_qualifications_are_ranges<std::array<int, 0>>, "Incorrectly detects empty std::array as a non-range.");
static_assert(all_qualifications_are_ranges<std::vector<int>>, "Incorrectly detects std::vector as a non-range.");
static_assert(all_qualifications_are_ranges<std::unordered_map<int, int>>, "Incorrectly detects std::unordered_map as a non-range.");
static_assert(all_qualifications_are_ranges<std::unordered_set<int>>, "Incorrectly detects std::unordered_set as a non-range.");
