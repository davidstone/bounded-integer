// algorithm equivalent to std::inplace_merge followed by std::unique
// Copyright (C) 2014 David Stone
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

#ifndef CONTAINERS_UNIQUE_INPLACE_MERGE_HPP_
#define CONTAINERS_UNIQUE_INPLACE_MERGE_HPP_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace containers {
namespace detail {

// copy_unique requires that the range passed in is already sorted.
//
// The binary predicate requires the "less than" semantics rather than
// "equal to" because we know the range must be sorted, which required a strict
// weak ordering. This allows reuse of the same predicate with no duplication of
// the checks. No element compares less than an earlier element.
//
// Returns an iterator pointing to one-past-the-end of the last element copied
template<typename InputIterator, typename MutableForwardIterator, typename BinaryPredicate>
MutableForwardIterator copy_unique(InputIterator first, InputIterator const last, MutableForwardIterator destination, BinaryPredicate less) {
	if (first == last) {
		return destination;
	}
	*destination = *first;
	++first;
	auto before_destination = destination;
	++destination;
	for (; first != last; ++first) {
		if (!less(*before_destination, *first)) {
			continue;
		}
		*destination = *first;
		++before_destination;
		++destination;
	}
	return destination;
}

template<typename InputIterator, typename MutableForwardIterator>
MutableForwardIterator copy_unique(InputIterator const first, InputIterator const last, MutableForwardIterator const destination) {
	return copy_unique(first, last, destination, std::less<>{});
}


// unique_inplace_merge works on two sets of data. This function moves from one
// of the two ranges until one of two conditions occurs:
//
// 1) The range is empty or
// 2) The front of the other range compares less than the front of this range
//
// The third parameter is an iterator to just before the destination we will be
// inserting into because we only insert into the destination if the value at
// before_destination is less than the front of the source range.
template<typename ForwardIterator, typename OutputIterator, typename Compare>
std::tuple<ForwardIterator, OutputIterator> move_chunk(
	ForwardIterator first, ForwardIterator const last,
	OutputIterator before_destination,
	std::remove_reference_t<decltype(*std::declval<ForwardIterator>())> const & other,
	Compare compare
) {
	for (; first != last; ++first) {
		// If this gets to the point where the other set of data contains
		// something smaller, stop inserting and switch over to the other set.
		if (compare(other, *first)) {
			break;
		}
		if (compare(*before_destination, *first)) {
			++before_destination;
			*before_destination = std::move(*first);
		}
	}
	return std::make_tuple(first, before_destination);
}

template<typename Iterator, typename Compare>
Iterator unique_inplace_merge(Iterator first, Iterator middle, Iterator last, Compare compare) {
	// This works when [first, middle) or [middle, last) is empty
	if (first == last) {
		return std::unique(first, last, [=](auto const & lhs, auto const & rhs) {
			return !compare(lhs, rhs) and !compare(rhs, lhs);
		});
	}

	// Keep all the elements that will end up at the beginning where they are
	auto destination = std::find_if(first, middle, [=](auto const & value) {
		return compare(*middle, value);
	});
	
	// This can be done better than just creating a std::vector, but it works
	// good enough for now (famous last words).
	using storage_type = std::vector<std::decay_t<decltype(*std::declval<Iterator>())>>;
	// Move the rest of the elements so we can use their space without
	// overwriting.
	storage_type temp(std::make_move_iterator(destination), std::make_move_iterator(middle));
	auto move_from = temp.begin();
	
	// If either range is entirely less than the other, we skip over this step,
	// as we do not have to clear out any of the ranges.
	if (!temp.empty() and first != destination) {
		--destination;
		while (true) {
			std::tie(middle, destination) = move_chunk(middle, last, destination, *move_from, compare);
			if (middle == last) {
				break;
			}
			std::tie(move_from, destination) = move_chunk(move_from, temp.end(), destination, *middle, compare);
			if (move_from == temp.end()) {
				break;
			}
		}
	}
	auto const create_remaining_range = [=](auto const begin, auto const end) {
		return std::make_pair(
			std::find_if(begin, end, [=](auto const & value) { return compare(*destination, value); }),
			end
		);
	};
	if (middle != last) {
		auto const remaining_range = create_remaining_range(middle, last);
		return copy_unique(
			std::make_move_iterator(remaining_range.first),
			std::make_move_iterator(remaining_range.second),
			std::next(destination),
			compare
		);
	}
	else if (move_from != temp.end()) {
		auto const remaining_range = create_remaining_range(move_from, temp.end());
		return copy_unique(
			std::make_move_iterator(remaining_range.first),
			std::make_move_iterator(remaining_range.second),
			std::next(destination),
			compare
		);
	}
	else {
		return last;
	}
}

template<typename Iterator>
Iterator unique_inplace_merge(Iterator first, Iterator middle, Iterator last) {
	return unique_inplace_merge(first, middle, last, std::less<>{});
}

}	// namespace detail
}	// namespace containers
#endif	// CONTAINERS_UNIQUE_INPLACE_MERGE_HPP_
