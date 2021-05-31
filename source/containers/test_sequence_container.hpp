// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/assign.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_empty.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/shrink_to_fit.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace containers_test {

using namespace bounded::literal;

template<typename Container>
constexpr bool test_sequence_container_default_constructed_empty() {
	auto const default_constructed = Container();
	BOUNDED_TEST(containers::begin(default_constructed) == containers::begin(default_constructed));
	BOUNDED_TEST(containers::begin(default_constructed) == containers::end(default_constructed));
	BOUNDED_TEST(containers::end(default_constructed) == containers::begin(default_constructed));
	BOUNDED_TEST(containers::end(default_constructed) == containers::end(default_constructed));
	BOUNDED_TEST(containers::size(default_constructed) == 0_bi);
	BOUNDED_TEST(default_constructed == default_constructed);
	BOUNDED_TEST(default_constructed == Container());
	BOUNDED_TEST(default_constructed == Container({}));
	Container implicit_from_empty_braces = {};
	BOUNDED_TEST(default_constructed == implicit_from_empty_braces);
	Container implicit_from_two_empty_braces = {{}};
	BOUNDED_TEST(default_constructed == implicit_from_two_empty_braces);
	return true;
}

template<typename Container>
constexpr bool test_range_constructor(auto const & source) {
	auto const container = Container(source);
	BOUNDED_TEST(containers::equal(container, source));
	return true;
}

constexpr void validate_range(auto const & container, auto const & initializer) {
	BOUNDED_TEST(containers::equal(container, initializer));
}
template<typename T, typename Capacity>
constexpr void validate_range(containers::uninitialized_dynamic_array<T, Capacity> const & container, auto const &) {
	auto const last = container.data() + container.capacity();
	for (auto ptr = container.data(); ptr != last; ++ptr) {
	}
}

// Self move assignment should have well-defined effects, regardless of whether
// the source is moved from.
template<typename Container>
constexpr auto test_self_move_assignment(auto const & initializer) {
	auto a = Container(initializer);
	a = std::move(a);
	a = Container(initializer);
	validate_range(a, initializer);

	[[maybe_unused]] auto b = std::move(a);
	a = std::move(a);

	return true;
}

template<typename Container>
constexpr auto test_self_swap(auto const & initializer) {
	auto a = Container(initializer);
	using std::swap;
	swap(a, a);
	validate_range(a, initializer);
	return true;
}

template<typename Container>
constexpr bool test_special_members(auto const & initializer) {
	test_self_move_assignment<Container>(initializer);
	test_self_swap<Container>(initializer);

	auto const container = Container(initializer);

	auto a = container;
	BOUNDED_TEST(container == a);

	auto b = std::move(a);
	BOUNDED_TEST(container == b);

	a = b;
	BOUNDED_TEST(container == a);
	BOUNDED_TEST(container == b);

	b = std::move(a);
	BOUNDED_TEST(container == b);

	a = container;
	BOUNDED_TEST(container == a);

	a = {};
	BOUNDED_TEST(a == Container());

	using std::swap;
	swap(a, b);
	BOUNDED_TEST(container == a);
	BOUNDED_TEST(b == Container());
	return true;
}

template<typename Container>
constexpr bool test_assign(auto const & source) {
	if constexpr (containers::resizable_container<Container>) {
		auto container = Container();
		containers::assign(container, source);
		BOUNDED_ASSERT(containers::equal(container, source));
		return true;
	} else {
		return true;
	}
}

template<typename Container>
constexpr auto test_reserve() {
	auto v = Container();
	auto const capacity0 = v.capacity();
	BOUNDED_TEST(capacity0 >= 0_bi);
	BOUNDED_TEST(containers::size(v) == 0_bi);
	v.reserve(10_bi);
	auto const capacity10 = v.capacity();
	BOUNDED_TEST(capacity10 >= 10_bi);
	BOUNDED_TEST(capacity10 >= capacity0);
	BOUNDED_TEST(containers::size(v) == 0_bi);
	shrink_to_fit(v);
	BOUNDED_TEST(containers::size(v) == 0_bi);
	BOUNDED_TEST(v.capacity() == capacity0);
	v.reserve(10_bi);
	BOUNDED_TEST(containers::size(v) == 0_bi);
	BOUNDED_TEST(v.capacity() == capacity10);
	containers::push_back(v, {});
	BOUNDED_TEST(containers::size(v) == 1_bi);
	BOUNDED_TEST(v.capacity() == capacity10);
	shrink_to_fit(v);
	BOUNDED_TEST(containers::size(v) == 1_bi);
	BOUNDED_TEST(v.capacity() >= capacity0);
	BOUNDED_TEST(v.capacity() <= capacity10);
	return true;
}

template<typename Container>
constexpr auto test_sequence_container_single(std::initializer_list<containers::range_value_t<Container>> init) {
	test_range_constructor<Container>(init);
	test_range_constructor<Container>(containers::range_view(init));
	test_special_members<Container>(init);
	test_assign<Container>(init);
}

template<typename Container>
constexpr auto test_sequence_container() {
	static_assert(!containers::iterator<Container>);
	static_assert(containers::iterator<typename Container::const_iterator>);
	static_assert(containers::iterator<typename Container::iterator>);

	test_sequence_container_default_constructed_empty<Container>();

	if constexpr (containers::detail::reservable<Container>) {
		test_reserve<Container>();
	}
	test_sequence_container_single<Container>({});
	test_sequence_container_single<Container>({5});
	test_sequence_container_single<Container>({0, 1, 4});
	test_sequence_container_single<Container>({0, 1, 2, 3, 4});
	test_sequence_container_single<Container>({
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	});

	return true;
}

} // namespace containers_test