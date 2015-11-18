// algorithm equivalent to std::inplace_merge followed by std::unique
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <containers/algorithms/find.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

namespace containers {
namespace detail {

template<typename T>
constexpr decltype(auto) base_iterator(T it) {
	return it;
}
template<typename T>
constexpr decltype(auto) base_iterator(std::move_iterator<T> it) {
	return base_iterator(it.base());
}
template<typename LHS, typename RHS>
constexpr auto equal_iterators(LHS lhs, RHS rhs) {
	return base_iterator(lhs) == base_iterator(rhs);
}


// This is written so that a special predicate built on std::less can work
// TODO: Handle OutputIterator in addition to MutableForwardIterator
template<typename InputIterator, typename Sentinel, typename MutableForwardIterator, typename BinaryPredicate>
constexpr auto unique_common(InputIterator first, Sentinel const last, MutableForwardIterator output, BinaryPredicate equal) {
	for (; base_iterator(first) != base_iterator(last); ++first) {
		auto && output_ref = *output;
		auto && first_ref = *first;
		if (equal(output_ref, first_ref)) {
			continue;
		}
		++output;
		*output = std::forward<decltype(first_ref)>(first_ref);
	}
	return std::next(output);
}


template<typename InputIterator, typename Sentinel, typename MutableForwardIterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique_copy(InputIterator const first, Sentinel const last, MutableForwardIterator const output, BinaryPredicate equal = BinaryPredicate{}) {
	if (base_iterator(first) == base_iterator(last)) {
		return output;
	}
	*output = *first;
	return ::containers::detail::unique_common(std::next(first), last, output, equal);
}


template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate>
constexpr auto find_first_equal(MutableForwardIterator first, Sentinel const last, BinaryPredicate equal) {
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


template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique(MutableForwardIterator const first, Sentinel const last, BinaryPredicate equal = BinaryPredicate{}) {
	auto const equal_element = find_first_equal(first, last, equal);
	if (equal_element == last) {
		return equal_element;
	}
	auto const other = ::containers::find_if(std::next(equal_element), last, [&](auto const & value) { return !equal(*equal_element, value); });
	if (other == last) {
		return equal_element;
	}
	*equal_element = std::move(*other);
	return ::containers::detail::unique_common(std::make_move_iterator(std::next(other)), last, equal_element, equal);
}

template<typename InputIterator, typename Sentinel, typename MutableForwardIterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_copy_less(InputIterator const first, Sentinel const last, MutableForwardIterator const output, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::detail::unique_copy(first, last, output, [=](auto const & lhs, auto const & rhs) { return !less(lhs, rhs); });
}
template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate = std::less<>>
constexpr auto unique_less(MutableForwardIterator const first, Sentinel const last, BinaryPredicate less = BinaryPredicate{}) {
	return ::containers::detail::unique(first, last, [=](auto const & lhs, auto const & rhs) { return !less(lhs, rhs); });
}


template<typename InputIterator, typename Sentinel, typename T, typename BinaryPredicate>
constexpr auto next_greater(InputIterator const first, Sentinel const last, T const & x, BinaryPredicate less) {
	return ::containers::find_if(first, last, [&](auto const & value) { return less(x, value); });
}


template<typename InputIterator1, typename Sentinel1, typename InputIterator2, typename Sentinel2, typename MutableForwardIterator, typename BinaryPredicate, typename PostFunction>
constexpr auto unique_merge_common(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, MutableForwardIterator output, BinaryPredicate less, PostFunction postFunction) {
	while (!equal_iterators(first1, last1) and !equal_iterators(first2, last2)) {
		*output = less(*first2, *first1) ? *first2++ : *first1++;
		first1 = next_greater(first1, last1, *output, less);
		first2 = next_greater(first2, last2, *output, less);
		++output;
	}

	if (!equal_iterators(first1, last1)) {
		return unique_copy_less(first1, last1, output, less);
	} else {
		return postFunction(first2, last2, output);
	}
}

template<typename InputIterator1, typename Sentinel1, typename InputIterator2, typename Sentinel2, typename MutableForwardIterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_merge_copy(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, MutableForwardIterator output, BinaryPredicate less = BinaryPredicate{}) {
	auto postFunction = [=](auto const first, auto const last, auto const out) {
		return unique_copy_less(first, last, out, less);
	};
	return unique_merge_common(first1, last1, first2, last2, output, less, postFunction);
}

// Both ranges must be sorted
template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate = std::less<>>
auto unique_inplace_merge(MutableForwardIterator first, MutableForwardIterator middle, Sentinel const last, BinaryPredicate less = BinaryPredicate{}) {
	if (first == middle or middle == last) {
		return unique_less(first, last, less);
	}
	
	// This can be done better than just creating a std::vector, but it works
	// good enough for now (famous last words).
	using storage_type = std::vector<std::decay_t<decltype(*first)>>;

	if (less(*middle, *first)) {
		storage_type temp(std::make_move_iterator(first), std::make_move_iterator(middle));
		return ::containers::detail::unique_merge_copy(
			std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()),
			std::make_move_iterator(middle), std::make_move_iterator(last),
			first,
			less
		);
	}
	
	// Keep all the elements that will end up at the beginning where they are
	auto find_first_to_move = [=]() mutable {
		struct Result {
			MutableForwardIterator before_target;
			MutableForwardIterator target;
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
	less(*first_to_move.before_target, *first_to_move.before_target);
	
	if (first_to_move.target == middle) {
		return unique_less(first_to_move.before_target, last, less);
	}
	
	// Move the rest of the elements so we can use their space without
	// overwriting.
	storage_type temp(std::make_move_iterator(first_to_move.target), std::make_move_iterator(middle));
	auto const new_middle = next_greater(middle, last, *first_to_move.before_target, less);

	auto postFunction = [=](auto const first_, auto const last_, auto) {
		return unique_less(first_.base(), last_.base(), less);
	};
	return ::containers::detail::unique_merge_common(
		std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()),
		std::make_move_iterator(new_middle), std::make_move_iterator(last),
		first_to_move.target,
		less,
		postFunction
	);
}

}	// namespace detail
}	// namespace containers
