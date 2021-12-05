// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <operators/operators.hpp>

namespace containers {
namespace detail {

using namespace bounded::literal;

struct batch_sentinel {};

template<typename Iterator, typename Sentinel, typename Count>
struct batch_iterator {
	using value_type = range_view<Iterator>;
	using difference_type = bounded::integer<
		-bounded::builtin_max_value<Count>,
		bounded::builtin_max_value<Count>
	>;
	using pointer = value_type *;
	using reference = value_type;
	using iterator_category = std::random_access_iterator_tag;

	constexpr explicit batch_iterator(range_view<Iterator, Sentinel> const all, Count const number_of_batches):
		m_all(all),
		m_number_of_batches(number_of_batches),
		m_batch(0_bi)
	{
	}

	constexpr auto operator*() const {
		auto const first = containers::begin(m_all);
		return value_type(
			first + elements_prior_to_batch(m_batch),
			first + elements_prior_to_batch(bounded::assume_in_range<batch_t>(m_batch + 1_bi))
		);
	}

	friend constexpr auto operator+(batch_iterator lhs, bounded::bounded_integer auto const rhs) {
		lhs.m_batch += rhs;
		return lhs;
	}
	friend constexpr auto operator+(bounded::bounded_integer auto const lhs, batch_iterator const rhs) {
		return rhs + lhs;
	}
	friend constexpr auto operator-(batch_iterator const lhs, batch_iterator const rhs) {
		return lhs.m_batch - rhs.m_batch;
	}
	friend constexpr auto operator-(batch_sentinel, batch_iterator const rhs) {
		return rhs.m_number_of_batches - rhs.m_batch;
	}

	friend auto operator==(batch_iterator const lhs, batch_iterator const rhs) {
		return lhs.m_batch == rhs.m_batch;
	}
	friend auto operator<=>(batch_iterator const lhs, batch_iterator const rhs) {
		return lhs.m_batch <=> rhs.m_batch;
	}

	friend auto operator==(batch_iterator const lhs, batch_sentinel) {
		return lhs.m_batch == lhs.m_number_of_batches;
	}
	friend auto operator<=>(batch_iterator const lhs, batch_sentinel) {
		return lhs.m_batch <=> lhs.m_number_of_batches;
	}

private:
	using batch_t = bounded::integer<0, bounded::builtin_max_value<Count>>;
	constexpr auto elements_prior_to_batch(batch_t const batch) const {
		auto const total_elements = containers::size(m_all);
		auto const base = total_elements / m_number_of_batches;
		auto const add_one_if_below = total_elements % m_number_of_batches;
		return bounded::assume_in_range<range_size_t<range_view<Iterator, Sentinel>>>(base * batch + bounded::min(add_one_if_below, batch));
	}
	[[no_unique_address]] range_view<Iterator, Sentinel> m_all;
	[[no_unique_address]] bounded::integer<1, bounded::builtin_max_value<Count>> m_number_of_batches;
	[[no_unique_address]] batch_t m_batch;
};

} // namespace detail

template<bounded::bounded_integer Integer> requires(numeric_traits::min_value<Integer> > 0_bi)
constexpr auto batched(range auto & r, Integer const number_of_batches) {
	return containers::range_view(
		detail::batch_iterator(range_view(r), number_of_batches),
		detail::batch_sentinel()
	);
}

} // namespace containers
