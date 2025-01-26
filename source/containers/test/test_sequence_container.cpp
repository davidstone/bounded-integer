// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#undef NDEBUG
#include <bounded/assert.hpp>

export module containers.test.test_sequence_container;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.forward_range;
import containers.is_container;
import containers.iterator;
import containers.linear_size;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers_test {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_forward_range_concepts() -> bool {
	static_assert(!containers::iterator<Container const &>);
	static_assert(!containers::iterator<Container &>);
	static_assert(!containers::iterator<Container &&>);
	static_assert(!containers::iterator<Container>);
	static_assert(containers::forward_range<Container const &>);
	static_assert(containers::forward_range<Container &>);
	static_assert(containers::forward_range<Container &&>);
	static_assert(containers::forward_range<Container>);
	return true;
}

// Because it would be really embarrassing if this didn't work...
export template<typename Container>
constexpr auto test_range_based_for_loop() {
	auto const ccontainer = Container();
	for (auto const & _ : ccontainer) {
	}
	auto mcontainer = Container();
	for (auto & _ : mcontainer) {
	}
	for (auto && _ : Container()) {
	}
}

export template<typename Container>
constexpr auto test_sequence_container_default_constructed_empty() -> bool {
	auto const default_constructed = Container();
	BOUNDED_ASSERT(containers::begin(default_constructed) == containers::begin(default_constructed));
	BOUNDED_ASSERT(containers::begin(default_constructed) == containers::end(default_constructed));
	BOUNDED_ASSERT(containers::end(default_constructed) == containers::begin(default_constructed));
	BOUNDED_ASSERT(containers::end(default_constructed) == containers::end(default_constructed));
	BOUNDED_ASSERT(containers::linear_size(default_constructed) == 0_bi);
	BOUNDED_ASSERT(default_constructed == default_constructed);
	BOUNDED_ASSERT(default_constructed == Container());
	BOUNDED_ASSERT(default_constructed == Container({}));
	Container const implicit_from_empty_braces = {};
	BOUNDED_ASSERT(default_constructed == implicit_from_empty_braces);
	return true;
}

export template<typename Container>
constexpr auto test_sequence_container_implicit_from_two_empty_braces() -> bool {
	Container const c = {{}};
	BOUNDED_ASSERT(c == Container());
	return true;
}

template<typename Container>
constexpr auto test_range_constructor(auto const make) -> void {
	BOUNDED_ASSERT(containers::equal(Container(make()), make()));
}

template<typename Container>
constexpr auto test_move_constructor(auto const make) -> void {
	auto source = Container(make());
	auto const target = std::move(source);
	BOUNDED_ASSERT(containers::equal(target, make()));
}

template<typename Container>
constexpr auto test_copy_constructor(auto const make) -> void {
	auto const source = Container(make());
	auto const target = source;
	BOUNDED_ASSERT(containers::equal(target, make()));
}

template<typename Container>
constexpr auto test_copy_constructor_from_mutable(auto const make) -> void {
	auto source = Container(make());
	auto const target = source;
	BOUNDED_ASSERT(containers::equal(target, make()));
}

template<typename Container>
constexpr auto test_copy_assignment_from_empty(auto const make) -> void {
	auto target = Container();
	target = Container(make());
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_copy_assignment_from_non_empty(auto const make) -> void {
	auto const source = Container(make());
	auto target = Container(make());
	target = source;
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_default_copy_assignment_from_empty() -> void {
	auto const source = Container();
	auto target = Container();
	target = source;
	BOUNDED_ASSERT(containers::equal(target, Container()));
}
template<typename Container>
constexpr auto test_default_copy_assignment_from_non_empty(auto const make) -> void {
	auto const source = Container();
	auto target = Container(make());
	target = source;
	BOUNDED_ASSERT(containers::equal(target, Container()));
}
template<typename Container>
constexpr auto test_copy_assignment_from_move_constructed_from(auto const make) -> void {
	auto target = Container(make());
	auto const source = std::move(target);
	target = source;
	BOUNDED_ASSERT(containers::equal(target, make()));
	BOUNDED_ASSERT(containers::equal(target, source));
}
template<typename Container>
constexpr auto test_copy_assignment_from_move_assigned_from(auto const make) -> void {
	auto target = Container(make());
	auto source = Container(make());
	source = std::move(target);
	target = source;
	BOUNDED_ASSERT(containers::equal(target, make()));
	BOUNDED_ASSERT(containers::equal(target, source));
}
template<typename Container>
constexpr auto test_self_copy_assignment(auto const make) -> void {
	auto copy = Container(make());
	// Turn off compiler warning for self assignment
	auto const & ref = copy;
	copy = ref;
	BOUNDED_ASSERT(containers::equal(copy, make()));
}
template<typename Container>
constexpr auto test_copy_assignment(auto const make) -> void {
	if constexpr (bounded::default_constructible<Container>) {
		test_copy_assignment_from_empty<Container>(make);
		test_default_copy_assignment_from_empty<Container>();
		test_default_copy_assignment_from_non_empty<Container>(make);
	}
	test_copy_assignment_from_non_empty<Container>(make);
	test_copy_assignment_from_move_constructed_from<Container>(make);
	test_copy_assignment_from_move_assigned_from<Container>(make);
	test_self_copy_assignment<Container>(make);
}

template<typename Container>
constexpr auto test_move_assignment_from_empty(auto const make) -> void {
	auto temp = Container(make());
	auto target = Container();
	target = std::move(temp);
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_move_assignment_from_non_empty(auto const make) -> void {
	auto temp = Container(make());
	auto target = Container(make());
	target = std::move(temp);
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_move_assignment_from_move_constructed_from(auto const make) -> void {
	auto target = Container(make());
	auto temp = std::move(target);
	target = std::move(temp);
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_move_assignment_from_move_assigned_from(auto const make) -> void {
	auto target = Container(make());
	auto temp = Container(make());
	temp = std::move(target);
	target = std::move(temp);
	BOUNDED_ASSERT(containers::equal(target, make()));
}
template<typename Container>
constexpr auto test_recover_from_self_move(auto const make, auto const & validate) -> void {
	auto container = Container(make());
	container = std::move(container);
	container = Container(make());
	BOUNDED_ASSERT(validate(container));
}
constexpr auto test_self_move_defined_when_already_moved_from(auto container) -> void {
	[[maybe_unused]] auto temp = std::move(container);
	container = std::move(container);
}
export template<typename Container>
constexpr auto test_self_move_assignment(auto const make, auto const & validate) -> bool {
	test_recover_from_self_move<Container>(make, validate);
	test_self_move_defined_when_already_moved_from(Container(make()));
	return true;
}
template<typename Container>
constexpr auto test_move_assignment(auto const make) -> void {
	if constexpr (bounded::default_constructible<Container>) {
		test_move_assignment_from_empty<Container>(make);
	}
	test_move_assignment_from_non_empty<Container>(make);
	test_move_assignment_from_move_constructed_from<Container>(make);
	test_move_assignment_from_move_assigned_from<Container>(make);
	test_self_move_assignment<Container>(make, [&](Container const & container) { return containers::equal(container, make()); });
}

template<typename Container>
constexpr auto test_assignment_from_empty_braces(auto const make) -> void {
	auto temp = Container(make());
	temp = {};
	BOUNDED_ASSERT(temp == Container());
}

template<typename Container>
constexpr auto test_swap_with_empty(Container const & container) -> void {
	auto a = container;
	auto b = Container();
	using std::swap;
	swap(a, b);
	BOUNDED_ASSERT(b == container);
	BOUNDED_ASSERT(a == Container());
}
export constexpr auto test_self_swap(auto container, auto const & validate) -> bool {
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

export template<typename Container>
constexpr auto test_special_members(auto const make) -> bool {
	using value_type = containers::range_value_t<Container>;
	test_move_constructor<Container>(make);
	test_move_assignment<Container>(make);
	if constexpr (bounded::copy_constructible<value_type>) {
		test_copy_constructor<Container>(make);
		test_copy_constructor_from_mutable<Container>(make);
		if constexpr (bounded::copy_assignable<value_type>) {
			test_copy_assignment<Container>(make);
		}
	}
	if constexpr (bounded::default_constructible<Container>) {
		test_assignment_from_empty_braces<Container>(make);
	}
	test_swap(make);
	return true;
}

export template<typename Container>
constexpr auto test_sequence_container_from(auto const make) -> void {
	test_range_constructor<Container>(make);
	test_special_members<Container>(make);
}

export template<typename Container>
constexpr auto test_sequence_container() -> bool {
	static_assert(containers::is_container<Container>);
	test_forward_range_concepts<Container>();

	if constexpr (bounded::default_constructible<Container>) {
		test_range_based_for_loop<Container>();

		test_sequence_container_default_constructed_empty<Container>();
		test_sequence_container_implicit_from_two_empty_braces<Container>();

		test_sequence_container_from<Container>([] {
			return containers::to_array<containers::range_value_t<Container>>({});
		});
	}
	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({5});
	});
	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({0, 1, 4});
	});
	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		});
	});

	return true;
}

} // namespace containers_test