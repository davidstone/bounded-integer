// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.algorithms.drop;

import containers.algorithms.advance;
import containers.algorithms.concatenate;
import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_random_access_iterator;
import containers.integer_range;
import containers.iter_difference_t;
import containers.iterator;
import containers.iterator_addable;
import containers.iterator_category_base;
import containers.iterator_t;
import containers.random_access_sentinel_for;
import containers.range;
import containers.range_size_t;
import containers.sentinel_t;
import containers.size;
import containers.sized_range;
import containers.static_string;
import containers.subrange;
import containers.subtractable;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

namespace containers {

enum class extra_elements_policy {
	drop, exception
};

template<typename Sentinel>
struct reduced_range_sentinel {
	constexpr explicit reduced_range_sentinel(Sentinel sentinel):
		m_sentinel(std::move(sentinel))
	{
	}

	constexpr auto base() const {
		return m_sentinel;
	}

private:
	[[no_unique_address]] Sentinel m_sentinel;
};

struct drop_exception : std::runtime_error {
	explicit drop_exception(std::string_view const str):
		std::runtime_error(containers::concatenate<std::string>(
			"Tried to drop more elements than were in the range: "_s,
			str
		))
	{
	}
};

template<typename Iterator, auto max_size>
struct reduced_range_iterator {
	using difference_type = bounded::integer<bounded::normalize<-max_size>, bounded::normalize<max_size>>;

	constexpr explicit reduced_range_iterator(Iterator it):
		m_it(std::move(it))
	{
	}

	constexpr auto base() const & -> Iterator const & {
		return m_it;
	}
	constexpr auto base() && -> Iterator && {
		return m_it;
	}

	constexpr auto operator*() const -> decltype(auto) {
		return *m_it;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend auto operator<=>(reduced_range_iterator const &, reduced_range_iterator const &) = default;
	template<std::three_way_comparable<Iterator> Sentinel>
	friend constexpr auto operator<=>(reduced_range_iterator const & lhs, reduced_range_sentinel<Sentinel> const & rhs) {
		return lhs.m_it <=> rhs.base();
	}
	template<bounded::equality_comparable<Iterator> Sentinel>
	friend constexpr auto operator==(reduced_range_iterator const & lhs, reduced_range_sentinel<Sentinel> const & rhs) {
		return lhs.m_it == rhs.base();
	}

	template<typename Offset> requires(
		iterator_addable<Iterator, Offset> and numeric_traits::min_value<Offset> <= max_size
	)
	friend constexpr auto operator+(reduced_range_iterator it, Offset offset) -> reduced_range_iterator {
		return reduced_range_iterator(std::move(it).m_it + offset);
	}
	friend constexpr auto operator+(reduced_range_iterator it, bounded::constant_t<1>) -> reduced_range_iterator {
		if constexpr (max_size == 0_bi) {
			std::unreachable();
		} else {
			++it.m_it;
			return it;
		}
	}
	friend constexpr auto operator+(reduced_range_iterator it, bounded::constant_t<-1>) -> reduced_range_iterator {
		if constexpr (max_size == 0_bi) {
			std::unreachable();
		} else {
			--it.m_it;
			return it;
		}
	}

	friend constexpr auto operator-(reduced_range_iterator const lhs, reduced_range_iterator const rhs) -> difference_type
		requires subtractable<Iterator>
	{
		return ::bounded::assume_in_range<difference_type>(lhs.m_it - rhs.m_it);
	}

	template<subtractable<Iterator> Sentinel>
	friend constexpr auto operator-(reduced_range_sentinel<Sentinel> const lhs, reduced_range_iterator const & rhs) -> difference_type {
		return ::bounded::assume_in_range<difference_type>(lhs.base() - rhs.m_it);
	}

private:
	[[no_unique_address]] Iterator m_it;
};

template<auto max_size, typename T>
constexpr auto iterator_or_sentinel(T value) {
	if constexpr (iterator<T>) {
		return reduced_range_iterator<T, max_size>(std::move(value));
	} else {
		return reduced_range_sentinel<T>(std::move(value));
	}
}

template<extra_elements_policy const policy, range Range, bounded::bounded_integer Count>
constexpr auto drop_impl(Range && source, Count const count) {
	using iterator = iterator_t<Range>;
	auto const last = containers::end(OPERATORS_FORWARD(source));

	constexpr auto max_remaining = bounded::max(
		bounded::integer(numeric_traits::max_value<range_size_t<Range>>) - numeric_traits::min_value<Count>,
		0_bi
	);
	using drop_iterator = reduced_range_iterator<iterator, max_remaining>;

	if constexpr (sized_range<Range>) {
		auto const source_size = bounded::integer(containers::size(source));
		auto const remaining = [&] {
			if constexpr (policy == extra_elements_policy::exception) {
				return bounded::check_in_range<drop_exception>(
					source_size - count,
					0_bi,
					numeric_traits::max_value<range_size_t<Range>>
				);
			} else {
				return bounded::max(source_size - count, 0_bi);
			}
		}();
		auto const to_remove = bounded::min(source_size, count);
		auto first = containers::next(containers::begin(OPERATORS_FORWARD(source)), to_remove);
		return containers::subrange(
			drop_iterator(std::move(first)),
			iterator_or_sentinel<max_remaining>(last),
			remaining
		);
	} else {
		auto first = containers::begin(OPERATORS_FORWARD(source));
		for (auto const _ : containers::integer_range(count)) {
			if (first == last) {
				if constexpr (policy == extra_elements_policy::exception) {
					throw drop_exception("unsized range");
				} else {
					break;
				}
			}
			++first;
		}
		return containers::subrange(
			drop_iterator(std::move(first)),
			iterator_or_sentinel<max_remaining>(last)
		);
	}
}

export template<range Range, bounded::bounded_integer Count> requires(numeric_traits::min_value<Count> >= 0_bi)
constexpr auto drop(Range && source, Count const count) {
	return drop_impl<extra_elements_policy::drop>(OPERATORS_FORWARD(source), count);
}

export template<range Range, bounded::bounded_integer Count>
	requires(
		numeric_traits::min_value<Count> >= 0_bi and
		numeric_traits::max_value<Count> <= numeric_traits::max_value<range_size_t<Range>>
	)
constexpr auto drop_exactly(Range && source, Count const count) {
	return drop_impl<extra_elements_policy::exception>(OPERATORS_FORWARD(source), count);
}

} // namespace containers
