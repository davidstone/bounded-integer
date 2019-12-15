// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/log.hpp>
#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

enum class count_sign { all, negative };

template<typename T, count_sign sign>
constexpr auto digits_impl(auto const base) {
	auto safe_log = [=](auto value) {
		if constexpr (value == constant<0>) {
			return constant<0>;
		} else if constexpr (value > constant<0>) {
			return ::bounded::log(value, base);
		} else {
			return ::bounded::log(-value, base) + constant<sign == count_sign::negative>;
		}
	};
	auto const representation_min = safe_log(constant<detail::builtin_min_value<T>>);
	auto const representation_max = safe_log(constant<detail::builtin_max_value<T>>);
	constexpr auto sign_bits = constant<detail::builtin_min_value<T> < 0 and sign == count_sign::all>;
	return ::bounded::max(representation_min, representation_max) + sign_bits + constant<1>;
}

} // namespace detail

template<typename T>
constexpr auto digits_in_string_representation(auto const base) {
	return ::bounded::detail::digits_impl<T, detail::count_sign::negative>(base);
}

// This can be used for determining the size of a bitfield of a value
template<typename T>
inline constexpr auto representation_bits = ::bounded::detail::digits_impl<T, detail::count_sign::all>(constant<2>);

}	// namespace bounded
