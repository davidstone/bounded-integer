// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.single_element_storage;

import bounded;
import std_module;

namespace tv {

export template<typename T>
union [[clang::trivial_abi]] single_element_storage {
	constexpr ~single_element_storage() {}
	T value;
};
template<typename T> requires bounded::trivially_destructible<T>
union single_element_storage<T> {
	~single_element_storage() = default;
	T value;
};

export constexpr auto relocate_into_storage = [](auto & value) {
	return single_element_storage<std::remove_cvref_t<decltype(value)>>{bounded::relocate(value)};
};

} // namespace tv
