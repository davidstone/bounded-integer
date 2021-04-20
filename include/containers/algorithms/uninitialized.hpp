// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/relocate_range_adapter.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/data.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/range_value_t.hpp>
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

template<typename InputRange, typename OutputIterator>
concept memcpyable =
	contiguous_range<InputRange> and
	to_addressable<OutputIterator> and
	std::is_same_v<range_value_t<InputRange>, iter_value_t<OutputIterator>> and
	std::is_trivially_copyable_v<iter_value_t<OutputIterator>>;

} // namespace detail

struct uninitialized_copy_t {
	template<iterator InputIterator, sentinel_for<InputIterator> Sentinel, iterator OutputIterator>
	constexpr auto operator()(InputIterator first, Sentinel const last, OutputIterator out) const {
		auto out_first = out;
		try {
			for (; first != last; ++first) {
				bounded::construct(*out, [&]() -> decltype(auto) { return *first; });
				++out;
			}
		} catch (...) {
			containers::destroy_range(range_view(out_first, out));
			throw;
		}
		return out;
	}

	constexpr auto operator()(range auto && input, iterator auto output) const {
		return operator()(
			containers::begin(OPERATORS_FORWARD(input)),
			containers::end(OPERATORS_FORWARD(input)),
			output
		);
	}
} inline constexpr uninitialized_copy;


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


template<range InputRange, iterator OutputIterator>
constexpr auto uninitialized_relocate(InputRange && source, OutputIterator out) {
	if constexpr (detail::memcpyable<InputRange, OutputIterator>) {
		auto result = ::containers::uninitialized_copy(source, out);
		containers::destroy_range(source);
		return result;
	} else {
		return ::containers::uninitialized_copy(
			::containers::detail::relocate_range_adapter(OPERATORS_FORWARD(source)),
			out
		);
	}
}


// TODO: Define in terms of uninitialized_copy + repeat_default_n?
template<forward_iterator ForwardIterator, sentinel_for<ForwardIterator> Sentinel>
constexpr auto uninitialized_default_construct(ForwardIterator const first, Sentinel const last) {
	auto it = first;
	try {
		for (; it != last; ++it) {
			bounded::construct(*it, bounded::construct_return<iter_value_t<ForwardIterator>>);
		}
	} catch (...) {
		containers::destroy_range(range_view(first, it));
		throw;
	}
}

}	// namespace containers
