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

inline constexpr auto uninitialized_copy = [](range auto && input, iterator auto output) {
	auto out_first = output;
	try {
		auto const last = containers::end(OPERATORS_FORWARD(input));
		for (auto first = containers::begin(OPERATORS_FORWARD(input)); first != last; ++first) {
			bounded::construct(*output, [&]() -> decltype(auto) { return *first; });
			++output;
		}
	} catch (...) {
		containers::destroy_range(range_view(out_first, output));
		throw;
	}
	return output;
};

inline constexpr auto uninitialized_copy_no_overlap = []<range InputRange, iterator OutputIterator>(InputRange && source, OutputIterator out) {
	// TODO: Figure out how to tell the optimizer there is no overlap so I do
	// not need to explicitly call `memcpy`.
	if constexpr (detail::memcpyable<InputRange, OutputIterator>) {
		if (std::is_constant_evaluated()) {
			return uninitialized_copy(OPERATORS_FORWARD(source), out);
		} else {
			auto const offset = containers::size(source);
			std::memcpy(containers::to_address(out), containers::data(source), static_cast<std::size_t>(offset) * sizeof(range_value_t<InputRange>));
			return out + static_cast<iter_difference_t<OutputIterator>>(offset);
		}
	} else {
		return uninitialized_copy(OPERATORS_FORWARD(source), out);
	}
};

inline constexpr auto uninitialized_relocate = [](range auto && source, iterator auto out) {
	return uninitialized_copy(
		::containers::detail::relocate_range_adapter(OPERATORS_FORWARD(source)),
		out
	);
};

inline constexpr auto uninitialized_relocate_no_overlap = []<range InputRange, iterator OutputIterator>(InputRange && source, OutputIterator out) {
	if constexpr (detail::memcpyable<InputRange, OutputIterator>) {
		auto result = ::containers::uninitialized_copy(source, out);
		containers::destroy_range(source);
		return result;
	} else {
		return uninitialized_relocate(OPERATORS_FORWARD(source), out);
	}
};

} // namespace containers
