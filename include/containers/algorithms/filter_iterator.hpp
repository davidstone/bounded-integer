// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/range_view.hpp>
#include <containers/tuple.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cassert>
#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

// TODO: Switch to [[no_unique_address]] when supported
template<typename Sentinel, typename Condition>
struct filter_iterator_increment : private tuple<Sentinel, Condition> {
	constexpr filter_iterator_increment(Sentinel last, Condition condition) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple<Sentinel, Condition>(std::move(last), std::move(condition))
	) {
	}
	template<typename Iterator>
	constexpr auto operator()(Iterator const it, bounded::constant_t<1>) const {
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
		return compare(it.base(), it.add().sentinel());
	}
} filter_sentinel_function;

}	// namespace detail

template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto filter_iterator(ForwardIterator first, Sentinel last, UnaryPredicate && condition) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(
		::containers::find_if(first, last, condition),
		detail::default_dereference{},
		detail::filter_iterator_increment<Sentinel, std::remove_reference_t<UnaryPredicate>>(last, BOUNDED_FORWARD(condition)),
		detail::not_a_function{},
		detail::default_compare{}
	)
)

template<typename Range, typename UnaryPredicate>
constexpr auto filter(Range & range, UnaryPredicate && condition) BOUNDED_NOEXCEPT_VALUE(
	range_view(
		containers::filter_iterator(begin(range), end(range), BOUNDED_FORWARD(condition)),
		iterator_adapter_sentinel(detail::filter_sentinel_function)
	)
)

}	// namespace containers
