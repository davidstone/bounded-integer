// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/forward.hpp>

namespace containers {

struct is_partitioned_t {
	constexpr auto operator()(range auto && input, auto predicate) const -> bool {
		auto first = containers::begin(input);
		auto last = containers::end(input);
		auto it = containers::find_if_not(first, last, predicate);
		return containers::find_if(it, last, predicate) == last;
	}
} constexpr inline is_partitioned;

struct partition_point_t {
	template<range Input>
	constexpr auto operator()(Input && input, auto predicate) const {
		auto count = bounded::integer<0, bounded::builtin_max_value<range_size_t<Input>>>(containers::size(input));
		auto first = containers::begin(input);
		if constexpr (numeric_traits::max_value<decltype(count)> == bounded::constant<0>) {
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
} constexpr inline partition_point;

namespace detail {

template<typename T>
concept decrementable = requires(T value) { --value; };

} // namespace detail

struct partition_t {
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
				using std::swap;
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
					using std::swap;
					swap(*it, *first);
					++first;
				}
			}
		}
		return first;
	}
	constexpr auto operator()(range auto && input, auto predicate) const {
		return operator()(containers::begin(input), containers::end(input), predicate);
	}
} constexpr inline partition;

} // namespace containers
