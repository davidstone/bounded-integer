// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/common_type.hpp>
#include <bounded/detail/common_type_and_value_category.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_function_object.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/requires.hpp>

#include <utility>

namespace bounded {

// Specialize this class to give the correct type for your own extreme function
// if the default does not work. Types that cannot be used to construct this
// type are assumed to never be valid results, and are thus implicitly ignored
// as possible extreme candidates.
template<typename Compare, typename T1, typename T2>
struct extreme_type {
	using type = std::common_type_t<T1, T2>;
};

template<typename Compare, typename T1, typename T2>
using extreme_t = typename extreme_type<Compare, std::decay_t<T1>, std::decay_t<T2>>::type;

// TODO: This should be selected only for less and greater
template<
	typename Compare,
	auto lhs_min, auto lhs_max, typename lhs_policy, bool lhs_poisoned,
	auto rhs_min, auto rhs_max, typename rhs_policy, bool rhs_poisoned
>
struct extreme_type<
	Compare,
	integer<lhs_min, lhs_max, lhs_policy, lhs_poisoned>,
	integer<rhs_min, rhs_max, rhs_policy, rhs_poisoned>
> {
private:
	static constexpr auto select = [](auto const lhs, auto const rhs) noexcept {
		if constexpr (Compare{}(lhs, rhs)) {
			return detail::normalize<lhs.value()>;
		} else {
			return detail::normalize<rhs.value()>;
		}
	};
	static constexpr auto minimum = select(constant<lhs_min>, constant<rhs_min>);
	static constexpr auto maximum = select(constant<lhs_max>, constant<rhs_max>);
	using policy = detail::common_policy<lhs_policy, rhs_policy>;
	static constexpr auto poisoned = lhs_poisoned or rhs_poisoned;
public:
	using type = integer<minimum, maximum, policy, poisoned>;
};


// Unlike the standard versions of min and max, which accept a fixed number of
// arguments, these functions accept a variadic pack. Therefore, there is no way
// to distinguish whether the user wanted to compare all of the object with
// `operator<` or a comparison function. The separately named function `extreme`
// serves that purpose.
//
// Because the variadic pack must be at the end of the parameter list, `extreme`
// accepts the comparison function as the first argument

constexpr struct {
private:
	template<typename Compare, typename T1, typename T2>
	using result_type = detail::add_common_cv_reference_t<extreme_t<Compare, T2, T1>, T1, T2>;
	
	template<typename Compare, typename T1, typename T2, typename result_t = result_type<Compare, T1, T2>>
	static constexpr auto extreme_two(Compare compare, T1 && t1, T2 && t2) BOUNDED_NOEXCEPT_DECLTYPE(
		compare(t2, t1) ?
			static_cast<result_t>(BOUNDED_FORWARD(t2)) :
			static_cast<result_t>(BOUNDED_FORWARD(t1))
	)

	// These are needed because you cannot have a recursive noexcept specification
	// https://stackoverflow.com/questions/23772928/recursive-noexcept-specification

	template<typename Compare, typename T1, typename T2>
	static constexpr auto noexcept_extreme() noexcept {
		using result_t = result_type<Compare, T1, T2>;
		if constexpr (not std::is_constructible_v<result_t, T1> or not std::is_constructible_v<result_t, T2>) {
			return true;
		} else {
			return noexcept(extreme_two(std::declval<Compare>(), std::declval<T2>(), std::declval<T1>()));
		}
	}
	
	template<typename Compare, typename T1, typename T2, typename T3, typename... Ts>
	static constexpr auto noexcept_extreme() noexcept {
		return std::bool_constant<
			noexcept_extreme<Compare, T1, T2>() and
			noexcept_extreme<Compare, result_type<Compare, T1, T2>, T3, Ts...>()
		>{};
	}
	
public:
	template<typename Compare, typename T>
	constexpr decltype(auto) operator()(Compare, T && t) const noexcept {
		return BOUNDED_FORWARD(t);
	}


	template<typename Compare, typename T1, typename T2>
	constexpr decltype(auto) operator()(Compare compare [[maybe_unused]], T1 && t1, T2 && t2) const noexcept(noexcept_extreme<Compare, T1 &&, T2 &&>()) {
		using result_t = result_type<Compare, T1, T2>;
		if constexpr (not std::is_constructible_v<result_t, T2>) {
			return BOUNDED_FORWARD(t1);
		} else if constexpr (not std::is_constructible_v<result_t, T1>) {
			return BOUNDED_FORWARD(t2);
		} else {
			return extreme_two(std::move(compare), BOUNDED_FORWARD(t1), BOUNDED_FORWARD(t2));
		}
	}

	template<typename Compare, typename T1, typename T2, typename... Ts>
	constexpr decltype(auto) operator()(Compare compare, T1 && t1, T2 && t2, Ts && ... ts) const noexcept(noexcept_extreme<Compare, T1, T2, Ts...>()) {
		return operator()(
			compare,
			operator()(compare, BOUNDED_FORWARD(t1), BOUNDED_FORWARD(t2)),
			BOUNDED_FORWARD(ts)...
		);
	}

} extreme;



constexpr struct {
	template<typename... Ts>
	constexpr auto operator()(Ts && ... ts) const BOUNDED_NOEXCEPT_DECLTYPE(
		extreme(less(), BOUNDED_FORWARD(ts)...)
	)
} min;

constexpr struct {
	template<typename... Ts>
	constexpr auto operator()(Ts && ... ts) const BOUNDED_NOEXCEPT_DECLTYPE(
		extreme(greater(), BOUNDED_FORWARD(ts)...)
	)
} max;

}	// namespace bounded
