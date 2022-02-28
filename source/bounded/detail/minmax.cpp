// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/minmax.hpp>

#include <bounded/detail/class.hpp>
#include "../homogeneous_equals.hpp"
#include "../../test_int.hpp"

#include <memory>

namespace {

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
		std::is_same<decltype(bounded::min(std::declval<T &>(), std::declval<T &>())), T &>{},
		"Incorrect result type for bounded::min with T & and T &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(std::declval<T const &>(), std::declval<T const &>())), T const &>{},
		"Incorrect result type for bounded::min with T const & and T const &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(std::declval<T &>(), std::declval<T const &>())), T const &>{},
		"Incorrect result type for bounded::min with T & and T const &."
	);
	
	static_assert(
		std::is_same<decltype(bounded::min(std::declval<T &&>(), std::declval<T &&>())), T>{},
		"Incorrect result type for bounded::min with T && and T &&."
	);
	static_assert(
		std::is_same<decltype(bounded::min(std::declval<T &>(), std::declval<T &&>())), T>{},
		"Incorrect result type for bounded::min with T & and T &&."
	);
	static_assert(
		std::is_same<decltype(bounded::min(std::declval<T const &>(), std::declval<T &&>())), T>{},
		"Incorrect result type for bounded::min with T const & and T &&."
	);
}

constexpr auto check_minmax() {
	// Check that built-in and class types have the same behavior, unlike
	// operator?:
	check_specific_reference_minmax<int>();
	check_specific_reference_minmax<bounded_test::integer>();
	
	static_assert(
		std::is_same<decltype(bounded::min(std::declval<int const &>(), std::declval<long const &>())), long>{},
		"Incorrect result type for bounded::min with int const & and long const &."
	);
	return true;
}
static_assert(check_minmax());

}	// namespace
