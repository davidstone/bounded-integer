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
#include <bounded/detail/is_bounded_integer.hpp>
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

// TODO: This should only be selected for less and greater
template<
	typename Compare,
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy, storage_type lhs_storage, bool lhs_poisoned,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy, storage_type rhs_storage, bool rhs_poisoned
>
struct extreme_type<
	Compare,
	integer<lhs_min, lhs_max, lhs_policy, lhs_storage, lhs_poisoned>,
	integer<rhs_min, rhs_max, rhs_policy, rhs_storage, rhs_poisoned>
> {
private:
	static constexpr auto minimum = Compare{}(lhs_min, rhs_min) ? lhs_min : rhs_min;
	static constexpr auto maximum = Compare{}(lhs_max, rhs_max) ? lhs_max : rhs_max;
	using policy = detail::common_policy<lhs_policy, rhs_policy>;
	static constexpr auto storage = detail::common_storage_type(lhs_storage, rhs_storage);
	static constexpr auto poisoned = lhs_poisoned or rhs_poisoned;
public:
	using type = integer<minimum, maximum, policy, storage, poisoned>;
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
			static_cast<result_t>(std::forward<T2>(t2)) :
			static_cast<result_t>(std::forward<T1>(t1))
	)

	// These are needed because you cannot have a recursive noexcept specification
	// https://stackoverflow.com/questions/23772928/recursive-noexcept-specification

	template<typename Compare, typename T1, typename T2>
	static constexpr auto noexcept_extreme() noexcept {
		using result_t = result_type<Compare, T1, T2>;
		if constexpr (not std::is_constructible<result_t, T1>{} or not std::is_constructible<result_t, T2>{}) {
			return std::true_type{};
		} else {
			return std::bool_constant<noexcept(extreme_two(std::declval<Compare>(), std::declval<T2>(), std::declval<T1>()))>{};
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
		return std::forward<T>(t);
	}


	template<typename Compare, typename T1, typename T2>
	constexpr decltype(auto) operator()(Compare compare, T1 && t1, T2 && t2) const noexcept(noexcept_extreme<Compare, T1 &&, T2 &&>()) {
		using result_t = result_type<Compare, T1, T2>;
		if constexpr (not std::is_constructible<result_t, T2>{}) {
			static_cast<void>(compare);
			return std::forward<T1>(t1);
		} else if constexpr (not std::is_constructible<result_t, T1>{}) {
			static_cast<void>(compare);
			return std::forward<T2>(t2);
		} else {
			return extreme_two(std::move(compare), std::forward<T1>(t1), std::forward<T2>(t2));
		}
	}

	template<typename Compare, typename T1, typename T2, typename... Ts>
	constexpr decltype(auto) operator()(Compare compare, T1 && t1, T2 && t2, Ts && ... ts) const noexcept(noexcept_extreme<Compare, T1, T2, Ts...>()) {
		return operator()(
			compare,
			operator()(compare, std::forward<T1>(t1), std::forward<T2>(t2)),
			std::forward<Ts>(ts)...
		);
	}

} extreme;



constexpr struct {
	template<typename... Ts>
	constexpr auto operator()(Ts && ... ts) const BOUNDED_NOEXCEPT_DECLTYPE(
		extreme(less(), std::forward<Ts>(ts)...)
	)
} min;

constexpr struct {
	template<typename... Ts>
	constexpr auto operator()(Ts && ... ts) const BOUNDED_NOEXCEPT_DECLTYPE(
		extreme(greater(), std::forward<Ts>(ts)...)
	)
} max;

}	// namespace bounded
