// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.generate;

import containers.algorithms.compare;
import containers.algorithms.generate;
import containers.array;

import bounded;

using namespace bounded::literal;

static_assert(containers::equal(containers::generate_n(0_bi, bounded::value_to_function(5)), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::generate_n(1_bi, bounded::value_to_function(5)), containers::array{5}));
static_assert(containers::equal(containers::generate_n(2_bi, bounded::value_to_function(5)), containers::array{5, 5}));

static_assert(containers::equal(containers::generate_n_idempotent(0_bi, bounded::value_to_function(5)), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::generate_n_idempotent(1_bi, bounded::value_to_function(5)), containers::array{5}));
static_assert(containers::equal(containers::generate_n_idempotent(2_bi, bounded::value_to_function(5)), containers::array{5, 5}));

static_assert(containers::equal(containers::indexed_generate_n(0_bi, bounded::copy), containers::array<bounded::constant_t<0>, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1_bi, bounded::copy), containers::array{0_bi}));
static_assert(containers::equal(containers::indexed_generate_n(2_bi, bounded::copy), containers::array{0_bi, 1_bi}));

static_assert(containers::equal(containers::indexed_generate_n(0, bounded::copy), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1, bounded::copy), containers::array{0}));
static_assert(containers::equal(containers::indexed_generate_n(2, bounded::copy), containers::array{0, 1}));