// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/is_container.hpp>
#include <containers/null_iterator.hpp>
#include <containers/size.hpp>
#include <containers/algorithms/zip.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Result, typename Size, typename Container, BOUNDED_REQUIRES(
	std::is_rvalue_reference_v<Container &&> and std::is_same_v<Container, Result>
)>
constexpr auto individual_reusable_container(Size const total_size, Container && container) BOUNDED_NOEXCEPT_VALUE(
	(container.capacity() >= total_size) ? std::addressof(container) : nullptr
)

template<typename Result, typename Size, typename Container, BOUNDED_REQUIRES(
	!std::is_rvalue_reference_v<Container &&> or !std::is_same_v<Container, Result>
)>
constexpr auto individual_reusable_container(Size, Container &&) noexcept {
	return nullptr;
}

template<typename Result, typename Size, typename... Ranges>
constexpr auto reusable_container(Size const total_size, Ranges && ... ranges) noexcept(
	(... and noexcept(::containers::detail::individual_reusable_container<Result>(total_size, BOUNDED_FORWARD(ranges))))
) {
	Result * result = nullptr;
	(
		...,
		(result = result ? result : individual_reusable_container<Result>(total_size, BOUNDED_FORWARD(ranges)))
	);
	return result;
}

template<typename Result, typename ZipIterator, typename Range, typename... RangesAfter>
constexpr auto concatenate_with_enough_spare_capacity(
	Result & result,
	ZipIterator before_first,
	ZipIterator before_last,
	Range && range,
	RangesAfter && ... after
) {
	if constexpr (std::is_same_v<Result, Range>) {
		if (std::addressof(range) == std::addressof(result)) {
			result.insert(begin(result), before_first, before_last);
			(..., append(result, begin(BOUNDED_FORWARD(after)), end(BOUNDED_FORWARD(after))));
			return std::move(result);
		}
	}
	if constexpr (sizeof...(RangesAfter) > 0) {
		return concatenate_with_enough_spare_capacity(
			result,
			zip_iterator(before_first, before_last, begin(range)),
			zip_iterator(before_last, before_last, end(range)),
			BOUNDED_FORWARD(after)...
		);
	} else {
		assert(false);
		return std::move(result);
	}
}

// Adding up a bunch of sizes leads to overflow in bounds
template<typename Size, BOUNDED_REQUIRES(std::is_integral_v<Size>)>
constexpr auto ugly_size_hack(Size const size) {
	constexpr auto max =  std::min(std::numeric_limits<Size>::max(), std::numeric_limits<std::uintmax_t>::max() / 64);
	return static_cast<bounded::integer<0, max>>(size);
}
template<typename Size, BOUNDED_REQUIRES(!std::is_integral_v<Size>)>
constexpr auto ugly_size_hack(Size const size) {
	return size;
}

}	// namespace detail

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
template<typename Result, typename... Ranges, BOUNDED_REQUIRES(is_container<Result> and (... and is_iterable<Ranges>))>
constexpr auto concatenate(Ranges && ... ranges) {
	static_assert(!std::is_reference_v<Result>, "Cannot concatenate into a reference.");
	auto const total_size = (... + ::containers::detail::ugly_size_hack(size(ranges)));
	auto const reusable = detail::reusable_container<Result>(total_size, BOUNDED_FORWARD(ranges)...);
	if (reusable) {
		return concatenate_with_enough_spare_capacity(*reusable, detail::null_iterator{}, detail::null_iterator{}, BOUNDED_FORWARD(ranges)...);
	} else {
		Result result;
		result.reserve(static_cast<typename Result::size_type>(total_size));
		(..., append(result, begin(BOUNDED_FORWARD(ranges)), end(BOUNDED_FORWARD(ranges))));
		return result;
	}
}

}	// namespace containers
