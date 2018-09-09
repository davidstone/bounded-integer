// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_destroy_iterator.hpp>
#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/type.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

// reinterpret_cast or static_cast from void * are not allowed in constexpr

template<typename Target, typename Source, typename Enable = void>
constexpr auto is_static_castable = false;

template<typename Target, typename Source>
constexpr auto is_static_castable<
	Target,
	Source,
	std::void_t<decltype(static_cast<Target>(std::declval<Source>()))>
> = true;

template<typename Target, typename Source>
constexpr auto static_or_reinterpret_cast(Source source) noexcept {
	if constexpr (is_static_castable<Target, Source>) {
		return static_cast<Target>(source);
	} else {
		return reinterpret_cast<Target>(source);
	}
}

template<typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto destroy_range(InputIterator first, Sentinel const last) noexcept {
	// This static_assert fails with reverse_iterator because std::prev is not
	// noexcept
	// static_assert(noexcept(bounded::destroy(*first)));
	for (; first != last; ++first) {
		bounded::destroy(*first);
	}
}

}	// namespace detail


template<
	typename InputIterator, typename Sentinel,
	typename ForwardIterator,
	BOUNDED_REQUIRES(!std::is_nothrow_constructible<
		std::decay_t<decltype(*std::declval<ForwardIterator>())>,
		decltype(*std::declval<InputIterator &>())
	>::value)
>
auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out) {
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


template<
	typename InputIterator, typename Sentinel,
	typename ForwardIterator,
	BOUNDED_REQUIRES(std::is_nothrow_constructible<
		std::decay_t<decltype(*std::declval<ForwardIterator>())>,
		decltype(*std::declval<InputIterator &>())
	>::value)
>
constexpr auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out) noexcept {
	for (; first != last; ++first) {
		bounded::construct(*out, *first);
		++out;
	}
	return out;
}



template<typename InputIterator, typename Sentinel, typename ForwardIterator>
constexpr auto uninitialized_move(InputIterator const first, Sentinel const last, ForwardIterator const out) BOUNDED_NOEXCEPT_VALUE(
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
template<
	typename InputIterator,
	typename Sentinel,
	typename ForwardIterator,
	BOUNDED_REQUIRES(not noexcept(::containers::uninitialized_copy(
		::containers::move_destroy_iterator(std::declval<InputIterator const &>()),
		::containers::move_destroy_iterator(std::declval<Sentinel const &>()),
		std::declval<ForwardIterator const &>()
	)))
>
auto uninitialized_move_destroy(InputIterator const first, Sentinel const last, ForwardIterator out) {
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

template<
	typename InputIterator,
	typename Sentinel,
	typename ForwardIterator,
	BOUNDED_REQUIRES(noexcept(::containers::uninitialized_copy(
		::containers::move_destroy_iterator(std::declval<InputIterator const &>()),
		::containers::move_destroy_iterator(std::declval<Sentinel const &>()),
		std::declval<ForwardIterator const &>()
	)))
>
constexpr auto uninitialized_move_destroy(InputIterator const first, Sentinel const last, ForwardIterator const out) noexcept {
	return ::containers::uninitialized_copy(
		::containers::move_destroy_iterator(first),
		::containers::move_destroy_iterator(last),
		out
	);
}



template<
	typename ForwardIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(!std::is_nothrow_default_constructible<std::decay_t<decltype(*std::declval<ForwardIterator const &>())>>::value)
>
auto uninitialized_default_construct(ForwardIterator const first, Sentinel const last) {
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

template<
	typename ForwardIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(std::is_nothrow_default_constructible<std::decay_t<decltype(*std::declval<ForwardIterator const &>())>>::value)
>
constexpr auto uninitialized_default_construct(ForwardIterator first, Sentinel const last) noexcept {
	for (; first != last; ++first) {
		bounded::construct(*first);
	}
}

}	// namespace containers
