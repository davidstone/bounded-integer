// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>

#include <operators/returns.hpp>

#include <memory>
#include <utility>

namespace containers {

// Until the standard has a constexpr version
template<typename T>
struct reference_wrapper {
	using type = T;

	constexpr reference_wrapper(T & ref):
		m_ptr(std::addressof(ref))
	{
	}

	constexpr operator T &() const {
		return *m_ptr;
	}
	constexpr T & get() const {
		return *m_ptr;
	}

	// TODO: Use terse syntax when clang does not crash
	template<typename... Args>
	constexpr auto operator()(Args && ... args) const OPERATORS_RETURNS(
		get()(OPERATORS_FORWARD(args)...)
	)

private:
	T * m_ptr;
};

template<typename T>
reference_wrapper(T &) -> reference_wrapper<T>;

template<typename T>
inline constexpr auto is_reference_wrapper = false;

template<typename T>
inline constexpr auto is_reference_wrapper<reference_wrapper<T>> = true;

template<typename T>
inline constexpr auto is_reference_wrapper<std::reference_wrapper<T>> = true;

template<typename T>
constexpr auto ref(T & value) {
	return reference_wrapper<T>(value);
}

template<typename T>
constexpr auto cref(T const & value) {
	return reference_wrapper<T const>(value);
}
template<typename T>
constexpr auto cref(T && value) = delete;

template<typename T>
constexpr auto && unwrap(T && reference) {
	if constexpr (is_reference_wrapper<std::remove_cvref_t<T>>) {
		return reference.get();
	} else {
		return OPERATORS_FORWARD(reference);
	}
}

}	// namespace containers
