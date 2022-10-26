// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {

struct compute_end_from_size {
	constexpr compute_end_from_size() = default;
	constexpr compute_end_from_size(auto const &) {
	}
};

template<typename T>
concept explicit_sentinel = !std::same_as<T, compute_end_from_size>;

template<typename Sentinel, typename Iterator>
concept range_view_sentinel = sentinel_for<Sentinel, Iterator> or std::same_as<Sentinel, compute_end_from_size>;

struct no_explicit_size {
	constexpr no_explicit_size() = default;
	constexpr no_explicit_size(auto const &) {
	}
};

template<typename T>
concept explicit_size = !std::same_as<T, no_explicit_size>;

template<typename T>
concept range_view_size = bounded::integral<T> or std::same_as<T, no_explicit_size>;

} // namespace detail

template<iterator Iterator, detail::range_view_sentinel<Iterator> Sentinel = Iterator, detail::range_view_size Size = detail::no_explicit_size>
struct range_view {
	static_assert(detail::explicit_sentinel<Sentinel> or detail::explicit_size<Size>);

	constexpr range_view(Iterator first, Sentinel last, Size size_) requires detail::explicit_size<Size>:
		m_begin(std::move(first)),
		m_end(std::move(last)),
		m_size(size_)
	{
	}
	constexpr range_view(Iterator first, Sentinel last) requires(!detail::explicit_size<Size>):
		m_begin(std::move(first)),
		m_end(std::move(last))
	{
	}
	constexpr range_view(Iterator first, Size size_) requires(!detail::explicit_sentinel<Sentinel>):
		m_begin(std::move(first)),
		m_size(size_)
	{
	}
	constexpr explicit range_view(std::pair<Iterator, Sentinel> pair) requires(!detail::explicit_size<Size>):
		range_view(std::move(pair).first, std::move(pair).second)
	{
	}

	constexpr range_view(sized_range auto && r) requires detail::explicit_size<Size>:
		range_view(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r)),
			containers::size(r)
		)
	{
	}
	constexpr range_view(range auto && r) requires(!detail::explicit_size<Size>):
		range_view(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r))
		)
	{
	}
	
	constexpr auto begin() const -> Iterator {
		return m_begin;
	}
	constexpr auto size() const -> Size requires detail::explicit_size<Size> {
		return m_size;
	}
	// This is required for range-based for loops to work. A constrained `end`
	// is found by language rules and then causes an error when it cannot be
	// called.
	constexpr auto end() const {
		if constexpr (detail::explicit_sentinel<Sentinel>) {
			return m_end;
		} else {
			return begin() + size();
		}
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator==(range_view, range_view) -> bool = default;
	
private:
	[[no_unique_address]] Iterator m_begin;
	[[no_unique_address]] Sentinel m_end;
	[[no_unique_address]] Size m_size;
};

template<range Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(std::declval<Range &&>())),
	decltype(containers::end(std::declval<Range &&>()))
>;

template<range Range> requires detail::has_member_size<Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(std::declval<Range &&>())),
	decltype(containers::end(std::declval<Range &&>())),
	decltype(std::declval<Range &&>().size())
>;

template<forward_random_access_range Range> requires detail::has_member_size<Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(std::declval<Range &&>())),
	detail::compute_end_from_size,
	decltype(std::declval<Range &&>().size())
>;

template<iterator Iterator, sentinel_for<Iterator> Sentinel>
range_view(Iterator, Sentinel) -> range_view<
	Iterator,
	Sentinel
>;

template<iterator Iterator, bounded::integral Size>
range_view(Iterator, Size) -> range_view<
	Iterator,
	detail::compute_end_from_size,
	Size
>;

template<typename>
inline constexpr auto is_range_view = false;

template<typename Iterator, typename Sentinel, typename Size>
inline constexpr auto is_range_view<range_view<Iterator, Sentinel, Size>> = true;

} // namespace containers
