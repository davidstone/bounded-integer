// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/begin_end.hpp>
#include <containers/dynamic_array.hpp>

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

#include <functional>
#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

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

}	// namespace detail


template<iterator Iterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique_copy(Iterator const first, sentinel_for<Iterator> auto const last, iterator auto const output, BinaryPredicate equal = BinaryPredicate{}) {
	if (first == last) {
		return output;
	}
	*output = *first;
	return ::containers::detail::unique_common(::containers::next(first), last, output, equal);
}


namespace detail {

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

}	// namespace detail



template<iterator Iterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique(Iterator const first, sentinel_for<Iterator> auto const last, BinaryPredicate equal = BinaryPredicate{}) {
	auto const equal_element = detail::find_first_equal(first, last, equal);
	if (equal_element == last) {
		return equal_element;
	}
	auto const other = ::containers::find_if(::containers::next(equal_element), last, [&](auto const & value) { return !equal(*equal_element, value); });
	if (other == last) {
		return equal_element;
	}
	*equal_element = std::move(*other);
	return ::containers::detail::unique_common(
		::containers::move_iterator(containers::next(other)),
		::containers::move_iterator(last),
		equal_element,
		equal
	);
}

template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_copy_less(Iterator const first, sentinel_for<Iterator> auto const last, iterator auto const output, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::unique_copy(first, last, output, negate(less));
}
template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_less(Iterator const first, sentinel_for<Iterator> auto const last, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::unique(first, last, negate(less));
}


namespace detail {

template<iterator Iterator>
constexpr auto next_greater(Iterator const first, sentinel_for<Iterator> auto const last, auto const & x, auto less) {
	return ::containers::find_if(first, last, [&](auto const & value) { return less(x, value); });
}


}	// namespace detail


template<iterator Iterator1, iterator Iterator2, typename BinaryPredicate = std::less<>>
constexpr auto unique_merge_copy(Iterator1 first1, sentinel_for<Iterator1> auto const last1, Iterator2 first2, sentinel_for<Iterator2> auto const last2, iterator auto output, BinaryPredicate less = BinaryPredicate{}) {
	while (first1 != last1 and first2 != last2) {
		*output = less(*first2, *first1) ? *first2++ : *first1++;
		first1 = ::containers::detail::next_greater(first1, last1, *output, less);
		first2 = ::containers::detail::next_greater(first2, last2, *output, less);
		++output;
	}

	return (first1 != last1) ?
		::containers::unique_copy_less(first1, last1, output, less) :
		::containers::unique_copy_less(first2, last2, output, less);
}

// Both ranges must be sorted
template<iterator Iterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_inplace_merge(Iterator first, Iterator middle, sentinel_for<Iterator> auto const last, BinaryPredicate less = BinaryPredicate{}) {
	BOUNDED_ASSERT(containers::is_sorted(range_view(first, middle), less));
	BOUNDED_ASSERT(containers::is_sorted(range_view(middle, last), less));
	auto less_to_equal = [&](auto const & lhs, auto const & rhs) {
		return !less(lhs, rhs) and !less(rhs, lhs);
	};
	BOUNDED_ASSERT(detail::find_first_equal(first, middle, less_to_equal) == middle);

	if (first == middle or middle == last) {
		return unique_less(first, last, less);
	}
	
	using storage_type = dynamic_array<std::decay_t<decltype(*first)>>;

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
	auto const new_middle = ::containers::detail::next_greater(middle, last, *first_to_move.before_target, less);

	return ::containers::unique_merge_copy(
		::containers::move_iterator(containers::begin(temp)), ::containers::move_iterator(containers::end(temp)),
		::containers::move_iterator(new_middle), ::containers::move_iterator(last),
		first_to_move.target,
		less
	);
}

}	// namespace containers
