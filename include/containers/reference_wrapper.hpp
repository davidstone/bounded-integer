// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward.hpp>
#include <bounded/detail/returns.hpp>
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

	template<typename... Args>
	constexpr auto operator()(Args && ... args) const BOUNDED_RETURNS(
		get()(BOUNDED_FORWARD(args)...)
	)

private:
	T * m_ptr;
};

template<typename T>
reference_wrapper(T &) -> reference_wrapper<T>;

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
constexpr auto && unwrap(reference_wrapper<T> reference) {
	return reference.get();
}
template<typename T>
constexpr auto && unwrap(T && reference) {
	return BOUNDED_FORWARD(reference);
}

}	// namespace containers
