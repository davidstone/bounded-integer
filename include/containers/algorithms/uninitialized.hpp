// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_destroy_iterator.hpp>
#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

// reinterpret_cast or static_cast from void * are not allowed in constexpr
template<typename Target, typename Source>
concept static_castable = requires(Source source) { static_cast<Target>(source); };

template<typename Target>
constexpr auto static_or_reinterpret_cast(auto && source) {
	if constexpr (static_castable<Target, decltype(source)>) {
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

constexpr void destroy_range(range auto && r) {
	::containers::detail::destroy_range(containers::begin(OPERATORS_FORWARD(r)), containers::end(OPERATORS_FORWARD(r)));
}

}	// namespace detail


template<iterator InputIterator>
constexpr auto uninitialized_copy(InputIterator first, sentinel_for<InputIterator> auto const last, iterator auto out) {
	auto out_first = out;
	try {
		for (; first != last; ++first) {
			bounded::construct(*out, [&] { return *first; });
			++out;
		}
	} catch (...) {
		detail::destroy_range(out_first, out);
		throw;
	}
	return out;
}

constexpr auto uninitialized_copy(range auto && input, iterator auto output) {
	return uninitialized_copy(
		containers::begin(OPERATORS_FORWARD(input)),
		containers::end(OPERATORS_FORWARD(input)),
		output
	);
}


template<iterator InputIterator>
constexpr auto uninitialized_move(InputIterator const first, sentinel_for<InputIterator> auto const last, iterator auto const out) {
	return ::containers::uninitialized_copy(::containers::move_iterator(first), ::containers::move_iterator(last), out);
}

template<bidirectional_iterator BidirectionalInputIterator>
constexpr auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, bidirectional_iterator auto const out_last) {
	return containers::uninitialized_copy(
		containers::reverse_iterator(last),
		containers::reverse_iterator(first),
		containers::reverse_iterator(out_last)
	).base();
}

template<bidirectional_iterator BidirectionalInputIterator>
constexpr auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, bidirectional_iterator auto const out_last) {
	return containers::uninitialized_copy_backward(
		containers::move_iterator(first),
		containers::move_iterator(last),
		out_last
	);
}


template<iterator InputIterator>
constexpr auto uninitialized_move_destroy(InputIterator const first, sentinel_for<InputIterator> auto const last, iterator auto out) {
	return ::containers::uninitialized_copy(
		::containers::move_destroy_iterator(first),
		::containers::move_destroy_iterator(last),
		out
	);
}

constexpr auto uninitialized_move_destroy(range auto && source, iterator auto out) {
	return ::containers::uninitialized_move_destroy(
		containers::begin(OPERATORS_FORWARD(source)),
		containers::end(OPERATORS_FORWARD(source)),
		out
	);
}


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
