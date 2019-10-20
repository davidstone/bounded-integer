// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/normalize.hpp>
#include <bounded/detail/policy/null_policy.hpp>

#include <type_traits>

namespace std {

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<bounded::bounded_integer LHS, bounded::bounded_integer RHS> requires(std::is_same_v<LHS, std::decay_t<LHS>> and std::is_same_v<RHS, std::decay_t<RHS>>)
struct common_type<LHS, RHS> {
private:
	static inline constexpr auto minimum = bounded::detail::normalize<bounded::detail::safe_min(
		bounded::detail::builtin_min_value<LHS>,
		bounded::detail::builtin_min_value<RHS>
	)>;
	static inline constexpr auto maximum = bounded::detail::normalize<bounded::detail::safe_max(
		bounded::detail::builtin_max_value<LHS>,
		bounded::detail::builtin_max_value<RHS>
	)>;
public:
	using type = bounded::integer<minimum, maximum, bounded::null_policy>;
};


// We use common_type heavily in the creation of deduced arrays, so we need to
// add in some tricks to limit the maximum instantiation depth:

template<
	bounded::bounded_integer T0,
	typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6, typename T7, typename T8, typename T9, typename T10,
	typename T11, typename T12, typename T13, typename T14, typename T15,
	typename T16, typename T17, typename T18, typename T19, typename T20,
	typename T21, typename T22, typename T23, typename T24, typename T25,
	typename T26, typename T27, typename T28, typename T29, typename T30,
	typename T31, typename T32, typename T33, typename T34, typename T35,
	typename T36, typename T37, typename T38, typename T39, typename T40,
	typename T41, typename T42, typename T43, typename T44, typename T45,
	typename T46, typename T47, typename T48, typename T49, typename T50,
	typename... Ts
> requires(std::is_same_v<T0, std::decay_t<T0>>)
struct common_type<
	T0,
	T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
	T11, T12, T13, T14, T15, T16, T17, T18, T19, T20,
	T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
	T31, T32, T33, T34, T35, T36, T37, T38, T39, T40,
	T41, T42, T43, T44, T45, T46, T47, T48, T49, T50,
	Ts...
> {
	using type = common_type_t<
		common_type_t<
			T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
			T10, T11, T12, T13, T14, T15, T16, T17, T18, T19,
			T20, T21, T22, T23, T24, T25, T26, T27, T28, T29,
			T30, T31, T32, T33, T34, T35, T36, T37, T38, T39,
			T40, T41, T42, T43, T44, T45, T46, T47, T48, T49
		>, T50, Ts...
	>;
};

}	// namespace std
