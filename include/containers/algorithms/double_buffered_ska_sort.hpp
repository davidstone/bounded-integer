// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/count.hpp>
#include <containers/algorithms/minmax_element.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/array/array.hpp>
#include <containers/begin_end.hpp>
#include <containers/front_back.hpp>
#include <containers/index_type.hpp>
#include <containers/integer_range.hpp>
#include <containers/range_value_t.hpp>
#include <containers/size.hpp>
#include <containers/to_radix_sort_key.hpp>

#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/forward.hpp>

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {
namespace extract {

using std::get;

template<typename T, std::size_t index, typename OriginalExtractor, typename CurrentExtractor>
struct return_type_impl {
private:
	using tuple = decltype(std::declval<CurrentExtractor const &>()(std::declval<T &&>()));
	using element_type = std::tuple_element_t<index, std::decay_t<tuple>>;
	using get_type = decltype(get<index>(std::declval<tuple>()));
	using translated_type = decltype(std::declval<OriginalExtractor const &>()(std::declval<get_type>()));
	static constexpr auto force_value =
		!std::is_reference_v<get_type> or
		(!std::is_reference_v<tuple> and !std::is_reference_v<element_type>);
public:
	using type = std::conditional_t<force_value, std::decay_t<translated_type>, translated_type>;
};

} // namespace extract

template<typename T, std::size_t index, typename OriginalExtractor, typename CurrentExtractor>
using extract_return_type_tuple = typename extract::return_type_impl<T, index, OriginalExtractor, CurrentExtractor>::type;


template<typename T, typename OriginalExtractor, typename CurrentExtractor>
struct extract_return_type_range_impl {
private:
	using range = decltype(std::declval<CurrentExtractor const &>()(std::declval<T &&>()));
	using get_type = range_value_t<range const &>;
	using translated_type = decltype(std::declval<OriginalExtractor const &>()(std::declval<get_type>()));
	// TODO: This doesn't work properly with range views
	static constexpr auto force_value =
		!std::is_reference_v<get_type> or
		!std::is_reference_v<range>;
public:
	using type = std::conditional_t<force_value, std::decay_t<translated_type>, translated_type>;
};

template<typename T, typename OriginalExtractor, typename CurrentExtractor>
using extract_return_type_range = typename extract_return_type_range_impl<T, OriginalExtractor, CurrentExtractor>::type;


constexpr void bool_sort_copy(auto & source, auto & output, auto const & extract_key) {
	// As an optimization, we could copy all initial false values and all
	// trailing true values, then only do this algorithm on the middle section.
	// I'm not sure if the increased code size is worth the benefit of iterating
	// over those values only once.
	auto false_position = containers::begin(output);
	auto true_position = containers::next(
		false_position,
		containers::count_if(source, containers::negate(extract_key))
	);
	for (auto && value : source) {
		auto & position = extract_key(value) ? true_position : false_position;
		*position = std::move(value);
		++position;
	}
}


template<range Source, range Buffer, typename ExtractKey>
constexpr bool double_buffered_numeric_sort(Source & source, Buffer & buffer, ExtractKey const & extract_key) {
	constexpr auto size = bounded::constant<sizeof(std::decay_t<decltype(extract_key(containers::front(source)))>)>;
	auto counts = containers::array<containers::array<std::size_t, 256>, static_cast<std::size_t>(size)>();
	auto const index_range = integer_range(size);

	for (auto const & value : source) {
		auto key = bounded::integer(extract_key(value));
		for (auto const index : index_range) {
			auto const inner_index = (key >> (index * 8_bi)) % 256_bi;
			++counts[index][inner_index];
		}
	}
	auto total = containers::array<std::size_t, static_cast<std::size_t>(size)>();
	for (auto const index : index_range) {
		auto & indexed_total = total[index];
		auto & count = counts[index];
		for (auto const i : containers::integer_range(256_bi)) {
			indexed_total += std::exchange(count[i], indexed_total);
		}
	}
	for (auto index_it = containers::begin(index_range); index_it != containers::end(index_range); ) {
		auto sort_segment_copy = [&](auto & current, auto & next) {
			for (auto && value : current) {
				auto const key = (bounded::integer(extract_key(value)) >> (*index_it * 8_bi)) % 256_bi;
				next[containers::index_type<Buffer>(counts[*index_it][key]++)] = std::move(value);
			}
			++index_it;
		};
		sort_segment_copy(source, buffer);
		if constexpr (size % 2_bi == 0_bi) {
			sort_segment_copy(buffer, source);
		}
	}
	return size % 2_bi == 1_bi;
}

constexpr auto double_buffered_sort_impl(range auto & source, range auto & buffer, auto const & original_extractor, auto const & current_extractor) -> bool;

template<typename OriginalExtractor, typename CurrentExtractor, std::size_t... indexes>
constexpr bool double_buffered_range_sort(range auto & source, range auto & buffer, OriginalExtractor const & original_extractor, CurrentExtractor const & current_extractor) {
	using key_t = std::decay_t<decltype(current_extractor(containers::front(source)))>;
	// Currently just sized ranges
	auto const max_size = [&]{
		if constexpr (requires { key_t::size(); }) {
			return key_t::size();
		} else if constexpr (requires { std::tuple_size<key_t>::value; }) {
			return std::tuple_size<key_t>::value;
		} else {
			// TODO: Support variable-sized ranges
#if 0
			return *containers::max_element(
				containers::transform(
					source,
					[](auto const & range) { return containers::size(range); }
				)
			);
#endif
		}
	}();
	bool which = false;
	for (auto const index : containers::reversed(containers::integer_range(max_size))) {
		auto extract_index = [&](auto && value) -> decltype(auto) {
			return static_cast<extract_return_type_range<decltype(value), OriginalExtractor, CurrentExtractor>>(original_extractor(current_extractor(OPERATORS_FORWARD(value))[index]));
		};
		if (which) {
			which = !containers::detail::double_buffered_sort_impl(buffer, source, original_extractor, extract_index);
		} else {
			which = containers::detail::double_buffered_sort_impl(source, buffer, original_extractor, extract_index);
		}
	}
	return which;
}

template<typename OriginalExtractor, typename CurrentExtractor, std::size_t... indexes>
constexpr bool double_buffered_tuple_sort(range auto & source, range auto & buffer, OriginalExtractor const & original_extractor, CurrentExtractor const & current_extractor, std::index_sequence<indexes...>) {
	auto which = false;
	auto do_iteration = [&]<auto index>(bounded::constant_t<index>) {
		using std::get;
		auto extract_index = [&](auto && value) -> decltype(auto) {
			using std::get;
			return static_cast<extract_return_type_tuple<decltype(value), index, OriginalExtractor, CurrentExtractor>>(original_extractor(get<index>(current_extractor(OPERATORS_FORWARD(value)))));
		};
		if (which) {
			which = !containers::detail::double_buffered_sort_impl(buffer, source, original_extractor, extract_index);
		} else {
			which = containers::detail::double_buffered_sort_impl(source, buffer, original_extractor, extract_index);
		}
	};
	(..., do_iteration(bounded::constant<sizeof...(indexes) - indexes - 1>));
	return which;
}

constexpr auto double_buffered_sort_impl(range auto & source, range auto & buffer, auto const & original_extractor, auto const & current_extractor) -> bool {
	using key_t = std::decay_t<decltype(current_extractor(containers::front(source)))>;
	if constexpr (std::is_same_v<key_t, bool>) {
		containers::detail::bool_sort_copy(source, buffer, current_extractor);
		return true;
	} else if constexpr (std::is_unsigned_v<key_t>) {
		return containers::detail::double_buffered_numeric_sort(source, buffer, current_extractor);
	} else if constexpr (containers::range<key_t>) {
		return containers::detail::double_buffered_range_sort(source, buffer, original_extractor, current_extractor);
	} else {
		static_assert(tuple_like<key_t>);
		return containers::detail::double_buffered_tuple_sort(source, buffer, original_extractor, current_extractor, std::make_index_sequence<std::tuple_size_v<key_t>>());
	}
}

} // namespace detail

struct double_buffered_ska_sort_t {
	constexpr auto operator()(range auto && source, range auto && buffer, auto const & extract_key) const -> bool {
		if constexpr (numeric_traits::max_value<typename std::remove_reference_t<decltype(source)>::size_type> <= bounded::constant<1>) {
			return false;
		} else {
			// TODO: Allow buffer to be larger, return range_view instead of bool
			// TODO: The actual algorithm doesn't require a sized range
			BOUNDED_ASSERT(containers::size(source) == containers::size(buffer));
			// This delegates to an implementation function to collapse lvalue and
			// rvalue code into a single instantiation and to let the tuple version
			// be recursive with the main function. We need to accept rvalues so
			// that users can easily pass in view types.
			return detail::double_buffered_sort_impl(source, buffer, extract_key, extract_key);
		}
	}

	constexpr auto operator()(range auto && source, range auto && buffer) const -> bool {
		return operator()(source, buffer, to_radix_sort_key);
	}
} inline constexpr double_buffered_ska_sort;

} // namespace containers