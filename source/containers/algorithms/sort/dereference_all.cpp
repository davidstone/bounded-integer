// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.dereference_all;

import containers.array;
import containers.iter_value_t;

import bounded;
import std_module;
export import tv;

namespace containers {

export template<typename T, auto size>
using element_refs = containers::array<std::reference_wrapper<T>, size>;

export template<typename Iterator>
constexpr auto dereference_all(Iterator it, auto const size) {
	auto const [...indexes] = bounded::index_sequence_struct(size);
	using value_type = iter_value_t<Iterator>;
	using elements_t = element_refs<value_type, size>;
	struct result_t {
		elements_t elements;
		Iterator last;
	};
	// Must use brace initialization for guaranteed evaluation order. Uses a
	// tuple instead of an array (even though all types are the same)
	// because we cannot have arrays of references. Cannot use CTAD because
	// we want to deduce references for the values.
	return result_t{
		elements_t{(static_cast<void>(indexes), *it++)...},
		std::move(it)
	};
}

} // namespace containers
