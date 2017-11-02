// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace bounded {

// This does not requiring having a definition of bounded::integer to get at the
// minimum and maximum, so it can be used in the definition of bounded::integer.
template<typename T>
struct basic_numeric_limits {
private:
	using real_numeric_limits = std::conditional_t<std::is_same<T, std::decay_t<T>>{},
		std::numeric_limits<T>,
		basic_numeric_limits<std::decay_t<T>>
	>;
public:
	static constexpr auto min() noexcept {
		return real_numeric_limits::min();
	}
	static constexpr auto max() noexcept {
		return real_numeric_limits::max();
	}
	static constexpr auto is_specialized = real_numeric_limits::is_specialized;
	static constexpr auto is_integer = real_numeric_limits::is_integer;
	static constexpr auto is_bounded = real_numeric_limits::is_bounded;
};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
struct basic_numeric_limits<integer<minimum, maximum, overflow_policy, storage, poisoned>> {
	static constexpr auto min() noexcept {
		return minimum;
	}
	static constexpr auto max() noexcept {
		return maximum;
	}
	static constexpr auto is_specialized = true;
	static constexpr auto is_integer = true;
	static constexpr auto is_bounded = true;
};

template<typename T, T value>
struct basic_numeric_limits<std::integral_constant<T, value>> {
	static constexpr auto min() noexcept {
		return value;
	}
	static constexpr auto max() noexcept {
		return value;
	}
	static constexpr auto is_specialized = std::numeric_limits<T>::is_specialized;
	static constexpr auto is_integer = std::numeric_limits<T>::is_integer;
	static constexpr auto is_bounded = true;
};

}	// namespace bounded
