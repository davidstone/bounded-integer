// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/relocate.hpp>

#include <type_traits>

namespace bounded {

template<typename T>
union single_element_storage {
	constexpr ~single_element_storage() {}
	T value;
};
template<typename T> requires std::is_trivially_destructible_v<T>
union single_element_storage<T> {
	~single_element_storage() = default;
	T value;
};

inline constexpr auto relocate_into_storage = [](auto & value) {
	return single_element_storage<std::remove_cvref_t<decltype(value)>>{relocate(value)};
};

} // namespace bounded
