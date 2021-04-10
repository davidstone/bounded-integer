// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>

#include <bounded/integer.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <iterator>
#include <utility>

namespace containers {

template<typename Iterator, typename Sentinel = Iterator>
struct range_view {
	using value_type = iter_value_t<Iterator>;

	using size_type = bounded::integer<
		0,
		bounded::normalize<static_cast<std::uintmax_t>(bounded::max_value<iter_difference_t<Iterator>>)>
	>;
	
	using const_iterator = Iterator;
	using iterator = Iterator;

	constexpr range_view(Iterator first, Sentinel last):
		m_begin(std::move(first)),
		m_end(std::move(last))
	{
	}
	constexpr explicit range_view(std::pair<Iterator, Sentinel> pair):
		range_view(std::move(pair).first, std::move(pair).second)
	{
	}
	constexpr range_view(range auto && r):
		range_view(containers::begin(OPERATORS_FORWARD(r)), containers::end(OPERATORS_FORWARD(r)))
	{
	}
	
	constexpr auto begin() const {
		return m_begin;
	}
	constexpr auto end() const {
		return m_end;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator==(range_view, range_view) -> bool = default;
	
private:
	[[no_unique_address]] Iterator m_begin;
	[[no_unique_address]] Sentinel m_end;
};

template<typename Range>
range_view(Range &&) -> range_view<decltype(containers::begin(std::declval<Range &&>()), containers::end(std::declval<Range &&>()))>;

template<typename>
inline constexpr auto is_range_view = false;

template<typename Iterator, typename Sentinel>
inline constexpr auto is_range_view<range_view<Iterator, Sentinel>> = true;

} // namespace containers
