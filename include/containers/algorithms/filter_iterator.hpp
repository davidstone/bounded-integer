// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/reference_wrapper.hpp>
#include <containers/tuple.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cassert>
#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

// TODO: Switch to [[no_unique_address]] when supported
template<typename Sentinel, typename UnaryPredicate>
struct filter_iterator_traits : private tuple<Sentinel, UnaryPredicate>, default_dereference, default_compare {
	constexpr filter_iterator_traits(Sentinel last, UnaryPredicate condition) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple<Sentinel, UnaryPredicate>(std::move(last), std::move(condition))
	) {
	}
	template<typename Iterator>
	constexpr auto add(Iterator const it, bounded::constant_t<1>) const {
		auto && last = (*this)[0_bi];
		auto && condition = (*this)[1_bi];
		assert(it != last);
		return ::containers::find_if(std::next(it), last, condition);
	}
	constexpr auto && sentinel() const {
		return (*this)[0_bi];
	}
};

constexpr struct {
	template<typename IteratorAdapter>
	constexpr auto operator()(IteratorAdapter const & it) const {
		return compare(it.base(), it.traits().sentinel());
	}
} filter_sentinel_function;

}	// namespace detail

template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto filter_iterator(ForwardIterator first, Sentinel last, UnaryPredicate && condition) BOUNDED_NOEXCEPT_VALUE(
	::containers::adapt_iterator(
		::containers::find_if(first, last, condition),
		detail::filter_iterator_traits(last, BOUNDED_FORWARD(condition))
	)
)

template<typename Range, typename UnaryPredicate>
struct filter {
private:
	template<typename T>
	using wrapped = reference_wrapper<std::remove_reference_t<T>>;
public:
	using const_iterator = decltype(containers::filter_iterator(
		begin(std::declval<Range const>()),
		end(std::declval<Range const>()),
		std::declval<wrapped<UnaryPredicate const>>()
	));
	using iterator = decltype(containers::filter_iterator(
		begin(std::declval<Range>()),
		end(std::declval<Range>()),
		std::declval<wrapped<UnaryPredicate>>()
	));

	using value_type = typename std::remove_reference_t<Range>::value_type;
	using size_type = typename std::remove_reference_t<Range>::size_type;

	constexpr filter(Range && range, UnaryPredicate && predicate) noexcept(std::is_nothrow_move_constructible_v<Range> and std::is_nothrow_move_constructible_v<UnaryPredicate>):
		m_range(BOUNDED_FORWARD(range)),
		m_predicate(BOUNDED_FORWARD(predicate))
	{
	}
	
	friend constexpr auto begin(filter const & filtered) {
		return containers::filter_iterator(begin(filtered.m_range), end(filtered.m_range), reference_wrapper(filtered.m_predicate));
	}
	friend constexpr auto begin(filter & filtered) {
		return containers::filter_iterator(begin(filtered.m_range), end(filtered.m_range), reference_wrapper(filtered.m_predicate));
	}
	friend constexpr auto begin(filter && filtered) {
		return containers::filter_iterator(begin(std::move(filtered).m_range), end(std::move(filtered).m_range), reference_wrapper(filtered.m_predicate));
	}
	friend constexpr auto end(filter const &) {
		return iterator_adapter_sentinel(detail::filter_sentinel_function);
	}
	
private:
	Range m_range;
	UnaryPredicate m_predicate;
};

template<typename Range, typename UnaryPredicate>
filter(Range &&, UnaryPredicate &&) -> filter<Range, UnaryPredicate>;

}	// namespace containers
