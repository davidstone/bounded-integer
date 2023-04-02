// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

#include "../../test_assert.hpp"

export module containers.algorithms.concatenate;

import containers.algorithms.copy;
import containers.algorithms.reverse_iterator;
import containers.algorithms.uninitialized;
import containers.append;
import containers.begin_end;
import containers.can_set_size;
import containers.data;
import containers.is_iterator;
import containers.iterator_t;
import containers.iter_difference_t;
import containers.range_value_t;
import containers.range_view;
import containers.resizable_container;
import containers.resize;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Result>
constexpr auto concatenate_prepend_append(Result &, iterator_t<Result &>) {
	std::unreachable();
}

// This assumes that the beginning of the result is full of unused memory, and
// all elements in result are already where they belong.
template<typename Result>
constexpr auto concatenate_prepend_append(Result & result, iterator_t<Result &> const it, auto && range, auto && ... ranges) {
	if constexpr (std::same_as<Result, std::remove_reference_t<decltype(range)>>) {
		if (std::addressof(result) == std::addressof(range)) {
			(..., ::containers::append(result, OPERATORS_FORWARD(ranges)));
			return;
		}
	}
	auto const next_it = ::containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(range), it);
	::containers::concatenate_prepend_append(result, next_it, OPERATORS_FORWARD(ranges)...);
}

template<typename Result, typename Integer>
struct reusable_concatenate_t {
	Result * ptr;
	Integer before_size;
};

template<typename Result, typename Integer, typename Range>
constexpr auto reusable_concatenate_container(reusable_concatenate_t<Result, Integer> result, auto const total_size, Range && range) {
	if (result.ptr) {
		return result;
	}
	if constexpr (std::same_as<Result, Range>) {
		if (range.capacity() >= total_size) {
			return reusable_concatenate_t<Result, Integer>{std::addressof(range), result.before_size};
		}
	}
	result.before_size += size(range);
	return result;
}

// Adding up a bunch of sizes leads to overflow in bounds
template<typename Size>
constexpr auto ugly_size_hack(Size const size) {
	if constexpr (std::integral<Size>) {
		using result_t = bounded::integer<
			0,
			bounded::normalize<bounded::min(numeric_traits::max_value<Size>, numeric_traits::max_value<std::uintmax_t> / 64)>
		>;
		return ::bounded::assume_in_range<result_t>(size);
	} else {
		return size;
	}
}

template<typename Container>
concept can_reuse_storage =
	can_set_size<Container> or
	(std::is_trivial_v<range_value_t<Container>> and resizable_container<Container>);

template<typename Container>
constexpr auto move_existing_data_to_final_position(Container & container, auto const before_size) {
	if constexpr (can_set_size<Container>) {
		// Use data instead of begin to construct an iterator after `end()`.
		auto const new_begin = containers::data(container) + before_size;
		::containers::uninitialized_relocate(containers::reversed(container), containers::reverse_iterator(new_begin + ::containers::size(container)));
		container.set_size(containers::size(container) + before_size);
	} else {
		static_assert(std::is_trivial_v<range_value_t<Container>> and resizable_container<Container>);
		using difference_type = iter_difference_t<iterator_t<Container &>>;
		auto const original_size = bounded::integer(::containers::size(container));
		auto const new_size = original_size + before_size;
		::containers::resize(container, new_size);
		::containers::copy_backward(
			containers::range_view(containers::begin(container), containers::begin(container) + static_cast<difference_type>(original_size)),
			containers::begin(container) + static_cast<difference_type>(new_size)
		);
	}
}

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
export template<typename Result>
constexpr auto concatenate(auto && ... ranges) -> Result {
	auto const total_size = (0_bi + ... + ::containers::ugly_size_hack(::containers::size(ranges)));

	if constexpr (can_reuse_storage<Result>) {
		using Integer = bounded::integer<0, bounded::builtin_max_value<decltype(total_size)>>;
		auto reusable = reusable_concatenate_t<Result, Integer>{nullptr, 0_bi};
		(..., (reusable = ::containers::reusable_concatenate_container(reusable, total_size, OPERATORS_FORWARD(ranges))));
		if (reusable.ptr) {
			auto & ref = *reusable.ptr;
			if (reusable.before_size != 0_bi) {
				::containers::move_existing_data_to_final_position(ref, reusable.before_size);
			}
			::containers::concatenate_prepend_append(ref, containers::begin(ref), OPERATORS_FORWARD(ranges)...);
			return std::move(ref);
		}
	}

	auto result = Result();
	result.reserve(::bounded::assume_in_range<range_size_t<Result>>(total_size));
	(..., ::containers::append(result, OPERATORS_FORWARD(ranges)));
	return result;
}

} // namespace containers

constexpr bool test() {
	using ::containers::begin;
	using ::containers::end;
	using Container = containers::vector<bounded_test::integer>;

	auto const a = Container({2, 6, 8});
	auto const b = Container({3, 5});
	auto const c = Container({1, 2, 3});
	
	auto reserve_extra_capacity = [&](auto container) {
		container.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(containers::size(a) + containers::size(b) + containers::size(c)));
		return container;
	};
	
	auto const expected_result = Container({2, 6, 8, 3, 5, 1, 2, 3});
	
	BOUNDED_TEST(::containers::concatenate<Container>() == Container());
	BOUNDED_TEST(::containers::concatenate<Container>(expected_result) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(Container({1})) == Container({1}));
	BOUNDED_TEST(::containers::concatenate<Container>(expected_result, Container()) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(Container(), expected_result) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, c) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), b, bounded::copy(c)) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), bounded::copy(c)) == expected_result);

	auto const extra_capacity_a = reserve_extra_capacity(a);
	BOUNDED_TEST(::containers::concatenate<Container>(extra_capacity_a, b, c) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(reserve_extra_capacity(a), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, reserve_extra_capacity(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, reserve_extra_capacity(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(reserve_extra_capacity(a), bounded::copy(b), c) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Container>(Container(), a, Container(), b, Container(), c, Container()) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(reserve_extra_capacity(a), reserve_extra_capacity(b), reserve_extra_capacity(c)) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<containers::vector<char>>(std::string_view("a"), std::string_view("b")) == containers::vector<char>({'a', 'b'}));

	using non_copyable_container = containers::vector<bounded_test::non_copyable_integer>;
	BOUNDED_TEST(::containers::concatenate<non_copyable_container>(non_copyable_container({1})) == non_copyable_container({1}));
	return true;
}

static_assert(test());
