// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/requires.hpp>

#include <memory>
#include <type_traits>

namespace bounded {
namespace detail {

template<typename T>
constexpr auto has_member_addressof(std::remove_reference_t<decltype(std::declval<T &>().operator&())> *) noexcept { return true; }
template<typename T>
constexpr auto has_member_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto has_free_addressof(std::remove_reference_t<decltype(operator&(std::declval<T &>()))> *) noexcept { return true; }
template<typename T>
constexpr auto has_free_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto has_addressof(std::remove_reference_t<decltype(&(std::declval<T &>()))> *) noexcept { return true; }
template<typename T>
constexpr auto has_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto constexpr_addressof =
	!has_member_addressof<T>(nullptr) and
	!has_free_addressof<T>(nullptr) and
	has_addressof<T>(nullptr);

}	// namespace detail

template<typename T>
constexpr auto addressof(T & t) noexcept {
	if constexpr (detail::constexpr_addressof<T>) {
		return &t;
	} else {
		return std::addressof(t);
	}
}

}	// namespace bounded
