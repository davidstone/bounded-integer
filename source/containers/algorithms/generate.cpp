// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/generate.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>

#include <bounded/copy.hpp>
#include <bounded/value_to_function.hpp>

namespace {

using namespace bounded::literal;

static_assert(containers::equal(containers::generate_n(0_bi, bounded::value_to_function(5)), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::generate_n(1_bi, bounded::value_to_function(5)), containers::array{5}));
static_assert(containers::equal(containers::generate_n(2_bi, bounded::value_to_function(5)), containers::array{5, 5}));

static_assert(containers::equal(containers::indexed_generate_n(0_bi, bounded::copy), containers::array<bounded::constant_t<0>, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1_bi, bounded::copy), containers::array{0_bi}));
static_assert(containers::equal(containers::indexed_generate_n(2_bi, bounded::copy), containers::array{0_bi, 1_bi}));

static_assert(containers::equal(containers::indexed_generate_n(0, bounded::copy), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1, bounded::copy), containers::array{0}));
static_assert(containers::equal(containers::indexed_generate_n(2, bounded::copy), containers::array{0, 1}));

} // namespace