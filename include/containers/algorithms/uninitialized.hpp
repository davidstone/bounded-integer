// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/allocator.hpp>
#include <containers/type_list.hpp>
#include <containers/algorithms/iterator.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

// reinterpret_cast or static_cast from void * are not allowed in constexpr

template<typename Target, typename Source, typename Enable = void>
struct is_static_castable_c : std::false_type {};

template<typename Target, typename Source>
struct is_static_castable_c<
	Target,
	Source,
	void_t<decltype(static_cast<Target>(std::declval<Source>()))>
> : std::true_type {};

template<typename Target, typename Source>
constexpr auto is_static_castable = is_static_castable_c<Target, Source>::value;

template<typename Target, typename Source, BOUNDED_REQUIRES(is_static_castable<Target, Source>)>
constexpr auto static_or_reinterpret_cast(Source source) noexcept {
	return static_cast<Target>(source);
}

template<typename Target, typename Source, BOUNDED_REQUIRES(!is_static_castable<Target, Source>)>
auto static_or_reinterpret_cast(Source source) noexcept {
	return reinterpret_cast<Target>(source);
}

template<typename Allocator, typename T, typename... Args>
constexpr auto construct(Allocator && allocator, T * pointer, Args && ... args) BOUNDED_NOEXCEPT(
	allocator_traits<Allocator>::construct(
		allocator,
		::containers::detail::static_or_reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer),
		std::forward<Args>(args)...
	)
)

template<typename Allocator, typename T>
constexpr auto destroy(Allocator && allocator, T * pointer) BOUNDED_NOEXCEPT(
	allocator_traits<Allocator>::destroy(
		allocator,
		::containers::detail::static_or_reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer)
	)
)

template<typename Allocator, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto destroy(Allocator && allocator, InputIterator first, Sentinel const last) noexcept {
	static_assert(noexcept(::containers::detail::destroy(allocator, ::bounded::addressof(*first))));
	for (; first != last; ++first) {
		::containers::detail::destroy(allocator, ::bounded::addressof(*first));
	}
}

}	// namespace detail


template<
	typename InputIterator, typename Sentinel,
	typename ForwardIterator,
	typename Allocator,
	BOUNDED_REQUIRES(!std::is_nothrow_constructible<
		std::decay_t<decltype(*std::declval<ForwardIterator>())>,
		decltype(*std::declval<InputIterator &>())
	>::value)
>
auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out, Allocator && allocator) {
	auto out_first = out;
	try {
		for (; first != last; ++first) {
			::containers::detail::construct(allocator, ::bounded::addressof(*out), *first);
			++out;
		}
	} catch (...) {
		for (; out_first != out; ++out_first) {
			::containers::detail::destroy(allocator, ::bounded::addressof(*out_first));
		}
		throw;
	}
	return out;
}


template<
	typename InputIterator, typename Sentinel,
	typename ForwardIterator,
	typename Allocator,
	BOUNDED_REQUIRES(std::is_nothrow_constructible<
		std::decay_t<decltype(*std::declval<ForwardIterator>())>,
		decltype(*std::declval<InputIterator &>())
	>::value)
>
constexpr auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out, Allocator && allocator) noexcept {
	for (; first != last; ++first) {
		::containers::detail::construct(allocator, ::bounded::addressof(*out), *first);
		++out;
	}
	return out;
}



template<typename InputIterator, typename ForwardIterator, typename Allocator>
constexpr auto uninitialized_move(InputIterator const first, InputIterator const last, ForwardIterator const out, Allocator && allocator) BOUNDED_NOEXCEPT_VALUE(
	::containers::uninitialized_copy(::containers::make_move_iterator(first), ::containers::make_move_iterator(last), out, allocator)
)

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator, typename Allocator>
constexpr auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last, Allocator && allocator) BOUNDED_NOEXCEPT_VALUE(
	::containers::uninitialized_copy(::containers::make_reverse_iterator(last), ::containers::make_reverse_iterator(first), ::containers::make_reverse_iterator(out_last), allocator).base()
)

template<typename BidirectionalInputIterator, typename BidirectionalOutputIterator, typename Allocator>
constexpr auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, BidirectionalOutputIterator const out_last, Allocator && allocator) BOUNDED_NOEXCEPT_VALUE(
	::containers::uninitialized_copy_backward(::containers::make_move_iterator(first), ::containers::make_move_iterator(last), out_last, allocator)
)


template<typename InputIterator, typename ForwardIterator, typename Allocator>
constexpr auto uninitialized_move_destroy(InputIterator const first, InputIterator const last, ForwardIterator const out, Allocator && allocator) noexcept(noexcept(::containers::uninitialized_move(first, last, out, allocator))) {
	auto const it = ::containers::uninitialized_move(first, last, out, allocator);
	::containers::detail::destroy(allocator, first, last);
	return it;
}



template<
	typename ForwardIterator,
	typename Sentinel,
	typename Allocator,
	BOUNDED_REQUIRES(!std::is_nothrow_default_constructible<std::decay_t<decltype(*std::declval<ForwardIterator const &>())>>::value)
>
auto uninitialized_default_construct(ForwardIterator const first, Sentinel const last, Allocator && allocator) {
	auto it = first;
	try {
		for (; it != last; ++it) {
			::containers::detail::construct(allocator, ::bounded::addressof(*it));
		}
	} catch (...) {
		for (auto rollback = first; rollback != it; ++rollback) {
			::containers::detail::destroy(allocator, ::bounded::addressof(*rollback));
		}
		throw;
	}
}

template<
	typename ForwardIterator,
	typename Sentinel,
	typename Allocator,
	BOUNDED_REQUIRES(std::is_nothrow_default_constructible<std::decay_t<decltype(*std::declval<ForwardIterator const &>())>>::value)
>
constexpr auto uninitialized_default_construct(ForwardIterator first, Sentinel const last, Allocator && allocator) noexcept {
	for (; first != last; ++first) {
		::containers::detail::construct(allocator, ::bounded::addressof(*first));
	}
}

}	// namespace containers
