// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/policy/modulo_policy.hpp>

namespace check_modulo_policy {
constexpr bounded::modulo_policy policy;

static_assert(
	policy.assignment(bounded::constant<5>, bounded::constant<0>, bounded::constant<9>) == bounded::constant<5>,
	"Incorrect result for a minimum of zero and value in range."
);

static_assert(
	policy.assignment(bounded::constant<5>, -bounded::constant<5>, bounded::constant<9>) == bounded::constant<5>,
	"Incorrect result for a negative minimum and value in range."
);
static_assert(
	policy.assignment(-bounded::constant<5>, -bounded::constant<5>, bounded::constant<9>) == -bounded::constant<5>,
	"Incorrect result for a negative minimum and value equal to the minimum."
);

static_assert(
	policy.assignment(bounded::constant<17>, bounded::constant<0>, bounded::constant<9>) == bounded::constant<7>,
	"Incorrect result for a minimum of zero and value too high."
);

static_assert(
	policy.assignment(bounded::constant<1>, bounded::constant<2>, bounded::constant<9>) == bounded::constant<9>,
	"Incorrect result for a positive minimum and a positive value too low."
);

static_assert(
	policy.assignment(-bounded::constant<1>, bounded::constant<0>, bounded::constant<9>) == bounded::constant<9>,
	"Incorrect result for a minimum of zero and value too low."
);

static_assert(
	policy.assignment(bounded::constant<12>, -bounded::constant<5>, bounded::constant<9>) == -bounded::constant<3>,
	"Incorrect result for a negative minimum and value too high."
);

static_assert(
	policy.assignment(-bounded::constant<10>, -bounded::constant<5>, bounded::constant<9>) == bounded::constant<5>,
	"Incorrect result for a negative minimum and value too low."
);

static_assert(
	policy.assignment(-bounded::constant<1000>, bounded::constant<4>, bounded::constant<4>) == bounded::constant<4>,
	"Incorrect result for a single-value range and value too low."
);
static_assert(
	policy.assignment(bounded::constant<4>, bounded::constant<4>, bounded::constant<4>) == bounded::constant<4>,
	"Incorrect result for a single-value range and value in range."
);
static_assert(
	policy.assignment(bounded::constant<678412>, bounded::constant<4>, bounded::constant<4>) == bounded::constant<4>,
	"Incorrect result for a single-value range and value too high."
);

static_assert(
	policy.assignment(bounded::constant<0>, bounded::constant<0>, bounded::constant<0>) == bounded::constant<0>,
	"Incorrect result for a single-value range that can only hold 0 and a value of 0."
);

}	// namespace
