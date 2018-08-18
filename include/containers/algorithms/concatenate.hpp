// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/null_iterator.hpp>
#include <containers/size.hpp>
#include <containers/algorithms/zip.hpp>

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
	(... and noexcept(::containers::detail::individual_reusable_container<Result>(total_size, std::forward<Ranges>(ranges))))
) {
	Result * result = nullptr;
	(
		...,
		(result = result ? result : individual_reusable_container<Result>(total_size, std::forward<Ranges>(ranges)))
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
	if (std::addressof(range) == std::addressof(result)) {
		result.insert(begin(result), before_first, before_last);
		(..., append(result, begin(std::forward<RangesAfter>(after)), end(std::forward<RangesAfter>(after))));
		return std::move(result);
	} else {
		return concatenate_with_enough_spare_capacity(
			result,
			zip_iterator(before_first, before_last, begin(range)),
			zip_iterator(before_last, before_last, end(range)),
			std::forward<RangesAfter>(after)...
		);
	}
}

template<typename Container, typename ZipIterator, typename Range>
constexpr auto concatenate_with_enough_spare_capacity(
	Container & result,
	ZipIterator before_first,
	ZipIterator before_last,
	Range && range
) {
	assert(std::addressof(range) == std::addressof(result));
	result.insert(begin(result), before_first, before_last);
	return std::move(result);
}

}	// namespace detail

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
template<typename Result, typename... Ranges, BOUNDED_REQUIRES(is_container<Result> and (... and is_iterable<Ranges>))>
constexpr auto concatenate(Ranges && ... ranges) {
	static_assert(!std::is_reference_v<Result>, "Cannot concatenate into a reference.");
	auto const total_size = (... + size(ranges));
	auto const reusable = detail::reusable_container<Result>(total_size, std::forward<Ranges>(ranges)...);
	if (reusable) {
		return concatenate_with_enough_spare_capacity(*reusable, detail::null_iterator{}, detail::null_iterator{}, std::forward<Ranges>(ranges)...);
	} else {
		Result result;
		result.reserve(static_cast<typename Result::size_type>(total_size));
		(..., append(result, begin(std::forward<Ranges>(ranges)), end(std::forward<Ranges>(ranges))));
		return result;
	}
}

template<typename... Ranges>
constexpr auto concatenate(Ranges && ... ranges) BOUNDED_NOEXCEPT_VALUE(
	concatenate<std::common_type_t<std::decay_t<Ranges>...>>(std::forward<Ranges>(ranges)...)
)

}	// namespace containers
