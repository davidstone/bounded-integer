// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace containers {
namespace detail {

template<typename T>
constexpr auto add_const(T & value) noexcept -> T const & {
	return value;
}
template<typename T>
constexpr auto add_const(T * value) noexcept -> T const * {
	return value;
}

template<typename T>
constexpr auto & remove_const(T const & value) noexcept {
	return const_cast<T &>(value);
}
template<typename T>
constexpr auto remove_const(T const * value) noexcept {
	return const_cast<T *>(value);
}

}	// namespace detail
}	// namespace containers
