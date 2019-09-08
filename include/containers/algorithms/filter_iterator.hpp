// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/reference_wrapper.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

// TODO: Switch to [[no_unique_address]] when supported
template<typename Sentinel, typename UnaryPredicate>
struct filter_iterator_traits : default_dereference, default_compare {
private:
	[[no_unique_address]] Sentinel m_sentinel;
	[[no_unique_address]] UnaryPredicate m_predicate;
public:
	constexpr filter_iterator_traits(Sentinel last, UnaryPredicate condition) noexcept(
		std::is_nothrow_move_constructible_v<Sentinel> and
		std::is_nothrow_move_constructible_v<UnaryPredicate>
	):
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

	template<typename Iterator>
	constexpr auto add(Iterator const it, bounded::constant_t<1>) const {
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

template<typename T>
concept is_filter_iterator_traits = detail::is_filter_iterator_traits<T>;

struct filter_iterator_sentinel {
};

template<typename Iterator, typename Traits> requires is_filter_iterator_traits<Traits>
constexpr auto compare(adapt_iterator<Iterator, Traits> const lhs, filter_iterator_sentinel) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().compare(lhs.base(), lhs.traits().sentinel())
)

template<typename Iterator, typename Traits> requires is_filter_iterator_traits<Traits>
constexpr auto compare(filter_iterator_sentinel, adapt_iterator<Iterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs.traits().compare(rhs.traits().sentinel(), rhs.base())
)

template<typename Iterator, typename Traits> requires is_filter_iterator_traits<Traits>
constexpr auto operator==(adapt_iterator<Iterator, Traits> const lhs, filter_iterator_sentinel) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().equal(lhs.base(), lhs.traits().sentinel())
)

template<typename Iterator, typename Traits> requires is_filter_iterator_traits<Traits>
constexpr auto operator==(filter_iterator_sentinel, adapt_iterator<Iterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs.traits().equal(rhs.traits().sentinel(), rhs.base())
)

template<typename ForwardIterator, typename Traits>
constexpr auto filter_iterator_impl(ForwardIterator first, Traits traits) BOUNDED_NOEXCEPT_VALUE(
	containers::adapt_iterator(
		containers::find_if(first, containers::unwrap(traits).sentinel(), containers::unwrap(traits).predicate()),
		traits
	)
)

}	// namespace detail

template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto filter_iterator(ForwardIterator first, Sentinel last, UnaryPredicate condition) BOUNDED_NOEXCEPT_VALUE(
	detail::filter_iterator_impl(first, detail::filter_iterator_traits(last, std::move(condition)))
)

template<typename Range, typename UnaryPredicate>
struct filter {
private:
	using sentinel = decltype(end(std::declval<Range &>()));
	using traits = detail::filter_iterator_traits<sentinel, UnaryPredicate>;
public:
	using const_iterator = decltype(detail::filter_iterator_impl(
		begin(std::declval<Range const &>()),
		reference_wrapper(std::declval<traits const &>())
	));
	using iterator = decltype(detail::filter_iterator_impl(
		begin(std::declval<Range &>()),
		reference_wrapper(std::declval<traits &>())
	));

	using value_type = typename std::remove_reference_t<Range>::value_type;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::uintmax_t>(std::numeric_limits<typename std::iterator_traits<iterator>::difference_type>::max())>
	>;

	constexpr filter(Range && range, UnaryPredicate predicate) noexcept(std::is_nothrow_move_constructible_v<Range> and std::is_nothrow_constructible_v<traits, sentinel, UnaryPredicate>):
		m_range(BOUNDED_FORWARD(range)),
		m_traits(end(m_range), std::move(predicate))
	{
	}
	
	friend constexpr auto begin(filter const & filtered) {
		return detail::filter_iterator_impl(begin(filtered.m_range), reference_wrapper(filtered.m_traits));
	}
	friend constexpr auto begin(filter & filtered) {
		return detail::filter_iterator_impl(begin(filtered.m_range), reference_wrapper(filtered.m_traits));
	}
	friend constexpr auto begin(filter && filtered) {
		return detail::filter_iterator_impl(begin(std::move(filtered).m_range), reference_wrapper(filtered.m_traits));
	}
	friend constexpr auto end(filter const &) {
		return detail::filter_iterator_sentinel{};
	}
	
private:
	Range m_range;
	traits m_traits;
};

template<typename Range, typename UnaryPredicate>
filter(Range &&, UnaryPredicate) -> filter<Range, UnaryPredicate>;

}	// namespace containers
