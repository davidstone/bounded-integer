// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/operator_bracket.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <utility>

namespace containers {

template<typename Iterator, typename Sentinel = Iterator>
struct range_view {
	using value_type = typename std::iterator_traits<Iterator>::value_type;

	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::uintmax_t>(bounded::max_value<typename std::iterator_traits<Iterator>::difference_type>)>
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
	template<typename Range> requires range<Range>
	constexpr explicit range_view(Range & range):
		range_view(begin(range), end(range))
	{
	}
	
	friend constexpr Iterator begin(range_view view) {
		return view.m_begin;
	}
	friend constexpr Sentinel end(range_view view) {
		return view.m_end;
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(range_view)
	
private:
	Iterator m_begin;
	Sentinel m_end;
};

template<typename Range>
range_view(Range &) -> range_view<decltype(begin(std::declval<Range &>()), end(std::declval<Range &>()))>;

template<typename Iterator, typename Sentinel>
constexpr auto operator==(range_view<Iterator, Sentinel> const lhs, range_view<Iterator, Sentinel> const rhs) {
	return begin(lhs) == begin(rhs) and end(lhs) == end(rhs);
}

template<typename>
inline constexpr auto is_range_view = false;

template<typename Iterator, typename Sentinel>
inline constexpr auto is_range_view<range_view<Iterator, Sentinel>> = true;

} // namespace containers
