// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/concatenate_view.hpp>

#include <containers/array/array.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/repeat_n.hpp>
#include <containers/size.hpp>
#include <containers/value_type.hpp>

namespace {
using namespace bounded::literal;

template<typename LHS, typename RHS>
constexpr bool equal_values_and_types(LHS const & lhs, RHS const & rhs) {
	static_assert(std::is_same_v<containers::range_value_t<LHS>, containers::range_value_t<RHS>>);
	return containers::equal(lhs, rhs);
}

constexpr auto array1 = containers::array{1, 2, 3};
constexpr auto array2 = containers::array{2, 3, 5, 7};
constexpr auto array3 = containers::array{1, 1, 2, 3, 5};

constexpr auto two = containers::concatenate_view(array1, array2);

using iterator = decltype(begin(two));
using sentinel = decltype(end(two));

static_assert(containers::iterator<iterator>);
static_assert(containers::sentinel_for<sentinel, iterator>);
static_assert(containers::range<decltype(two)>);

static_assert(containers::size(two) == containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	two,
	containers::array{1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(two) + containers::size(two) == end(two));
static_assert(begin(two) + containers::size(two) - begin(two) == containers::size(two));

constexpr auto three = containers::concatenate_view(array3, array1, array2);
static_assert(containers::size(three) == containers::size(array3) + containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	three,
	containers::array{1, 1, 2, 3, 5, 1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(three) + containers::size(three) == end(three));
static_assert(begin(three) + containers::size(three) - begin(three) == containers::size(three));

static_assert(*(begin(three) + 7_bi) == 3);

constexpr auto from_temp = containers::concatenate_view(containers::array{1}, containers::array{2});
static_assert(equal_values_and_types(from_temp, containers::array{1, 2}));

static_assert(containers::equal(
	containers::concatenate_view(array1, containers::static_vector<int, 4>(containers::repeat_n(2_bi, 8))),
	containers::array{1, 2, 3, 8, 8}
));

} // namespace
