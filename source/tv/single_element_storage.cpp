// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.single_element_storage;

import bounded;
import std_module;

namespace tv {

export template<typename T>
union [[clang::trivial_abi]] single_element_storage {
	single_element_storage() requires bounded::trivially_default_constructible<T> = default;
	constexpr single_element_storage() {}
	explicit constexpr single_element_storage(bounded::construct_function_for<T> auto && construct):
		value(OPERATORS_FORWARD(construct)())
	{
	}
	single_element_storage(single_element_storage &&) = default;
	single_element_storage(single_element_storage const &) = default;
	auto operator=(single_element_storage &&) & -> single_element_storage & = default;
	auto operator=(single_element_storage const &) & -> single_element_storage & = default;
	~single_element_storage() requires bounded::trivially_destructible<T> = default;
	constexpr ~single_element_storage() {}
	T value;
};

export constexpr auto relocate_into_storage = [](auto & value) {
	return single_element_storage<std::remove_cvref_t<decltype(value)>>(
		[&] { return bounded::relocate(value); }
	);
};

} // namespace tv
