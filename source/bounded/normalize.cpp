// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.normalize;

import bounded.homogeneous_equals;
import bounded.is_bounded_integer;

import numeric_traits;
import std_module;

namespace bounded {

template<typename T>
constexpr auto as_builtin_integer(T const x) {
	if constexpr (bounded_integer<T>) {
		return x.value();
	} else if constexpr (detail::is_integral_constant_of_integral<T>) {
		return x.get();
	} else if constexpr (std::is_enum_v<T>) {
		return std::to_underlying(x);
	} else {
		static_assert(builtin_integer<T> or std::same_as<T, bool>);
		return x;
	}
}

constexpr auto must_use_unsigned(auto const value) {
	return value > 0 and static_cast<numeric_traits::max_unsigned_t>(value) > static_cast<numeric_traits::max_unsigned_t>(numeric_traits::max_value<numeric_traits::max_signed_t>);
}

template<typename T>
constexpr auto fits_in_int(T const value) {
	return (value < static_cast<T>(0)) ?
		value >= static_cast<T>(numeric_traits::min_value<int>) :
		value <= static_cast<T>(numeric_traits::max_value<int>);
}

export template<auto value>
constexpr auto normalize = static_cast<
	std::conditional_t<::bounded::fits_in_int(+::bounded::as_builtin_integer(value)), int,
	std::conditional_t<::bounded::must_use_unsigned(::bounded::as_builtin_integer(value)), numeric_traits::max_unsigned_t,
	numeric_traits::max_signed_t
>>>(value);

} // namespace bounded

namespace {

static_assert(homogeneous_equals(bounded::normalize<0>, 0));
static_assert(homogeneous_equals(bounded::normalize<1>, 1));
static_assert(homogeneous_equals(bounded::normalize<-1>, -1));
static_assert(homogeneous_equals(bounded::normalize<0U>, 0));
static_assert(homogeneous_equals(bounded::normalize<static_cast<unsigned char>(0)>, 0));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::max_value<numeric_traits::max_unsigned_t>>, numeric_traits::max_value<numeric_traits::max_unsigned_t>));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::max_value<numeric_traits::max_signed_t>>, numeric_traits::max_value<numeric_traits::max_signed_t>));
static_assert(homogeneous_equals(bounded::normalize<numeric_traits::min_value<numeric_traits::max_signed_t>>, numeric_traits::min_value<numeric_traits::max_signed_t>));

} // namespace