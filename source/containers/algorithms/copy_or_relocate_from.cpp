// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/copy_or_relocate_from.hpp>

#include <containers/vector.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

static_assert([] {
	using container_t = containers::vector<bounded_test::integer>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		container,
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(container == original);
	BOUNDED_TEST(result == original);
	return true;
}());

static_assert([] {
	using container_t = containers::vector<bounded_test::integer>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		std::move(container),
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(containers::is_empty(container));
	BOUNDED_TEST(result == original);
	return true;
}());

static_assert([] {
	using container_t = containers::vector<bounded_test::integer>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		containers::range_view(container),
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(container == original);
	BOUNDED_TEST(result == original);
	return true;
}());


static_assert([] {
	using container_t = containers::vector<int>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		container,
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(container == original);
	BOUNDED_TEST(result == original);
	return true;
}());

static_assert([] {
	using container_t = containers::vector<int>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		std::move(container),
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(container == original);
	BOUNDED_TEST(result == original);
	return true;
}());

static_assert([] {
	using container_t = containers::vector<int>;
	auto const original = container_t({1, 2});
	auto container = original;
	auto result = container_t();
	containers::copy_or_relocate_from(
		containers::range_view(container),
		[&](auto function) { containers::lazy_push_back(result, function); }
	);
	BOUNDED_TEST(container == original);
	BOUNDED_TEST(result == original);
	return true;
}());

} // namespace