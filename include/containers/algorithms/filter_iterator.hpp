// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/begin_end.hpp>
#include <containers/default_adapt_traits.hpp>
#include <containers/default_begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reference_wrapper.hpp>

#include <operators/forward.hpp>
#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

// TODO: use a custom begin function
template<typename Sentinel, typename UnaryPredicate>
struct filter_iterator_traits : default_begin_end, default_dereference, default_compare {
private:
	[[no_unique_address]] Sentinel m_sentinel;
	[[no_unique_address]] UnaryPredicate m_predicate;
public:
	constexpr filter_iterator_traits(Sentinel last, UnaryPredicate condition):
		m_sentinel(std::move(last)),
		m_predicate(std::move(condition))
	{
	}

	constexpr auto sentinel() const {
		return m_sentinel;
	}
	constexpr auto const & predicate() const {
		return m_predicate;
	}

	constexpr auto add(iterator auto const it, bounded::constant_t<1>) const {
		BOUNDED_ASSERT(it != m_sentinel);
		return containers::find_if(containers::next(it), m_sentinel, m_predicate);
	}
};

namespace detail {

template<typename T>
inline constexpr auto is_filter_iterator_traits = false;

template<typename Sentinel, typename UnaryPredicate>
inline constexpr auto is_filter_iterator_traits<filter_iterator_traits<Sentinel, UnaryPredicate>> = true;

template<typename T>
inline constexpr auto is_filter_iterator_traits<reference_wrapper<T>> = is_filter_iterator_traits<T>;

template<typename T>
inline constexpr auto is_filter_iterator_traits<T const> = is_filter_iterator_traits<T>;

} // namespace detail

// TODO: Change name to something without "is"
template<typename T>
concept is_filter_iterator_traits = detail::is_filter_iterator_traits<T>;

struct filter_iterator_sentinel {
};

template<iterator Iterator, is_filter_iterator_traits Traits>
constexpr auto operator<=>(adapt_iterator<Iterator, Traits> const lhs, filter_iterator_sentinel) OPERATORS_RETURNS(
	lhs.traits().compare(lhs.base(), lhs.traits().sentinel())
)

template<iterator Iterator, is_filter_iterator_traits Traits>
constexpr auto operator==(adapt_iterator<Iterator, Traits> const lhs, filter_iterator_sentinel) -> bool {
	return lhs.traits().equal(lhs.base(), lhs.traits().sentinel());
}

constexpr auto filter_iterator_impl(iterator auto first, is_filter_iterator_traits auto traits) {
	return containers::adapt_iterator(
		containers::find_if(first, containers::unwrap(traits).sentinel(), containers::unwrap(traits).predicate()),
		traits
	);
}

}	// namespace detail

template<iterator ForwardIterator>
constexpr auto filter_iterator(ForwardIterator first, sentinel_for<ForwardIterator> auto last, auto predicate) {
	return detail::filter_iterator_impl(first, detail::filter_iterator_traits(last, std::move(predicate)));
}

template<typename Range, typename UnaryPredicate>
struct filter {
private:
	using sentinel = decltype(::containers::end(std::declval<Range &>()));
	using traits = detail::filter_iterator_traits<sentinel, UnaryPredicate>;
public:
	using const_iterator = decltype(detail::filter_iterator_impl(
		::containers::begin(std::declval<Range const &>()),
		reference_wrapper(std::declval<traits const &>())
	));
	using iterator = decltype(detail::filter_iterator_impl(
		::containers::begin(std::declval<Range &>()),
		reference_wrapper(std::declval<traits &>())
	));

	using value_type = range_value_t<Range>;
	using size_type = bounded::integer<
		0,
		bounded::normalize<static_cast<std::uintmax_t>(bounded::max_value<typename std::iterator_traits<iterator>::difference_type>)>
	>;

	constexpr filter(Range && range, UnaryPredicate predicate):
		m_range(OPERATORS_FORWARD(range)),
		m_traits(::containers::end(m_range), std::move(predicate))
	{
	}
	
	constexpr auto begin() const & {
		return detail::filter_iterator_impl(::containers::begin(m_range), reference_wrapper(m_traits));
	}
	constexpr auto begin() & {
		return detail::filter_iterator_impl(::containers::begin(m_range), reference_wrapper(m_traits));
	}
	constexpr auto begin() && {
		return detail::filter_iterator_impl(::containers::begin(std::move(*this).m_range), reference_wrapper(m_traits));
	}
	static constexpr auto end() {
		return detail::filter_iterator_sentinel();
	}
	
private:
	Range m_range;
	traits m_traits;
};

template<typename Range, typename UnaryPredicate>
filter(Range &&, UnaryPredicate) -> filter<Range, UnaryPredicate>;

}	// namespace containers
