// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.dereference_all;

import bounded;
import std_module;
export import tv;

namespace containers {

template<typename T, std::size_t>
using type_ignore_size = T;

export template<typename Iterator>
constexpr auto dereference_all(Iterator it, auto const size) {
	using T = decltype(*it);
	static_assert(std::is_lvalue_reference_v<T>);
	return [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
		// Must use brace initialization for guaranteed evaluation order. Uses a
		// tuple instead of an array (even though all types are the same)
		// because we cannot have arrays of references. Cannot use CTAD because
		// we want to deduce references for the values.
		return tv::tuple<type_ignore_size<T, indexes>..., Iterator>{(static_cast<void>(indexes), *it++)..., std::move(it)};
	}(bounded::make_index_sequence(size));
}

} // namespace containers
