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
constexpr auto test_sequence_container_default_constructed_empty() -> bool {
	auto const default_constructed = Container();
	BOUNDED_TEST(containers::begin(default_constructed) == containers::begin(default_constructed));
	BOUNDED_TEST(containers::begin(default_constructed) == containers::end(default_constructed));
	BOUNDED_TEST(containers::end(default_constructed) == containers::begin(default_constructed));
	BOUNDED_TEST(containers::end(default_constructed) == containers::end(default_constructed));
	BOUNDED_TEST(containers::detail::linear_size(default_constructed) == 0_bi);
	BOUNDED_TEST(default_constructed == default_constructed);
	BOUNDED_TEST(default_constructed == Container());
	BOUNDED_TEST(default_constructed == Container({}));
	Container const implicit_from_empty_braces = {};
	BOUNDED_TEST(default_constructed == implicit_from_empty_braces);
	Container const implicit_from_two_empty_braces = {{}};
	BOUNDED_TEST(default_constructed == implicit_from_two_empty_braces);
	return true;
}

template<typename Container>
constexpr auto test_range_constructor(auto const & source) -> bool {
	auto const container = Container(source);
	BOUNDED_TEST(containers::equal(container, source));
	return true;
}

constexpr auto test_copy_constructor(auto const & container) -> void {
	auto const copy = container;
	BOUNDED_TEST(copy == container);
}

constexpr auto test_move_constructor(auto const & container) -> void {
	auto copy = container;
	auto const move = std::move(copy);
	BOUNDED_TEST(move == container);
}

template<typename Container>
constexpr auto test_copy_assignment_from_empty(Container const & container) -> void {
	auto copy = Container();
	copy = container;
	BOUNDED_TEST(copy == container);
}
constexpr auto test_copy_assignment_from_non_empty(auto const & container) -> void {
	auto copy = container;
	copy = container;
	BOUNDED_TEST(copy == container);
}
template<typename Container>
constexpr auto test_copy_assignment_from_moved_from(Container const & container) -> void {
	auto copy = Container();
	[[maybe_unused]] auto const temp = std::move(copy);
	copy = container;
	BOUNDED_TEST(copy == container);
}
constexpr auto test_self_copy_assignment(auto const & container) -> void {
	auto copy = container;
	// Turn off compiler warning for self assignment
	auto const & ref = copy;
	copy = ref;
	BOUNDED_TEST(copy == container);
}
constexpr auto test_copy_assignment(auto const & container) -> void {
	test_copy_assignment_from_empty(container);
	test_copy_assignment_from_non_empty(container);
	test_copy_assignment_from_moved_from(container);
	test_self_copy_assignment(container);
}

template<typename Container>
constexpr auto test_move_assignment_from_empty(Container const & container) -> void {
	auto temp = container;
	auto move = Container();
	move = std::move(temp);
	BOUNDED_TEST(move == container);
}
constexpr auto test_move_assignment_from_non_empty(auto const & container) -> void {
	auto temp = container;
	auto move = container;
	move = std::move(temp);
	BOUNDED_TEST(move == container);
}
template<typename Container>
constexpr auto test_move_assignment_from_moved_from(Container const & container) -> void {
	auto move = container;
	auto temp = std::move(move);
	move = std::move(temp);
	BOUNDED_TEST(move == container);
}
template<typename T, typename Capacity>
constexpr auto validate_range(containers::uninitialized_dynamic_array<T, Capacity> const & container, auto const &) -> void {
	// Not anything useful to validate for a range of uninitialized memory,
	// except that forming the begin and end pointers are still valid.
	auto const last = container.data() + container.capacity();
}
template<typename Container>
constexpr auto test_recover_from_self_move(auto const & initializer, auto const & validate) -> void {
	auto container = Container(initializer);
	container = std::move(container);
	container = Container(initializer);
	BOUNDED_TEST(validate(container));
}
constexpr auto test_self_move_defined_when_already_moved_from(auto container) -> void {
	[[maybe_unused]] auto temp = std::move(container);
	container = std::move(container);
}
template<typename Container>
constexpr auto test_self_move_assignment(auto const & initializer, auto const & validate) -> bool {
	test_recover_from_self_move<Container>(initializer, validate);
	test_self_move_defined_when_already_moved_from(Container(initializer));
	return true;
}
template<typename Container>
constexpr auto test_move_assignment(Container const & container) -> void {
	test_move_assignment_from_empty(container);
	test_move_assignment_from_non_empty(container);
	test_move_assignment_from_moved_from(container);
	test_self_move_assignment<Container>(container, bounded::equal_to(container));
}

template<typename Container>
constexpr auto test_assignment_from_empty_braces(Container const & container) -> void {
	auto temp = container;
	temp = {};
	BOUNDED_TEST(temp == Container());
}

template<typename Container>
constexpr auto test_swap_with_empty(Container const & container) -> void {
	auto a = container;
	auto b = Container();
	using std::swap;
	swap(a, b);
	BOUNDED_TEST(b == container);
	BOUNDED_TEST(a == Container());
}
constexpr auto test_self_swap(auto container, auto const & validate) -> bool {
	using std::swap;
	swap(container, container);
	validate(container);
	return true;
}
template<typename Container>
constexpr auto test_swap(Container const & container) -> void {
	test_swap_with_empty(container);
	test_self_swap(container, bounded::equal_to(container));
}

constexpr auto test_special_members(auto const & container) -> void {
	test_copy_constructor(container);
	test_move_constructor(container);
	test_copy_assignment(container);
	test_move_assignment(container);
	test_assignment_from_empty_braces(container);
	test_swap(container);
}

template<typename Container>
constexpr auto test_reserve() -> bool {
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
constexpr auto test_sequence_container_single(std::initializer_list<containers::range_value_t<Container>> init) -> void {
	test_range_constructor<Container>(init);
	test_range_constructor<Container>(containers::range_view(init));
	test_special_members(Container(init));
}

template<typename Container>
constexpr auto test_sequence_container() -> bool {
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