// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/normalize.hpp>

#include <bounded/detail/class.hpp>

#include <numeric_traits/min_max_value.hpp>

#include "homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(bounded::normalize<0>, 0));
static_assert(homogeneous_equals(bounded::normalize<1>, 1));
static_assert(homogeneous_equals(bounded::normalize<-1>, -1));
static_assert(homogeneous_equals(bounded::normalize<0U>, 0));
static_assert(homogeneous_equals(bounded::normalize<static_cast<unsigned char>(0)>, 0));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::max_value<numeric_traits::max_unsigned_t>>, numeric_traits::max_value<numeric_traits::max_unsigned_t>));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::max_value<numeric_traits::max_signed_t>>, numeric_traits::max_value<numeric_traits::max_signed_t>));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::min_value<numeric_traits::max_signed_t>>, numeric_traits::min_value<numeric_traits::max_signed_t>));
static_assert(homogeneous_equals(bounded::normalize<bounded::constant<0>>, 0));

} // namespace