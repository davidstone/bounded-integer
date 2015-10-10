// Algorithms to copy that work with sentinels
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

#include <bounded_integer/bounded_integer.hpp>
#include <bounded_integer/integer_range.hpp>

#include <iterator>
#include <memory>

namespace containers {
namespace detail {

// TODO: Work with other iterator categories
template<typename InputIterator, typename Sentinel>
constexpr auto distance(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	last - first
)

template<typename InputIterator, typename Sentinel, typename OutputIterator>
constexpr auto copy(InputIterator first, Sentinel const last, OutputIterator out) {
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (; first != last; ++first) {
		*out = *first;
		++out;
	}
	return result { first, out };
}

// Ideally this would be implemented with a counted iterator
template<typename InputIterator, typename Size, typename OutputIterator>
constexpr auto copy_n(InputIterator first, Size const count, OutputIterator out) {
	static_assert(count >= bounded::constant<0>, "Negative numbers not supported.");
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (auto const n : bounded::integer_range(count)) {
		static_cast<void>(n);
		*out = *first;
		++out;
		++first;
	}
	return result { first, out };
}

template<typename InputIterator, typename Sentinel, typename ForwardIterator>
auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out) {
	auto out_first = out;
	using out_type = typename std::iterator_traits<InputIterator>::value_type;
	try {
		for (; first != last; ++first) {
			::new(static_cast<void *>(std::addressof(*out))) out_type(*first);
			++out;
		}
	} catch (...) {
		for (; out_first != out; ++out_first) {
			reinterpret_cast<out_type &>(*out_first).~out_type();
		}
		throw;
	}
	return out;
}

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator>
auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out) {
	return ::containers::detail::uninitialized_copy(std::make_reverse_iterator(last), std::make_reverse_iterator(first), std::make_reverse_iterator(out)).base();
}

}	// namespace detail
}	// namespace containers
