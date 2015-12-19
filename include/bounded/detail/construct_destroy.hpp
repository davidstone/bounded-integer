// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/addressof.hpp>
#include <bounded/detail/requires.hpp>

#include <memory>
#include <type_traits>

namespace bounded {

template<typename T, typename... Args, BOUNDED_REQUIRES(!std::is_trivially_copyable<T>::value)>
auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	::new(static_cast<void *>(::bounded::addressof(ref))) T(std::forward<Args>(args)...)
)
template<typename T, typename... Args, BOUNDED_REQUIRES(std::is_trivially_copyable<T>::value)>
constexpr auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	ref = T(std::forward<Args>(args)...)
)

template<typename T, BOUNDED_REQUIRES(!std::is_trivially_destructible<T>::value)>
auto destroy(T & ref) BOUNDED_NOEXCEPT(
	ref.~T()
)
template<typename T, BOUNDED_REQUIRES(std::is_trivially_destructible<T>::value)>
constexpr auto destroy(T &) noexcept {
}

}	// namespace bounded
