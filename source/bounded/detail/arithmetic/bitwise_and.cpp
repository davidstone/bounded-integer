// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/bitwise_and.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
#include <bounded/detail/class.hpp>

namespace {

static_assert((bounded::constant<17> & bounded::constant<1>) == bounded::constant<1>);
static_assert((bounded::constant<18> & bounded::constant<1>) == bounded::constant<0>);

static_assert((bounded::constant<17> & 1) == 1);
static_assert((bounded::constant<18> & 1) == 0);

}	// namespace
