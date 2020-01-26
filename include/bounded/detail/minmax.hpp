// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/common_type_and_value_category.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_function_object.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/normalize.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <operators/forward.hpp>
#include <bounded/is_constructible.hpp>

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
template<typename Compare, bounded_integer LHS, bounded_integer RHS>
struct extreme_type<Compare, LHS, RHS> {
private:
	static constexpr auto select = [](auto const lhs, auto const rhs) {
		if constexpr (Compare{}(lhs, rhs)) {
			return detail::normalize<lhs.value()>;
		} else {
			return detail::normalize<rhs.value()>;
		}
	};
	static constexpr auto minimum = select(min_value<LHS>, min_value<RHS>);
	static constexpr auto maximum = select(max_value<LHS>, max_value<RHS>);
public:
	using type = integer<minimum, maximum>;
};


// Unlike the standard versions of min and max, which accept a fixed number of
// arguments, these functions accept a variadic pack. Therefore, there is no way
// to distinguish whether the user wanted to compare all of the object with
// `operator<` or a comparison function. The separately named function `extreme`
// serves that purpose.
//
// Because the variadic pack must be at the end of the parameter list, `extreme`
// accepts the comparison function as the first argument

constexpr inline struct extreme_function {
private:
	template<typename Compare, typename T1, typename T2>
	using result_type = detail::add_common_cv_reference_t<extreme_t<Compare, T2, T1>, T1, T2>;
	
	template<typename T1, typename T2>
	static constexpr decltype(auto) extreme_two(auto compare, T1 && t1, T2 && t2) {
		using result_t = result_type<decltype(compare), T1, T2>;
		return compare(t2, t1) ?
			static_cast<result_t>(OPERATORS_FORWARD(t2)) :
			static_cast<result_t>(OPERATORS_FORWARD(t1));
	}

public:
	constexpr decltype(auto) operator()(auto /* compare */, auto && t) const {
		return OPERATORS_FORWARD(t);
	}


	template<typename T1, typename T2>
	constexpr decltype(auto) operator()(auto compare, T1 && t1, T2 && t2) const {
		using result_t = result_type<decltype(compare), T1, T2>;
		if constexpr (not constructible_from<result_t, T2>) {
			return OPERATORS_FORWARD(t1);
		} else if constexpr (not constructible_from<result_t, T1>) {
			return OPERATORS_FORWARD(t2);
		} else {
			return extreme_two(std::move(compare), OPERATORS_FORWARD(t1), OPERATORS_FORWARD(t2));
		}
	}

	constexpr decltype(auto) operator()(auto compare, auto && t1, auto && t2, auto && ... ts) const {
		return operator()(
			compare,
			operator()(compare, OPERATORS_FORWARD(t1), OPERATORS_FORWARD(t2)),
			OPERATORS_FORWARD(ts)...
		);
	}

} extreme;


inline constexpr auto min = [](auto && ... ts) -> decltype(auto) {
	return extreme(less(), OPERATORS_FORWARD(ts)...);
};

inline constexpr auto max = [](auto && ... ts) -> decltype(auto) {
	return extreme(greater(), OPERATORS_FORWARD(ts)...);
};

}	// namespace bounded
