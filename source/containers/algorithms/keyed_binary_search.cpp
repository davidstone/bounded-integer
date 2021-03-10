// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/keyed_binary_search.hpp>

#include <containers/static_vector/static_vector.hpp>
#include <containers/flat_map.hpp>

#include <containers/algorithms/compare.hpp>

namespace {

using namespace bounded::literal;

template<std::size_t size>
using map_type = containers::basic_flat_multimap<containers::static_vector<containers::map_value_type<int, int>, size>>;

constexpr auto zero = map_type<0>();
static_assert(containers::keyed_lower_bound(zero, 0) == end(zero));
static_assert(containers::keyed_upper_bound(zero, 0) == end(zero));

constexpr auto one = map_type<1>({{1, 0}});
static_assert(containers::keyed_lower_bound(one, 0) == begin(one));
static_assert(containers::keyed_lower_bound(one, 1) == begin(one));
static_assert(containers::keyed_lower_bound(one, 2) == end(one));
static_assert(containers::keyed_upper_bound(one, 0) == begin(one));
static_assert(containers::keyed_upper_bound(one, 1) == end(one));
static_assert(containers::keyed_upper_bound(one, 2) == end(one));

constexpr auto two = map_type<2>({{1, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(two, 0) == begin(two) + 0_bi);
static_assert(containers::keyed_lower_bound(two, 1) == begin(two) + 0_bi);
static_assert(containers::keyed_lower_bound(two, 2) == begin(two) + 1_bi);
static_assert(containers::keyed_lower_bound(two, 3) == end(two));
static_assert(containers::keyed_upper_bound(two, 0) == begin(two) + 0_bi);
static_assert(containers::keyed_upper_bound(two, 1) == begin(two) + 1_bi);
static_assert(containers::keyed_upper_bound(two, 2) == begin(two) + 2_bi);
static_assert(containers::keyed_upper_bound(two, 3) == end(two));

constexpr auto three = map_type<3>({{1, 0}, {2, 0}, {3, 0}});
static_assert(containers::keyed_lower_bound(three, 0) == begin(three) + 0_bi);
static_assert(containers::keyed_lower_bound(three, 1) == begin(three) + 0_bi);
static_assert(containers::keyed_lower_bound(three, 2) == begin(three) + 1_bi);
static_assert(containers::keyed_lower_bound(three, 3) == begin(three) + 2_bi);
static_assert(containers::keyed_lower_bound(three, 4) == end(three));
static_assert(containers::keyed_upper_bound(three, 0) == begin(three) + 0_bi);
static_assert(containers::keyed_upper_bound(three, 1) == begin(three) + 1_bi);
static_assert(containers::keyed_upper_bound(three, 2) == begin(three) + 2_bi);
static_assert(containers::keyed_upper_bound(three, 3) == begin(three) + 3_bi);
static_assert(containers::keyed_upper_bound(three, 4) == end(three));

constexpr auto two_duplicates = map_type<2>({{1, 0}, {1, 0}});
static_assert(containers::keyed_lower_bound(two_duplicates, 0) == begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(two_duplicates, 1) == begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(two_duplicates, 2) == end(two_duplicates));
static_assert(containers::keyed_upper_bound(two_duplicates, 0) == begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_upper_bound(two_duplicates, 1) == end(two_duplicates));
static_assert(containers::keyed_upper_bound(two_duplicates, 2) == end(two_duplicates));

constexpr auto three_duplicates = map_type<3>({{1, 0}, {1, 0}, {1, 0}});
static_assert(containers::keyed_lower_bound(three_duplicates, 0) == begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(three_duplicates, 1) == begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(three_duplicates, 2) == end(three_duplicates));
static_assert(containers::keyed_upper_bound(three_duplicates, 0) == begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_upper_bound(three_duplicates, 1) == end(three_duplicates));
static_assert(containers::keyed_upper_bound(three_duplicates, 2) == end(three_duplicates));

constexpr auto three_two_duplicates_first = map_type<3>({{1, 0}, {1, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 0) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 1) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 2) == begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 3) == end(three_two_duplicates_first));
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 0) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 1) == begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 2) == begin(three_two_duplicates_first) + 3_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 3) == end(three_two_duplicates_first));

constexpr auto three_two_duplicates_last = map_type<3>({{1, 0}, {2, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 0) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 1) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 2) == begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 3) == end(three_two_duplicates_last));
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 0) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 1) == begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 2) == begin(three_two_duplicates_last) + 3_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 3) == end(three_two_duplicates_last));

} // namespace