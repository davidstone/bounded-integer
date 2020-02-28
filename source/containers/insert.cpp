// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/insert.hpp>

#include <containers/static_vector/static_vector.hpp>
#include <containers/repeat_n.hpp>
#include <containers/stable_vector.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

template<typename Container>
constexpr bool test_insert() {
	auto container = Container({ 1, 2, 3 });
	containers::insert(container, begin(container) + 1_bi, containers::repeat_n(3_bi, 12));
	auto const expected = { 1, 12, 12, 12, 2, 3 };
	BOUNDED_TEST(containers::equal(container, expected));
    return true;
}

static_assert(test_insert<containers::static_vector<int, 10>>());
static_assert(test_insert<containers::stable_vector<int, 10>>());

struct non_trivial_non_copyable {
	constexpr non_trivial_non_copyable() {}
	non_trivial_non_copyable(non_trivial_non_copyable const &) = delete;
	non_trivial_non_copyable(non_trivial_non_copyable &&) = default;
	non_trivial_non_copyable & operator=(non_trivial_non_copyable const &) = delete;
	non_trivial_non_copyable & operator=(non_trivial_non_copyable &&) = default;
};

[[maybe_unused]] void test_no_copies() {
	auto container = containers::static_vector<non_trivial_non_copyable, 10>();
	containers::lazy_push_back(container, bounded::construct_return<non_trivial_non_copyable>);
	insert(container, begin(container), non_trivial_non_copyable{});
}

} // namespace