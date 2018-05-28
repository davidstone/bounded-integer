// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Sentinel, typename Condition>
struct filter_iterator_adapter : private tuple<Sentinel, Condition> {
	constexpr filter_iterator_adapter(Sentinel last, Condition condition) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple<Sentinel, Condition>(std::move(last), std::move(condition))
	) {
	}
	template<typename Iterator>
	constexpr auto operator()(Iterator const it, bounded::constant_t<1>) const {
		auto && last = (*this)[0_bi];
		auto && condition = (*this)[1_bi];
		return ::containers::find_if(std::next(it), last, condition);
	}
};

}	// namespace detail

// TODO: support bidirectional iterator
template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto filter_iterator(ForwardIterator first, Sentinel last, UnaryPredicate && condition) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(
		::containers::find_if(first, last, condition),
		detail::default_dereference{},
		detail::filter_iterator_adapter<Sentinel, std::remove_reference_t<UnaryPredicate>>(last, std::forward<UnaryPredicate>(condition)),
		detail::not_a_function{},
		detail::default_compare{}
	)
)

}	// namespace containers
