// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/is_range.hpp>

#include <bounded/assert.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <limits>

namespace containers {

template<typename LHS, typename RHS>
constexpr void swap(LHS & lhs, RHS & rhs) {
	auto temp = std::move(lhs);
	lhs = std::move(rhs);
	rhs = std::move(temp);
}

constexpr inline struct is_partitioned_t {
	template<typename Range, typename UnaryPredicate>
	constexpr auto operator()(Range && range, UnaryPredicate predicate) const -> bool {
		auto first = begin(range);
		auto last = end(range);
		auto it = containers::find_if_not(first, last, predicate);
		return containers::find_if(it, last, predicate) == last;
	}
} is_partitioned;

constexpr inline struct partition_point_t {
	template<typename Range, typename UnaryPredicate> requires is_range<Range>
	constexpr auto operator()(Range && range, UnaryPredicate predicate) const {
		using size_type = decltype(size(range));
		auto count = bounded::integer<0, bounded::detail::normalize<std::numeric_limits<size_type>::max().value()>>(size(range));
		auto first = begin(range);
		if constexpr (count.max() == bounded::constant<0>) {
			return first;
		} else {
			while (count > bounded::constant<0>) {
				auto it = first;
				auto const step = count / bounded::constant<2>;
				::containers::advance(it, step);
				if (predicate(*it)) {
					first = ::containers::next(it);
					count -= step + bounded::constant<1>;
				} else {
					count = step;
				}
			}
			return first;
		}
	}
} partition_point;


namespace detail {

template<typename Iterator>
concept reversible_iterator = requires(Iterator it) {
	--it;
};

} // namespace detail

// TODO: Support bidirectional iterators
constexpr inline struct partition_t {
	template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate> requires is_iterator_sentinel<ForwardIterator, Sentinel>
	constexpr auto operator()(ForwardIterator first, Sentinel last, UnaryPredicate predicate) const {
		auto advance_first = [&]{
			first = containers::find_if_not(first, last, predicate);
		};
		advance_first();

		if constexpr (detail::reversible_iterator<Sentinel>) {
			auto advance_last = [&]{
				while (first != last) {
					--last;
					if (predicate(*last)) {
						break;
					}
				}
			};

			advance_last();
			while (first != last) {
				swap(*first, *last);
				advance_first();
				if (first == last) {
					break;
				}
				advance_last();
			}
		} else {
			if (first == last) {
				return first;
			}
			for (auto it = containers::next(first); it != last; ++it) {
				if (predicate(*it)) {
					swap(*it, *first);
					++first;
				}
			}
		}
		return first;
	}
	template<typename Range, typename UnaryPredicate> requires is_range<Range>
	constexpr auto operator()(Range && range, UnaryPredicate predicate) const {
		return operator()(begin(range), end(range), predicate);
	}
} partition;

}	// namespace containers
