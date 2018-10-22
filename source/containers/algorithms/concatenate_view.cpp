// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/concatenate_view.hpp>

#include <containers/array/array.hpp>

namespace {

template<typename LHS, typename RHS>
constexpr bool equal_values_and_types(LHS const & lhs, RHS const & rhs) {
	static_assert(std::is_same_v<typename LHS::value_type, typename RHS::value_type>);
	return containers::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

constexpr auto array1 = containers::array{1, 2, 3};
constexpr auto array2 = containers::array{2, 3, 5, 7};
constexpr auto array3 = containers::array{1, 1, 2, 3, 5};

constexpr auto two = containers::concatenate_view(array1, array2);
static_assert(size(two) == size(array1) + size(array2));
static_assert(equal_values_and_types(
	two,
	containers::array{1, 2, 3, 2, 3, 5, 7}
));

constexpr auto three = containers::concatenate_view(array3, array1, array2);
static_assert(size(three) == size(array3) + size(array1) + size(array2));
static_assert(equal_values_and_types(
	three,
	containers::array{1, 1, 2, 3, 5, 1, 2, 3, 2, 3, 5, 7}
));

static_assert(*(begin(three) + bounded::constant<7>) == 3);

constexpr auto from_temp = containers::concatenate_view(containers::array{1}, containers::array{2});
static_assert(equal_values_and_types(from_temp, containers::array{1, 2}));

} // namespace
