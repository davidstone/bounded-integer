// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/requires.hpp>

#include <cassert>
#include <limits>
#include <type_traits>

namespace bounded {

template<typename T>
struct basic_numeric_limits;

template<>
struct basic_numeric_limits<detail::max_unsigned_t> {
	static constexpr auto min() noexcept {
		return static_cast<detail::max_unsigned_t>(0);
	}
	static constexpr auto max() noexcept {
		return static_cast<detail::max_unsigned_t>(-1);
	}
	static constexpr auto is_specialized = true;
	static constexpr auto is_integer = true;
	static constexpr auto is_bounded = true;
};

template<>
struct basic_numeric_limits<detail::max_signed_t> {
	static constexpr auto max() noexcept {
		return static_cast<detail::max_signed_t>(basic_numeric_limits<detail::max_unsigned_t>::max() / 2);
	}
	// TODO: Implement this properly
	static constexpr auto min() noexcept {
		return -max() - 1;
	}
	static constexpr auto is_specialized = true;
	static constexpr auto is_integer = true;
	static constexpr auto is_bounded = true;
};

namespace detail {

template<auto value>
constexpr auto normalize = static_cast<std::conditional_t<
	value < 0 or static_cast<max_unsigned_t>(value) <= basic_numeric_limits<max_signed_t>::max(),
	max_signed_t,
	max_unsigned_t
>>(value);

template<typename T, BOUNDED_REQUIRES(is_signed_builtin<T>)>
constexpr auto from_unsigned_cast(max_unsigned_t const value) noexcept {
	using limits = basic_numeric_limits<max_signed_t>;
	if (value <= limits::max()) {
		return static_cast<max_signed_t>(value);
	} else {
		assert(value >= static_cast<max_unsigned_t>(limits::min()));
		return static_cast<max_signed_t>(value - static_cast<max_unsigned_t>(limits::min())) + limits::min();
	}
}

template<typename T, BOUNDED_REQUIRES(is_unsigned_builtin<T>)>
constexpr auto from_unsigned_cast(max_unsigned_t const value) noexcept {
	return value;
}

}	// namespace detail

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
		return detail::normalize<real_numeric_limits::min()>;
	}
	static constexpr auto max() noexcept {
		return detail::normalize<real_numeric_limits::max()>;
	}
	static constexpr auto is_specialized = real_numeric_limits::is_specialized;
	static constexpr auto is_integer = real_numeric_limits::is_integer;
	static constexpr auto is_bounded = real_numeric_limits::is_bounded;
};

template<auto minimum, auto maximum, typename overflow_policy, storage_type storage, bool poisoned>
struct basic_numeric_limits<integer<minimum, maximum, overflow_policy, storage, poisoned>> {
	static constexpr auto min() noexcept {
		return detail::normalize<minimum>;
	}
	static constexpr auto max() noexcept {
		return detail::normalize<maximum>;
	}
	static constexpr auto is_specialized = true;
	static constexpr auto is_integer = true;
	static constexpr auto is_bounded = true;
};

template<typename T, T value>
struct basic_numeric_limits<std::integral_constant<T, value>> {
	static constexpr auto min() noexcept {
		return detail::normalize<value>;
	}
	static constexpr auto max() noexcept {
		return detail::normalize<value>;
	}
	static constexpr auto is_specialized = std::numeric_limits<T>::is_specialized;
	static constexpr auto is_integer = std::numeric_limits<T>::is_integer;
	static constexpr auto is_bounded = true;
};

}	// namespace bounded
