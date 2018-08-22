// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/is_container.hpp>
#include <containers/size.hpp>
#include <containers/algorithms/zip.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Iterator, typename Sentinel>
struct range_view_for_adl {

	constexpr range_view_for_adl(Iterator first, Sentinel last):
		m_begin(first),
		m_end(last)
	{
	}

	template<typename Range>
	constexpr explicit range_view_for_adl(Range && range):
		range_view_for_adl(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)))
	{
	}
	
	friend constexpr auto begin(range_view_for_adl view) {
		return view.m_begin;
	}
	friend constexpr auto end(range_view_for_adl view) {
		return view.m_end;
	}
	
private:
	Iterator m_begin;
	Sentinel m_end;
};

template<typename Range>
range_view_for_adl(Range &&) -> range_view_for_adl<
	decltype(begin(BOUNDED_FORWARD(std::declval<Range>()))),
	decltype(end(BOUNDED_FORWARD(std::declval<Range>())))
>;

// Overload an operator so we can use a fold expression, otherwise compile
// times are horrible.
template<typename LHSIterator, typename LHSSentinel, typename RHSIterator, typename RHSSentinel>
constexpr auto operator->*(range_view_for_adl<LHSIterator, LHSSentinel> lhs, range_view_for_adl<RHSIterator, RHSSentinel> rhs) {
	return range_view_for_adl(
		zip_iterator(begin(std::move(lhs)), end(lhs), begin(std::move(rhs))),
		zip_iterator(end(lhs), end(lhs), end(rhs))
	);
}

template<typename Result, typename Size, typename... RangesBefore>
constexpr auto concatenate_impl(
	Size const total_size,
	tuple<RangesBefore...> ranges_before
) {
	return containers::apply(std::move(ranges_before), [=](auto && ... ranges) {
		Result result;
		result.reserve(static_cast<typename Result::size_type>(total_size));
		(..., append(result, begin(BOUNDED_FORWARD(ranges)), end(BOUNDED_FORWARD(ranges))));
		return result;
	});
}

template<typename Result, typename Size, typename... RangesBefore, typename Range, typename... RangesAfter>
constexpr auto concatenate_impl(
	Size const total_size,
	tuple<RangesBefore...> ranges_before,
	Range && range,
	RangesAfter && ... after
) {
	if constexpr (std::is_same_v<Result, Range>) {
		static_assert(std::is_rvalue_reference_v<Range &&>);
		if (range.capacity() >= total_size) {
			apply(std::move(ranges_before), [&](auto && ... ranges) {
				if constexpr (sizeof...(ranges) == 1) {
					auto && single_range = (..., BOUNDED_FORWARD(ranges));
					range.insert(begin(range), begin(BOUNDED_FORWARD(single_range)), end(BOUNDED_FORWARD(single_range)));
				} else if constexpr (sizeof...(ranges) > 1) {
					auto to_insert = (... ->* range_view_for_adl(BOUNDED_FORWARD(ranges)));
					range.insert(begin(range), begin(BOUNDED_FORWARD(to_insert)), end(BOUNDED_FORWARD(to_insert)));
				}
			});
			(..., append(range, begin(BOUNDED_FORWARD(after)), end(BOUNDED_FORWARD(after))));
			return BOUNDED_FORWARD(range);
		}
	}
	return concatenate_impl<Result>(
		total_size,
		tuple_cat(std::move(ranges_before), tie(BOUNDED_FORWARD(range))),
		BOUNDED_FORWARD(after)...
	);
}

// Adding up a bunch of sizes leads to overflow in bounds
template<typename Size>
constexpr auto ugly_size_hack(Size const size) {
	if constexpr (std::is_integral_v<Size>) {
		constexpr auto max =  std::min(std::numeric_limits<Size>::max(), std::numeric_limits<std::uintmax_t>::max() / 64);
		return static_cast<bounded::integer<0, max>>(size);
	} else {
		return size;
	}
}

}	// namespace detail

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
template<typename Result, typename... Ranges, BOUNDED_REQUIRES(is_container<Result> and (... and is_iterable<Ranges>))>
constexpr auto concatenate(Ranges && ... ranges) {
	static_assert(!std::is_reference_v<Result>, "Cannot concatenate into a reference.");
	auto const total_size = (0_bi + ... + ::containers::detail::ugly_size_hack(size(ranges)));
	return detail::concatenate_impl<Result>(total_size, tuple{}, BOUNDED_FORWARD(ranges)...);
}

}	// namespace containers
