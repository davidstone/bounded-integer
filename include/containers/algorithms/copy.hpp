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

#include <containers/addressof.hpp>
#include <containers/allocator.hpp>

#include <bounded_integer/bounded_integer.hpp>
#include <bounded_integer/integer_range.hpp>

#include <iterator>
#include <memory>

namespace containers {
namespace detail {

template<typename Allocator, typename T, typename... Args>
constexpr auto construct(Allocator && allocator, T * pointer, Args && ... args) BOUNDED_NOEXCEPT(
	allocator_traits<std::decay_t<Allocator>>::construct(allocator, reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer), std::forward<Args>(args)...)
)
template<typename Allocator, typename T>
constexpr auto destroy(Allocator && allocator, T * pointer) BOUNDED_NOEXCEPT(
	allocator_traits<std::decay_t<Allocator>>::destroy(allocator, reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer))
)


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

template<typename InputIterator, typename Sentinel, typename ForwardIterator, typename Allocator_>
auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out, Allocator_ && allocator) {
	auto out_first = out;
	try {
		for (; first != last; ++first) {
			::containers::detail::construct(allocator, ::containers::addressof(*out), *first);
			++out;
		}
	} catch (...) {
		for (; out_first != out; ++out_first) {
			::containers::detail::destroy(allocator, ::containers::addressof(*out_first));
		}
		throw;
	}
	return out;
}

template<typename InputIterator, typename ForwardIterator, typename Allocator>
auto uninitialized_move(InputIterator const first, InputIterator const last, ForwardIterator const out, Allocator && allocator) {
	return ::containers::detail::uninitialized_copy(std::make_move_iterator(first), std::make_move_iterator(last), out, allocator);
}

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator, typename Allocator>
auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last, Allocator && allocator) {
	return ::containers::detail::uninitialized_copy(std::make_reverse_iterator(last), std::make_reverse_iterator(first), std::make_reverse_iterator(out_last), allocator).base();
}

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator, typename Allocator>
auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last, Allocator && allocator) {
	return ::containers::detail::uninitialized_copy_backward(std::make_move_iterator(first), std::make_move_iterator(last), out_last, allocator);
}

template<typename ForwardIterator, typename Sentinel, typename Allocator_>
auto uninitialized_default_construct(ForwardIterator const first, Sentinel const last, Allocator_ && allocator) {
	auto it = first;
	try {
		for (; it != last; ++it) {
			::containers::detail::construct(allocator, ::containers::addressof(*it));
		}
	} catch (...) {
		for (auto rollback = first; rollback != it; ++rollback) {
			::containers::detail::destroy(allocator, ::containers::addressof(*rollback));
		}
		throw;
	}
}

}	// namespace detail
}	// namespace containers
