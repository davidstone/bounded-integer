// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/common_type.hpp>

#include "../homogeneous_equals.hpp"

namespace {

using type1 = bounded::integer<1, 5>;
using type2 = bounded::integer<3, 10>;
using common_type2 = std::common_type_t<type1, type2>;
using expected_type2 = bounded::integer<1, 10>;
static_assert(is_same_bounded<expected_type2, common_type2>{}, "common_type wrong for 2 values.");
using type3 = bounded::integer<-5, -5>;
using common_type3 = std::common_type_t<type1, type2, type3>;
using expected_type3 = bounded::integer<-5, 10>;
static_assert(is_same_bounded<expected_type3, common_type3>{}, "common_type wrong for 3 values.");
using type4 = bounded::integer<0, 0>;
using common_type4 = std::common_type_t<type1, type2, type3, type4>;
using expected_type4 = bounded::integer<-5, 10>;
static_assert(is_same_bounded<expected_type4, common_type4>{}, "common_type wrong for 4 values.");

}	// namespace
