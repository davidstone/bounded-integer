// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.uninitialized;

import containers.algorithms.destroy_range;
import containers.algorithms.copy_or_relocate_from;
import containers.begin_end;
import containers.data;
import containers.iter_difference_t;
import containers.iter_value_t;
import containers.iterator;
import containers.range;
import containers.range_value_t;
import containers.range_view;
import containers.size;
import containers.to_address;

import bounded;
import std_module;

namespace containers {

template<typename InputRange, typename OutputIterator>
concept memcpyable =
	contiguous_range<InputRange> and
	to_addressable<OutputIterator> and
	std::same_as<range_value_t<InputRange>, iter_value_t<OutputIterator>> and
	std::is_trivially_copyable_v<iter_value_t<OutputIterator>>;

template<typename Iterator>
struct construct_at_output {
	explicit constexpr construct_at_output(Iterator & output):
		m_output(output)
	{
	}
	constexpr auto operator()(auto make) const -> void {
		bounded::construct_at(*m_output, make);
		++m_output;
	}
private:
	Iterator & m_output;
};

export constexpr auto uninitialized_copy = [](range auto && input, iterator auto output) {
	auto out_first = output;
	try {
		copy_or_relocate_from(OPERATORS_FORWARD(input), construct_at_output(output));
	} catch (...) {
		containers::destroy_range(range_view(out_first, output));
		throw;
	}
	return output;
};

// std::memcpy does not allow either argument to be null, even when size is 0
constexpr auto memcpy(void * destination, void const * source, std::size_t const size) {
	#if defined __clang__
		return __builtin_memcpy(destination, source, size);
	#else
		if (size == 0) {
			return destination;
		}
		return std::memcpy(destination, source, size);
	#endif
}

export constexpr auto uninitialized_copy_no_overlap = []<range InputRange, iterator OutputIterator>(InputRange && source, OutputIterator out) {
	// TODO: Figure out how to tell the optimizer there is no overlap so I do
	// not need to explicitly call `memcpy`.
	if constexpr (memcpyable<InputRange, OutputIterator>) {
		if consteval {
			return uninitialized_copy(OPERATORS_FORWARD(source), out);
		} else {
			auto const offset = containers::size(source);
			::containers::memcpy(
				containers::to_address(out),
				containers::data(source),
				static_cast<std::size_t>(offset) * sizeof(range_value_t<InputRange>)
			);
			return out + ::bounded::assume_in_range<iter_difference_t<OutputIterator>>(offset);
		}
	} else {
		return uninitialized_copy(OPERATORS_FORWARD(source), out);
	}
};

export constexpr auto uninitialized_relocate = [](range auto && input, iterator auto output) {
	auto const last = containers::end(OPERATORS_FORWARD(input));
	for (auto it = containers::begin(OPERATORS_FORWARD(input)); it != last; ++it) {
		bounded::construct_at(*output, [&] {
			if constexpr (std::is_reference_v<decltype(*it)>) {
				return bounded::relocate(*it);
			} else {
				return *it;
			}
		});
		++output;
	}
	return output;
};

export constexpr auto uninitialized_relocate_no_overlap = []<range InputRange, iterator OutputIterator>(InputRange && source, OutputIterator out) {
	if constexpr (memcpyable<InputRange, OutputIterator>) {
		auto result = uninitialized_copy_no_overlap(source, out);
		::containers::destroy_range(source);
		return result;
	} else {
		return uninitialized_relocate(OPERATORS_FORWARD(source), out);
	}
};

} // namespace containers
