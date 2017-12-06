// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/overlapping_range.hpp>

#include <bounded/detail/class.hpp>

namespace {

using type = bounded::integer<0, 0>;
static_assert(bounded::detail::type_overlaps_range<type>(0, 0), "Type should overlap its own range.");
static_assert(bounded::detail::type_fits_in_range<type>(0, 0), "Type should fit in its own range.");
static_assert(!bounded::detail::type_overlaps_range<type>(1, 1), "Type should not overlap a disjoint range.");

}	// namespace
