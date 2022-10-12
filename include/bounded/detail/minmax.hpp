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
#include <bounded/normalize.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <operators/forward.hpp>
#include <bounded/concepts.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <functional>
#include <utility>

namespace bounded {

// Specialize this class to give the correct result for your own extreme
// function if the default does not work.
template<typename Compare, typename LHS, typename RHS>
struct extreme_value {
	static constexpr decltype(auto) operator()(Compare const compare, LHS && lhs, RHS && rhs) {
		using result_t = detail::add_common_cv_reference_t<std::common_type_t<LHS, RHS>, LHS, RHS>;
		return compare(rhs, lhs) ?
			static_cast<result_t>(OPERATORS_FORWARD(rhs)) :
			static_cast<result_t>(OPERATORS_FORWARD(lhs));
	}
};

// TODO: This should be selected only for less and greater
template<typename Compare, bounded_integer LHS, bounded_integer RHS>
struct extreme_value<Compare, LHS, RHS> {
	static constexpr decltype(auto) operator()(Compare const compare, LHS && lhs, RHS && rhs) {
		constexpr auto select = [](auto const lhs_, auto const rhs_) {
			if constexpr (Compare{}(rhs_, lhs_)) {
				return normalize<rhs_>;
			} else {
				return normalize<lhs_>;
			}
		};
		using lhs_t = std::remove_cvref_t<LHS>;
		using rhs_t = std::remove_cvref_t<RHS>;
		constexpr auto minimum = select(numeric_traits::min_value<lhs_t>, numeric_traits::min_value<rhs_t>);
		constexpr auto maximum = select(numeric_traits::max_value<lhs_t>, numeric_traits::max_value<rhs_t>);
		using type = integer<minimum, maximum>;
		using result_t = detail::add_common_cv_reference_t<type, LHS, RHS>;
		if constexpr (std::is_same_v<std::remove_cvref_t<result_t>, lhs_t> and std::is_same_v<lhs_t, rhs_t>) {
			return compare(rhs, lhs) ?
				OPERATORS_FORWARD(rhs) :
				OPERATORS_FORWARD(lhs);
		} else if constexpr (!constructible_from<result_t, RHS, unchecked_t>) {
			return lhs;
		} else if constexpr (!constructible_from<result_t, LHS, unchecked_t>) {
			return rhs;
		} else {
			return compare(rhs, lhs) ?
				::bounded::assume_in_range<result_t>(OPERATORS_FORWARD(rhs)) :
				::bounded::assume_in_range<result_t>(OPERATORS_FORWARD(lhs));
		}
	}
};


// Unlike the standard versions of min and max, which accept a fixed number of
// arguments, these functions accept a variadic pack. Therefore, there is no way
// to distinguish whether the user wanted to compare all of the object with
// `operator<` or a comparison function. The separately named function `extreme`
// serves that purpose.
//
// Because the variadic pack must be at the end of the parameter list, `extreme`
// accepts the comparison function as the first argument

struct extreme_function {
	static constexpr decltype(auto) operator()(auto /* compare */, auto && t) {
		return OPERATORS_FORWARD(t);
	}

	template<typename Compare, typename T1, typename T2>
	static constexpr decltype(auto) operator()(Compare compare, T1 && t1, T2 && t2) {
		return extreme_value<Compare, T1, T2>()(
			std::move(compare),
			OPERATORS_FORWARD(t1),
			OPERATORS_FORWARD(t2)
		);
	}

	static constexpr decltype(auto) operator()(auto compare, auto && t1, auto && t2, auto && ... ts) {
		return operator()(
			compare,
			operator()(compare, OPERATORS_FORWARD(t1), OPERATORS_FORWARD(t2)),
			OPERATORS_FORWARD(ts)...
		);
	}

} constexpr inline extreme;


inline constexpr auto min = [](auto && ... ts) -> decltype(auto) {
	return extreme(std::less(), OPERATORS_FORWARD(ts)...);
};

inline constexpr auto max = [](auto && ... ts) -> decltype(auto) {
	return extreme(std::greater(), OPERATORS_FORWARD(ts)...);
};

}	// namespace bounded
