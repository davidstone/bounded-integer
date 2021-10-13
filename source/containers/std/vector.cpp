// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/std/vector.hpp>

#include "../../test_int.hpp"

namespace {

using namespace bounded::literal;

static_assert(containers::equal(std_containers::vector<bounded_test::integer>(), containers::vector<bounded_test::integer>()));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{}, containers::vector<bounded_test::integer>()));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{1}, containers::vector<bounded_test::integer>({1})));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{1, 4}, containers::vector<bounded_test::integer>({1, 4})));

static_assert(std_containers::vector<bounded_test::integer>() == std_containers::vector<bounded_test::integer>());
static_assert(std_containers::vector<bounded_test::integer>() == std_containers::vector<bounded_test::integer>{});
static_assert(std_containers::vector<bounded_test::integer>() != std_containers::vector<bounded_test::integer>{1});
static_assert(std_containers::vector<bounded_test::integer>{1} == std_containers::vector<bounded_test::integer>{1});

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	v.assign(4, 1);
	return v == std_containers::vector<bounded_test::integer>{1, 1, 1, 1};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	constexpr int input[] = {0, 3, 2, 1};
	v.assign(std::begin(input), std::end(input));
	return v == std_containers::vector<bounded_test::integer>{0, 3, 2, 1};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	v.assign({1, 3, 6, 3});
	return v == std_containers::vector<bounded_test::integer>{1, 3, 6, 3};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	auto const init = std_containers::vector<bounded_test::integer>();
	v.insert(v.end(), init.begin(), init.end());
	return true;
}());

static_assert([]{
	using vector = std_containers::vector<bounded_test::integer>;
	auto v = vector({1, 2});
	v.reserve(4);
	v.insert(v.begin(), v[1]);
	return v == vector({2, 1, 2});
}());

} // namespace
