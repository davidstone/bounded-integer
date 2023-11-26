// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.minmax;

import bounded.add_common_cv_reference;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.comparison_function_object;
import bounded.concepts;
import bounded.declval;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.normalize;
import bounded.test_int;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

// Specialize this class to give the correct result for your own extreme
// function if the default does not work.
export template<typename Compare, typename LHS, typename RHS>
struct extreme_value {
	static constexpr decltype(auto) operator()(Compare const compare, LHS && lhs, RHS && rhs) {
		using result_t = add_common_cv_reference<std::common_type_t<LHS, RHS>, LHS, RHS>;
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
		using result_t = add_common_cv_reference<type, LHS, RHS>;
		if constexpr (std::same_as<std::remove_cvref_t<result_t>, lhs_t> and std::same_as<lhs_t, rhs_t>) {
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

};
export constexpr auto extreme = extreme_function();

export constexpr auto min = [](auto && ... ts) -> decltype(auto) {
	return extreme(std::less(), OPERATORS_FORWARD(ts)...);
};

export constexpr auto max = [](auto && ... ts) -> decltype(auto) {
	return extreme(std::greater(), OPERATORS_FORWARD(ts)...);
};

} // namespace bounded

namespace check_single_argument_minmax {
	constexpr auto value = bounded::constant<5>;
	static_assert(
		std::addressof(bounded::min(value)) == std::addressof(value),
		"A value does not have itself as the minimum."
	);
	static_assert(
		std::addressof(bounded::max(value)) == std::addressof(value),
		"A value does not have itself as the maximum."
	);
}

namespace check_double_argument_minmax {
	constexpr auto lower_value = bounded::constant<6>;
	constexpr auto greater_value = bounded::constant<10>;
	static_assert(homogeneous_equals(
		bounded::min(lower_value, greater_value),
		lower_value
	));
	static_assert(homogeneous_equals(
		bounded::min(greater_value, lower_value),
		lower_value
	));
	static_assert(homogeneous_equals(
		bounded::max(lower_value, greater_value),
		greater_value
	));
	static_assert(homogeneous_equals(
		bounded::max(greater_value, lower_value),
		greater_value
	));
}

namespace check_many_argument_minmax {
	constexpr auto value = bounded::integer<-53, 1000>(bounded::constant<3>);
	static_assert(homogeneous_equals(
		bounded::min(bounded::constant<0>, bounded::constant<10>, bounded::constant<5>, value),
		bounded::integer<-53, 0>(bounded::constant<0>)
	));

	static_assert(homogeneous_equals(
		bounded::max(bounded::constant<0>, bounded::constant<10>, bounded::constant<5>, value),
		bounded::integer<10, 1000>(bounded::constant<10>)
	));

	static_assert(homogeneous_equals(
		bounded::min(0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
		0
	));
}


namespace check_other_type_minmax {
	using bounded_test::integer;
	static_assert(bounded::min(integer(1), integer(2)) == integer(1));
	static_assert(bounded::min(integer(4), integer(3)) == integer(3));
	static_assert(bounded::min(integer(5), integer(5)) == integer(5));
}

template<typename T>
constexpr auto check_specific_reference_minmax() {
	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T &>(), bounded::declval<T &>())), T &>,
		"Incorrect result type for bounded::min with T & and T &."
	);

	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T const &>(), bounded::declval<T const &>())), T const &>,
		"Incorrect result type for bounded::min with T const & and T const &."
	);

	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T &>(), bounded::declval<T const &>())), T const &>,
		"Incorrect result type for bounded::min with T & and T const &."
	);
	
	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T &&>(), bounded::declval<T &&>())), T>,
		"Incorrect result type for bounded::min with T && and T &&."
	);
	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T &>(), bounded::declval<T &&>())), T>,
		"Incorrect result type for bounded::min with T & and T &&."
	);
	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<T const &>(), bounded::declval<T &&>())), T>,
		"Incorrect result type for bounded::min with T const & and T &&."
	);
}

constexpr auto check_minmax() {
	// Check that built-in and class types have the same behavior, unlike
	// operator?:
	check_specific_reference_minmax<int>();
	check_specific_reference_minmax<bounded_test::integer>();
	
	static_assert(
		std::same_as<decltype(bounded::min(bounded::declval<int const &>(), bounded::declval<long const &>())), long>,
		"Incorrect result type for bounded::min with int const & and long const &."
	);
	return true;
}
static_assert(check_minmax());
