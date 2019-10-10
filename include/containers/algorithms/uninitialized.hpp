// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_destroy_iterator.hpp>
#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

// reinterpret_cast or static_cast from void * are not allowed in constexpr
template<typename Target, typename Source>
concept static_castable = requires(Source source) { static_cast<Target>(source); };

template<typename Target, typename Source>
constexpr auto static_or_reinterpret_cast(Source source) {
	if constexpr (static_castable<Target, Source>) {
		return static_cast<Target>(source);
	} else {
		return reinterpret_cast<Target>(source);
	}
}

template<iterator InputIterator>
constexpr void destroy_range(InputIterator first, sentinel_for<InputIterator> auto const last) {
	for (; first != last; ++first) {
		bounded::destroy(*first);
	}
}

template<range Range>
constexpr void destroy_range(Range && range) {
	::containers::detail::destroy_range(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)));
}

}	// namespace detail


template<iterator InputIterator, iterator ForwardIterator>
constexpr auto uninitialized_copy(InputIterator first, sentinel_for<InputIterator> auto const last, ForwardIterator out) {
	auto out_first = out;
	try {
		for (; first != last; ++first) {
			bounded::construct(*out, *first);
			++out;
		}
	} catch (...) {
		detail::destroy_range(out_first, out);
		throw;
	}
	return out;
}


template<iterator InputIterator, iterator ForwardIterator>
constexpr auto uninitialized_move(InputIterator const first, sentinel_for<InputIterator> auto const last, ForwardIterator const out) {
	return ::containers::uninitialized_copy(::containers::move_iterator(first), ::containers::move_iterator(last), out);
}

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator>
constexpr auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last) {
	return containers::uninitialized_copy(
		containers::reverse_iterator(last),
		containers::reverse_iterator(first),
		containers::reverse_iterator(out_last)
	).base();
}

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator>
constexpr auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last) {
	return containers::uninitialized_copy_backward(
		containers::move_iterator(first),
		containers::move_iterator(last),
		out_last
	);
}


template<iterator InputIterator, iterator ForwardIterator>
constexpr auto uninitialized_move_destroy(InputIterator const first, sentinel_for<InputIterator> auto const last, ForwardIterator out) {
	return ::containers::uninitialized_copy(
		::containers::move_destroy_iterator(first),
		::containers::move_destroy_iterator(last),
		out
	);
}

constexpr auto uninitialized_move_destroy(range auto && source, iterator auto out) {
	return ::containers::uninitialized_move_destroy(
		begin(BOUNDED_FORWARD(source)),
		end(BOUNDED_FORWARD(source)),
		out
	);
}


namespace detail {

template<typename Source, typename Destination>
constexpr auto transfer_all_contents(Source && source, Destination & destination) {
	::containers::uninitialized_move_destroy(BOUNDED_FORWARD(source), begin(destination));
	source.append_from_capacity(-size(source));
}

} // namespace detail


template<iterator ForwardIterator>
constexpr auto uninitialized_default_construct(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last) {
	auto it = first;
	try {
		for (; it != last; ++it) {
			bounded::construct(*it);
		}
	} catch (...) {
		detail::destroy_range(first, it);
		throw;
	}
}

}	// namespace containers
