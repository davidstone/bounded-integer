// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <bounded/assert.hpp>
#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <limits>

namespace containers {

constexpr void swap(auto & lhs, auto & rhs) {
	auto temp = std::move(lhs);
	lhs = std::move(rhs);
	rhs = std::move(temp);
}

constexpr inline struct is_partitioned_t {
	constexpr auto operator()(range auto && input, auto predicate) const -> bool {
		auto first = begin(input);
		auto last = end(input);
		auto it = containers::find_if_not(first, last, predicate);
		return containers::find_if(it, last, predicate) == last;
	}
} is_partitioned;

constexpr inline struct partition_point_t {
	constexpr auto operator()(range auto && input, auto predicate) const {
		using size_type = decltype(containers::size(input));
		auto count = bounded::integer<0, bounded::detail::builtin_max_value<size_type>>(containers::size(input));
		auto first = begin(input);
		if constexpr (bounded::max_value<decltype(count)> == bounded::constant<0>) {
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

template<typename T>
concept decrementable = requires(T value) { --value; };

} // namespace detail

constexpr inline struct partition_t {
	template<iterator ForwardIterator>
	constexpr auto operator()(ForwardIterator first, sentinel_for<ForwardIterator> auto last, auto predicate) const {
		auto advance_first = [&]{
			first = containers::find_if_not(first, last, predicate);
		};
		advance_first();

		if constexpr (detail::decrementable<decltype(last)>) {
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
	constexpr auto operator()(range auto && input, auto predicate) const {
		return operator()(begin(input), end(input), predicate);
	}
} partition;

}	// namespace containers
