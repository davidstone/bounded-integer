// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/insert.hpp>

#include <containers/repeat_n.hpp>
#include <containers/stable_vector.hpp>
#include <containers/vector.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

template<typename Container>
constexpr bool test_range_insert() {
	auto container = Container({ 1, 2, 3 });
	containers::insert(container, begin(container) + 1_bi, containers::repeat_n(3_bi, bounded::test_int(12)));
	auto const expected = { 1, 12, 12, 12, 2, 3 };
	BOUNDED_TEST(containers::equal(container, expected));
    return true;
}

static_assert(test_range_insert<containers::stable_vector<bounded::test_int, 10>>());
static_assert(test_range_insert<containers::vector<bounded::test_int>>());

template<typename Container>
constexpr bool test_value_insert() {
	auto container = Container({1, 2, 3, 4, 5, 6});
	containers::insert(container, begin(container) + 1_bi, 7);
	auto const expected = {1, 7, 2, 3, 4, 5, 6};
	BOUNDED_TEST(containers::equal(container, expected));
    return true;
}

static_assert(test_value_insert<containers::stable_vector<bounded::test_int, 10>>());
static_assert(test_value_insert<containers::vector<bounded::test_int>>());

struct non_trivial_non_copyable {
	constexpr non_trivial_non_copyable() {}
	constexpr non_trivial_non_copyable(non_trivial_non_copyable &&) noexcept {}
	non_trivial_non_copyable(non_trivial_non_copyable const &) = delete;
	constexpr non_trivial_non_copyable & operator=(non_trivial_non_copyable &&) noexcept { return *this; }
	non_trivial_non_copyable & operator=(non_trivial_non_copyable const &) = delete;
};

static_assert(!std::is_trivially_copyable_v<non_trivial_non_copyable>);

constexpr bool test_no_copies() {
	auto container = containers::vector<non_trivial_non_copyable>();
	containers::lazy_push_back(container, bounded::construct_return<non_trivial_non_copyable>);
	insert(container, begin(container), non_trivial_non_copyable{});
	return true;
}

static_assert(test_no_copies());

} // namespace