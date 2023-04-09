// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.unique;

import containers.algorithms.advance;
import containers.algorithms.erase;
import containers.algorithms.find;
import containers.algorithms.is_sorted;
import containers.algorithms.move_iterator;
import containers.append;
import containers.begin_end;
import containers.dynamic_array;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.iter_difference_t;
import containers.iter_value_t;
import containers.iterator_t;
import containers.offset_type;
import containers.range_view;
import containers.repeat_n;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

// This is written so that a special predicate built on std::less can work
template<iterator Iterator>
constexpr auto unique_common(Iterator first, sentinel_for<Iterator> auto const last, iterator auto output, auto equal) {
	for (; first != last; ++first) {
		auto && output_ref = *output;
		auto && first_ref = *first;
		if (equal(output_ref, first_ref)) {
			continue;
		}
		++output;
		*output = OPERATORS_FORWARD(first_ref);
	}
	return ::containers::next(output);
}

export template<iterator Iterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique_copy(Iterator const first, sentinel_for<Iterator> auto const last, iterator auto const output, BinaryPredicate equal = BinaryPredicate{}) {
	if (first == last) {
		return output;
	}
	*output = *first;
	return ::containers::unique_common(::containers::next(first), last, output, equal);
}


template<iterator Iterator>
constexpr auto find_first_equal(Iterator first, sentinel_for<Iterator> auto const last, auto equal) {
	if (first == last) {
		return first;
	}
	auto previous = first;
	++first;
	for (; first != last; ++first) {
		if (equal(*previous, *first)) {
			break;
		}
		previous = first;
	}
	return first;
}


export template<iterator Iterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique(Iterator const first, sentinel_for<Iterator> auto const last, BinaryPredicate equal = BinaryPredicate{}) {
	auto const equal_element = ::containers::find_first_equal(first, last, equal);
	if (equal_element == last) {
		return equal_element;
	}
	auto const other = ::containers::find_if(::containers::next(equal_element), last, [&](auto const & value) { return !equal(*equal_element, value); });
	if (other == last) {
		return equal_element;
	}
	*equal_element = std::move(*other);
	return ::containers::unique_common(
		::containers::move_iterator(containers::next(other)),
		::containers::move_iterator(last),
		equal_element,
		equal
	);
}

export template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_copy_less(Iterator const first, sentinel_for<Iterator> auto const last, iterator auto const output, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::unique_copy(first, last, output, std::not_fn(less));
}
export template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_less(Iterator const first, sentinel_for<Iterator> auto const last, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::unique(first, last, std::not_fn(less));
}


template<iterator Iterator>
constexpr auto next_greater(Iterator const first, sentinel_for<Iterator> auto const last, auto const & x, auto less) {
	return ::containers::find_if(first, last, [&](auto const & value) { return less(x, value); });
}


export template<iterator Iterator1, iterator Iterator2, typename BinaryPredicate = std::less<>>
constexpr auto unique_merge_copy(Iterator1 first1, sentinel_for<Iterator1> auto const last1, Iterator2 first2, sentinel_for<Iterator2> auto const last2, iterator auto output, BinaryPredicate less = BinaryPredicate{}) {
	while (first1 != last1 and first2 != last2) {
		*output = less(*first2, *first1) ? *first2++ : *first1++;
		first1 = ::containers::next_greater(first1, last1, *output, less);
		first2 = ::containers::next_greater(first2, last2, *output, less);
		++output;
	}

	return (first1 != last1) ?
		::containers::unique_copy_less(first1, last1, output, less) :
		::containers::unique_copy_less(first2, last2, output, less);
}

// Both ranges must be sorted
export template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_inplace_merge(Iterator first, Iterator middle, sentinel_for<Iterator> auto const last, BinaryPredicate less = BinaryPredicate{}) {
	BOUNDED_ASSERT(containers::is_sorted(range_view(first, middle), less));
	BOUNDED_ASSERT(containers::is_sorted(range_view(middle, last), less));
	auto less_to_equal = [&](auto const & lhs, auto const & rhs) {
		return !less(lhs, rhs) and !less(rhs, lhs);
	};
	BOUNDED_ASSERT(::containers::find_first_equal(first, middle, less_to_equal) == middle);

	if (first == middle or middle == last) {
		return unique_less(first, last, less);
	}
	
	using storage_type = dynamic_array<iter_value_t<Iterator>, offset_type<Iterator>>;

	if (less(*middle, *first)) {
		auto temp = storage_type(range_view(
			::containers::move_iterator(first),
			::containers::move_iterator(middle)
		));
		return ::containers::unique_merge_copy(
			::containers::move_iterator(containers::begin(temp)), ::containers::move_iterator(containers::end(temp)),
			::containers::move_iterator(middle), ::containers::move_iterator(last),
			first,
			less
		);
	}
	
	// Keep all the elements that will end up at the beginning where they are
	auto find_first_to_move = [=]() mutable {
		struct Result {
			Iterator before_target;
			Iterator target;
		};

		auto before = first;
		++first;
		for ( ; first != middle; ++first) {
			if (less(*middle, *first)) {
				break;
			}
			before = first;
		}
		return Result{ before, first };
	};
	auto const first_to_move = find_first_to_move();
	
	if (first_to_move.target == middle) {
		return unique_less(first_to_move.before_target, last, less);
	}
	
	// Move the rest of the elements so we can use their space without
	// overwriting.
	auto temp = storage_type(range_view(
		::containers::move_iterator(first_to_move.target),
		::containers::move_iterator(middle)
	));
	auto const new_middle = ::containers::next_greater(middle, last, *first_to_move.before_target, less);

	return ::containers::unique_merge_copy(
		::containers::move_iterator(containers::begin(temp)), ::containers::move_iterator(containers::end(temp)),
		::containers::move_iterator(new_middle), ::containers::move_iterator(last),
		first_to_move.target,
		less
	);
}

} // namespace containers

using Container = containers::vector<bounded_test::integer>;

constexpr void test_unique_copy_less(Container const & source, Container const & expected) {
	auto destination = Container(containers::repeat_n(containers::size(source), 0));
	auto const it = containers::unique_copy_less(begin(source), end(source), begin(destination));
	containers::erase_after(destination, it);
	BOUNDED_ASSERT(destination == expected);
}

constexpr void test_unique_less(Container source, Container const & expected) {
	BOUNDED_ASSERT(containers::is_sorted(source));
	BOUNDED_ASSERT(containers::is_sorted(expected));
	test_unique_copy_less(source, expected);
}

constexpr void test_unique_merge_copy(Container const & lhs, Container const & rhs, Container const & expected) {
	auto result = Container(containers::repeat_n(::bounded::assume_in_range<containers::range_size_t<Container>>(containers::size(lhs) + containers::size(rhs)), 0));
	auto const it = containers::unique_merge_copy(begin(lhs), end(lhs), begin(rhs), end(rhs), begin(result));
	containers::erase_after(result, it);

	BOUNDED_ASSERT(result == expected);
}

constexpr void test_unique_inplace_merge(Container v, Container const & other, Container const & expected) {
	using iterator = containers::iterator_t<Container const &>;
	auto const midpoint = static_cast<containers::iter_difference_t<iterator>>(containers::size(v));
	containers::append(v, other);
	auto const it = containers::unique_inplace_merge(begin(v), begin(v) + midpoint, end(v));
	containers::erase_after(v, it);

	BOUNDED_ASSERT(v == expected);
}

constexpr void test_unique_merge(Container v, Container const & other, Container const & expected) {
	BOUNDED_ASSERT(containers::is_sorted(v));
	BOUNDED_ASSERT(containers::is_sorted(other));
	test_unique_merge_copy(v, other, expected);
	test_unique_inplace_merge(std::move(v), other, expected);
}

constexpr bool test_unique() {
	test_unique_less({{1}}, {{1}});
	test_unique_less({{1, 2}}, {{1, 2}});
	test_unique_less({{}}, {{}});
	test_unique_less({{1, 3, 5, 5, 5, 6, 10, 10}}, {{1, 3, 5, 6, 10}});
	test_unique_less({{1, 1, 1, 1, 1, 1, 1, 1}}, {{1}});
	test_unique_merge({{1, 2, 3, 5}}, {{7, 8, 9}}, {{1, 2, 3, 5, 7, 8, 9}});
	test_unique_merge({{1, 3, 5, 7, 9}}, {{2, 2, 2, 3, 3, 6, 7}}, {{1, 2, 3, 5, 6, 7, 9}});
	test_unique_merge({{2}}, {{1}}, {{1, 2}});
	test_unique_merge({{}}, {{6}}, {{6}});
	test_unique_merge({{4}}, {{}}, {{4}});
	test_unique_merge({{}}, {{}}, {{}});
	test_unique_merge({{1}}, {{1}}, {{1}});
	test_unique_merge({{8}}, {{8, 8, 8, 8, 8}}, {{8}});
	// Ideally unique_inplace_merge would not assume the first range has no
	// duplicates, but that is my current use-case. I do not know how to remove
	// this limitation without making the algorithm less efficient.
	// test_unique_merge({{8, 8}}, {{8, 8, 8, 8, 8}}, {{8}});
	return true;
}

static_assert(test_unique());
