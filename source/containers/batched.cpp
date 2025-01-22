// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.batched;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.iterator_t;
import containers.offset_type;
import containers.range;
import containers.size;
import containers.subrange;

import bounded;
import numeric_traits;
import operators;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Iterator, typename Sentinel, typename Size, typename Count>
struct batch_iterator {
	constexpr explicit batch_iterator(subrange<Iterator, Sentinel, Size> const all, Count const number_of_batches):
		m_all(all),
		m_number_of_batches(number_of_batches),
		m_batch(0_bi)
	{
	}

	constexpr auto operator*() const {
		auto const first = containers::begin(m_all);
		return subrange<Iterator>(
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
	friend constexpr auto operator-(std::default_sentinel_t, batch_iterator const rhs) {
		return rhs.m_number_of_batches - rhs.m_batch;
	}

	friend auto operator==(batch_iterator const lhs, batch_iterator const rhs) {
		return lhs.m_batch == rhs.m_batch;
	}
	friend auto operator<=>(batch_iterator const lhs, batch_iterator const rhs) {
		return lhs.m_batch <=> rhs.m_batch;
	}

	friend auto operator==(batch_iterator const lhs, std::default_sentinel_t) {
		return lhs.m_batch == lhs.m_number_of_batches;
	}
	friend auto operator<=>(batch_iterator const lhs, std::default_sentinel_t) {
		return lhs.m_batch <=> lhs.m_number_of_batches;
	}

private:
	using batch_t = bounded::integer<0, bounded::builtin_max_value<Count>>;
	constexpr auto elements_prior_to_batch(batch_t const batch) const {
		auto const total_elements = containers::size(m_all);
		auto const base = total_elements / m_number_of_batches;
		auto const add_one_if_below = total_elements % m_number_of_batches;
		using offset_t = offset_type<iterator_t<subrange<Iterator, Sentinel, Size>>>;
		return bounded::assume_in_range<offset_t>(base * batch + bounded::min(add_one_if_below, batch));
	}
	[[no_unique_address]] subrange<Iterator, Sentinel, Size> m_all;
	[[no_unique_address]] bounded::integer<1, bounded::builtin_max_value<Count>> m_number_of_batches;
	[[no_unique_address]] batch_t m_batch;
};

export template<bounded::bounded_integer Integer> requires(numeric_traits::min_value<Integer> > 0_bi)
constexpr auto batched(range auto & r, Integer const number_of_batches) {
	return containers::subrange(
		batch_iterator(subrange(r), number_of_batches),
		std::default_sentinel
	);
}

} // namespace containers

constexpr auto a = containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

static_assert([] {
	constexpr auto number_of_batches = 1_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], a);
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 2_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1, 2, 3, 4});
	containers::equal(batches[1_bi], containers::array{5, 6, 7, 8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 3_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1, 2, 3});
	containers::equal(batches[1_bi], containers::array{4, 5, 6});
	containers::equal(batches[2_bi], containers::array{7, 8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 5_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1});
	containers::equal(batches[1_bi], containers::array{2, 3});
	containers::equal(batches[2_bi], containers::array{4, 5});
	containers::equal(batches[3_bi], containers::array{6, 7});
	containers::equal(batches[4_bi], containers::array{8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 6_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1});
	containers::equal(batches[1_bi], containers::array{2, 3});
	containers::equal(batches[2_bi], containers::array{4, 5});
	containers::equal(batches[3_bi], containers::array{6, 7});
	containers::equal(batches[4_bi], containers::array{8});
	containers::equal(batches[5_bi], containers::array{9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 10_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0});
	containers::equal(batches[1_bi], containers::array{1});
	containers::equal(batches[2_bi], containers::array{2});
	containers::equal(batches[3_bi], containers::array{3});
	containers::equal(batches[4_bi], containers::array{4});
	containers::equal(batches[5_bi], containers::array{5});
	containers::equal(batches[6_bi], containers::array{6});
	containers::equal(batches[7_bi], containers::array{7});
	containers::equal(batches[8_bi], containers::array{8});
	containers::equal(batches[9_bi], containers::array{9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 12_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0});
	containers::equal(batches[1_bi], containers::array{1});
	containers::equal(batches[2_bi], containers::array{2});
	containers::equal(batches[3_bi], containers::array{3});
	containers::equal(batches[4_bi], containers::array{4});
	containers::equal(batches[5_bi], containers::array{5});
	containers::equal(batches[6_bi], containers::array{6});
	containers::equal(batches[7_bi], containers::array{7});
	containers::equal(batches[8_bi], containers::array{8});
	containers::equal(batches[9_bi], containers::array{9});
	containers::equal(batches[10_bi], containers::array<int, 0_bi>{});
	containers::equal(batches[11_bi], containers::array<int, 0_bi>{});
	return true;
}());

static_assert([] {
	constexpr auto empty = containers::array<int, 0_bi>();
	constexpr auto number_of_batches = 1_bi;
	auto const batches = containers::batched(empty, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], empty);
	return true;
}());
