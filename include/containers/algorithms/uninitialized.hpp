// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/relocate_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>
#include <containers/to_address.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <cstring>
#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

// reinterpret_cast or static_cast from void * are not allowed in constexpr
template<typename Target, typename Source>
concept static_castable = requires(Source source) { static_cast<Target>(source); };

template<typename Target>
constexpr auto static_or_reinterpret_cast(auto && source) {
	if constexpr (static_castable<Target, decltype(source)>) {
		return static_cast<Target>(source);
	} else {
		return reinterpret_cast<Target>(source);
	}
}

template<typename InputIterator, typename Sentinel, typename OutputIterator>
concept memcpyable =
	to_addressable<InputIterator> and
	random_access_sentinel_for<Sentinel, InputIterator> and
	to_addressable<OutputIterator> and
	std::is_same_v<iter_value_t<InputIterator>, iter_value_t<OutputIterator>> and
	std::is_trivially_copyable_v<iter_value_t<OutputIterator>>;

} // namespace detail


template<iterator InputIterator, sentinel_for<InputIterator> Sentinel, iterator OutputIterator>
constexpr auto uninitialized_copy(InputIterator first, Sentinel const last, OutputIterator out) {
	auto slow_path = [&] {
		auto out_first = out;
		try {
			for (; first != last; ++first) {
				bounded::construct(*out, [&] { return *first; });
				++out;
			}
		} catch (...) {
			containers::destroy_range(range_view(out_first, out));
			throw;
		}
		return out;
	};
	if constexpr (detail::memcpyable<InputIterator, Sentinel, OutputIterator>) {
		if (std::is_constant_evaluated()) {
			return slow_path();
		} else {
			auto const offset = last - first;
			std::memcpy(containers::to_address(out), containers::to_address(first), static_cast<std::size_t>(offset) * sizeof(iter_value_t<InputIterator>));
			return out + static_cast<iter_difference_t<OutputIterator>>(offset);
		}
	} else {
		return slow_path();
	}
}

constexpr auto uninitialized_copy(range auto && input, iterator auto output) {
	return uninitialized_copy(
		containers::begin(OPERATORS_FORWARD(input)),
		containers::end(OPERATORS_FORWARD(input)),
		output
	);
}


template<iterator InputIterator>
constexpr auto uninitialized_move(InputIterator const first, sentinel_for<InputIterator> auto const last, iterator auto const out) {
	return ::containers::uninitialized_copy(::containers::move_iterator(first), ::containers::move_iterator(last), out);
}

template<bidirectional_iterator BidirectionalInputIterator>
constexpr auto uninitialized_copy_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, bidirectional_iterator auto const out_last) {
	return containers::uninitialized_copy(
		containers::reverse_iterator(last),
		containers::reverse_iterator(first),
		containers::reverse_iterator(out_last)
	).base();
}

template<bidirectional_iterator BidirectionalInputIterator>
constexpr auto uninitialized_move_backward(BidirectionalInputIterator const first, BidirectionalInputIterator const last, bidirectional_iterator auto const out_last) {
	return containers::uninitialized_copy_backward(
		containers::move_iterator(first),
		containers::move_iterator(last),
		out_last
	);
}


template<iterator InputIterator>
constexpr auto uninitialized_relocate(InputIterator const first, sentinel_for<InputIterator> auto const last, iterator auto out) {
	return ::containers::uninitialized_copy(
		::containers::relocate_iterator(first),
		::containers::relocate_iterator(last),
		out
	);
}

constexpr auto uninitialized_relocate(range auto && source, iterator auto out) {
	return ::containers::uninitialized_relocate(
		containers::begin(OPERATORS_FORWARD(source)),
		containers::end(OPERATORS_FORWARD(source)),
		out
	);
}


template<iterator ForwardIterator>
constexpr auto uninitialized_default_construct(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last) {
	auto it = first;
	try {
		for (; it != last; ++it) {
			bounded::construct(*it);
		}
	} catch (...) {
		containers::destroy_range(range_view(first, it));
		throw;
	}
}

}	// namespace containers
