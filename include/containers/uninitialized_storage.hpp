// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace containers {

template<typename T>
using uninitialized_storage = std::aligned_storage_t<sizeof(T), alignof(T)>;

// This is defined to allow more constexpr code. reinterpret_cast is not allowed
// in a constexpr function, so trivial_storage does not require a
// reinterpret_cast if T can be trivially used as storage.
template<typename T>
using trivial_storage = std::conditional_t<
	std::is_trivially_default_constructible<T>::value and std::is_trivially_destructible<T>::value,
	T,
	uninitialized_storage<T>
>;

}	// namespace containers
