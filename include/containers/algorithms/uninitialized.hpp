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
constexpr auto static_or_reinterpret_cast(Source source) noexcept {
	if constexpr (static_castable<Target, Source>) {
		return static_cast<Target>(source);
	} else {
		return reinterpret_cast<Target>(source);
	}
}

template<iterator InputIterator>
constexpr auto destroy_range(InputIterator first, sentinel_for<InputIterator> auto const last) noexcept {
	// This static_assert fails with reverse_iterator because std::prev is not
	// noexcept
	// static_assert(noexcept(bounded::destroy(*first)));
	for (; first != last; ++first) {
		bounded::destroy(*first);
	}
}

template<range Range>
constexpr auto destroy_range(Range && range) BOUNDED_NOEXCEPT_DECLTYPE(
	::containers::detail::destroy_range(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)))
)

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
constexpr auto uninitialized_move(InputIterator const first, sentinel_for<InputIterator> auto const last, ForwardIterator const out) BOUNDED_NOEXCEPT_VALUE(
	::containers::uninitialized_copy(::containers::move_iterator(first), ::containers::move_iterator(last), out)
)

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator>
constexpr auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last) BOUNDED_NOEXCEPT_VALUE(
	containers::uninitialized_copy(
		containers::reverse_iterator(last),
		containers::reverse_iterator(first),
		containers::reverse_iterator(out_last)
	).base()
)

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator>
constexpr auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last) BOUNDED_NOEXCEPT_VALUE(
	containers::uninitialized_copy_backward(
		containers::move_iterator(first),
		containers::move_iterator(last),
		out_last
	)
)


// uninitialized_move_destroy guarantees that all elements in the input range
// have been destoyed when this function completes.
// This should probably just require noexcept
template<iterator InputIterator, iterator ForwardIterator>
constexpr auto uninitialized_move_destroy(InputIterator const first, sentinel_for<InputIterator> auto const last, ForwardIterator out) {
	constexpr auto is_noexcept = noexcept(::containers::uninitialized_copy(
		::containers::move_destroy_iterator(first),
		::containers::move_destroy_iterator(last),
		out
	));
	if constexpr (is_noexcept) {
		return ::containers::uninitialized_copy(
			::containers::move_destroy_iterator(first),
			::containers::move_destroy_iterator(last),
			out
		);
	} else {
		auto first_adapted = ::containers::move_destroy_iterator(first);
		auto const last_adapted = containers::move_destroy_iterator(last);
		auto out_first = out;
		try {
			for (; first_adapted != last_adapted; ++first_adapted) {
				bounded::construct(*out, *first_adapted);
				++out;
			}
		} catch (...) {
			detail::destroy_range(first_adapted.base(), last);
			detail::destroy_range(out_first, out);
			throw;
		}
		return out;
	}
}


namespace detail {

// TODO: When we have constexpr destructors, move append_from_capacity into
// scope_guard
template<typename Source, typename Destination>
constexpr auto transfer_all_contents(Source && source, Destination & destination) {
	auto set_old_size_to_zero = [&]{
		source.append_from_capacity(-size(source));
	};
	try {
		::containers::uninitialized_move_destroy(
			begin(source),
			end(source),
			begin(destination)
		);
		set_old_size_to_zero();
	} catch (...) {
		set_old_size_to_zero();
		throw;
	}
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
