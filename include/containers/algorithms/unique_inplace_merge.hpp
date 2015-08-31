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
	return it.base();
}

// If the types can be compared, use that. If they can't, they must not be
// compatible iterator types.
template<typename LHS, typename RHS>
constexpr auto not_same_iterator(LHS && lhs, RHS && rhs) -> decltype(lhs != rhs) {
	return lhs != rhs;
}
// This is worse match for any types unless there is no operator!= defined
template<typename... Args>
constexpr auto not_same_iterator(Args && ...) {
	return true;
}

// This is written so that a special predicate built on std::less can work
template<typename InputIterator, typename Sentinel, typename MutableForwardIterator, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique_copy(InputIterator first, Sentinel const last, MutableForwardIterator output, BinaryPredicate equal = BinaryPredicate{}) {
	if (base_iterator(first) == base_iterator(last)) {
		return output;
	}
	if (not_same_iterator(base_iterator(output), base_iterator(first))) {
		*output = *first;
	}
	while (base_iterator(++first) != base_iterator(last)) {
		if (!equal(*output, *first) and not_same_iterator(base_iterator(++output), base_iterator(first))) {
			*output = *first;
		}
	}
	return ++output;
}

template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate = std::equal_to<>>
constexpr auto unique(MutableForwardIterator first, Sentinel const last, BinaryPredicate equal = BinaryPredicate{}) {
	return ::containers::detail::unique_copy(std::make_move_iterator(first), last, first, equal);
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
	return std::find_if(first, last, [&](auto const & value) { return less(x, value); });
}


template<typename InputIterator1, typename Sentinel1, typename InputIterator2, typename Sentinel2, typename MutableForwardIterator, typename BinaryPredicate = std::less<>>
constexpr auto unique_merge_copy(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, MutableForwardIterator output, BinaryPredicate less = BinaryPredicate{}) {
	while (first1 != last1 and first2 != last2) {
		*output = less(*first2, *first1) ? *first2++ : *first1++;
		first1 = next_greater(first1, last1, *output, less);
		first2 = next_greater(first2, last2, *output, less);
		++output;
	}

	if (first1 != last1) {
		return unique_copy_less(first1, last1, output, less);
	} else {
		return unique_copy_less(first2, last2, output, less);
	}
}

// unique_inplace_merge assumes the first range is sorted and has no duplicates.
// It assumes the second range is sorted.
template<typename MutableForwardIterator, typename Sentinel, typename BinaryPredicate = std::less<>>
auto unique_inplace_merge(MutableForwardIterator first, MutableForwardIterator middle, Sentinel const last, BinaryPredicate less = BinaryPredicate{}) {
	if (first == middle or middle == last) {
		return unique_less(first, last, less);
	}

	// Keep all the elements that will end up at the beginning where they are

	auto new_ranges = [=]() mutable {
		auto p = [=](auto const & value) { return less(*middle, value); };
		struct Result {
			MutableForwardIterator destination;
			MutableForwardIterator second_range_first;
		};
		if (p(*first)) {
			return Result{ first, middle };
		}
		while (true) {
			auto const it = std::next(first);
			if (it == middle) {
				break;
			}
			if (p(*it)) {
				return Result{ it, next_greater(middle, last, *first, less) };
			}
			first = it;
		}
		return Result{ middle, next_greater(middle, last, *first, less) };
	};

	auto const iterators = new_ranges();

	// This can be done better than just creating a std::vector, but it works
	// good enough for now (famous last words).
	using storage_type = std::vector<std::decay_t<decltype(*first)>>;
	// Move the rest of the elements so we can use their space without
	// overwriting.
	storage_type temp(std::make_move_iterator(iterators.destination), std::make_move_iterator(middle));

	return unique_merge_copy(
		std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()),
		std::make_move_iterator(iterators.second_range_first), std::make_move_iterator(last),
		iterators.destination,
		less
	);
}

}	// namespace detail
}	// namespace containers
