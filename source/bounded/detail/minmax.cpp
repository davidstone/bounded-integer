// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#undef NDEBUG

#include <bounded/detail/minmax.hpp>

#include <bounded/detail/addressof.hpp>
#include <bounded/detail/class.hpp>
#include "../homogeneous_equals.hpp"
#include "../string_view.hpp"

namespace {

namespace check_single_argument_minmax {
	constexpr auto value = bounded::constant<5>;
	static_assert(
		bounded::addressof(bounded::min(value)) == bounded::addressof(value),
		"A value does not have itself as the minimum."
	);
	static_assert(
		bounded::addressof(bounded::max(value)) == bounded::addressof(value),
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
		bounded::integer<-53, 0>(0, bounded::non_check)
	));

	static_assert(homogeneous_equals(
		bounded::max(bounded::constant<0>, bounded::constant<10>, bounded::constant<5>, value),
		bounded::integer<10, 1000>(10, bounded::non_check)
	));

	static_assert(homogeneous_equals(
		bounded::min(0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
		0
	));
}


using bounded_test::string_view;
namespace check_non_integer_minmax {

	static_assert(string_view("0") < string_view("1"), "Incorrect string comparison function.");
	static_assert(string_view("00") < string_view("1"), "Incorrect string comparison function.");
	static_assert(string_view("0") < string_view("11"), "Incorrect string comparison function.");
	static_assert(string_view("09") < string_view("1"), "Incorrect string comparison function.");
	static_assert(string_view("09") < string_view("10"), "Incorrect string comparison function.");
	static_assert(string_view("1") < string_view("10"), "Incorrect string comparison function.");
	static_assert(string_view("10") > string_view("1"), "Incorrect string comparison function.");
	static_assert(string_view("1") == string_view("1"), "Incorrect string comparison function.");

	static_assert(bounded::min(string_view("0"), string_view("1")) == string_view("0"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("00"), string_view("1")) == string_view("00"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("0"), string_view("11")) == string_view("0"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("09"), string_view("1")) == string_view("09"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("09"), string_view("10")) == string_view("09"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("1"), string_view("10")) == string_view("1"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("10"), string_view("1")) == string_view("1"), "Incorrect value of min for string arguments.");
	static_assert(bounded::min(string_view("1"), string_view("1")) == string_view("1"), "Incorrect value of min for string arguments.");
}

template<typename T>
auto mref() -> T &;
template<typename T>
auto cref() -> T const &;
template<typename T>
auto rref() -> T &&;

template<typename T>
constexpr auto check_specific_reference_minmax() {
	static_assert(
		std::is_same<decltype(bounded::min(mref<T>(), mref<T>())), T &>{},
		"Incorrect result type for bounded::min with T & and T &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(cref<T>(), cref<T>())), T const &>{},
		"Incorrect result type for bounded::min with T const & and T const &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(mref<T>(), cref<T>())), T const &>{},
		"Incorrect result type for bounded::min with T & and T const &."
	);
	
	static_assert(
		std::is_same<decltype(bounded::min(rref<T>(), rref<T>())), T>{},
		"Incorrect result type for bounded::min with T and T."
	);
	static_assert(
		std::is_same<decltype(bounded::min(mref<T>(), rref<T>())), T>{},
		"Incorrect result type for bounded::min with T & and T."
	);
	static_assert(
		std::is_same<decltype(bounded::min(cref<T>(), rref<T>())), T>{},
		"Incorrect result type for bounded::min with T const & and T."
	);
}

constexpr auto check_minmax() {
	// Check that built-in and class types have the same behavior, unlike
	// operator?:
	check_specific_reference_minmax<int>();
	check_specific_reference_minmax<string_view>();
	
	static_assert(
		std::is_same<decltype(bounded::min(cref<int>(), cref<long>())), long>{},
		"Incorrect result type for bounded::min with int const & and long const &."
	);
}
static_assert((static_cast<void>(check_minmax()), true));

}	// namespace
