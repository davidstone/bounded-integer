// bounded::integer tests
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "bounded_integer.hpp"
#include "optional.hpp"
#include <cassert>
#include <sstream>

namespace {

using namespace bounded::literal;

namespace check_is_overflow_policy {
	static_assert(
		bounded::is_overflow_policy<bounded::null_policy>::value,
		"null_policy not recognized as an overflow policy."
	);
	static_assert(
		bounded::is_overflow_policy<bounded::throw_policy>::value,
		"throw_policy not recognized as an overflow policy."
	);
	static_assert(
		bounded::is_overflow_policy<bounded::clamp_policy>::value,
		"clamp_policy not recognized as an overflow policy."
	);
	static_assert(
		!bounded::is_overflow_policy<int>::value,
		"int recognized as an overflow policy."
	);

	static_assert(
		std::is_constructible<bounded::null_policy, bounded::clamp_policy>::value,
		"Cannot construct one policy from another."
	);
}

namespace check_common_policy {
	using bounded::null_policy;
	using bounded::throw_policy;

	static_assert(
		std::is_same<bounded::common_policy_t<null_policy, null_policy>, null_policy>::value,
		"common_policy gives wrong type for all null_policy"
	);

	static_assert(
		std::is_same<bounded::common_policy_t<null_policy, throw_policy>, throw_policy>::value,
		"common_policy gives wrong type for one null_policy (first)"
	);

	static_assert(
		std::is_same<bounded::common_policy_t<throw_policy, null_policy>, throw_policy>::value,
		"common_policy gives wrong type for one null_policy (second)"
	);

	static_assert(
		std::is_same<bounded::common_policy_t<throw_policy, throw_policy>, throw_policy>::value,
		"common_policy gives wrong type for all throw_policy"
	);
}

namespace check_common_type {
	using type1 = bounded::integer<1, 5>;
	using type2 = bounded::integer<3, 10>;
	using common_type2 = std::common_type_t<type1, type2>;
	using expected_type2 = bounded::integer<1, 10>;
	static_assert(std::is_same<expected_type2, common_type2>::value, "common_type wrong for 2 values.");
	using type3 = bounded::integer<-5, -5>;
	using common_type3 = std::common_type_t<type1, type2, type3>;
	using expected_type3 = bounded::integer<-5, 10>;
	static_assert(std::is_same<expected_type3, common_type3>::value, "common_type wrong for 3 values.");
	using type4 = bounded::integer<0, 0>;
	using common_type4 = std::common_type_t<type1, type2, type3, type4>;
	using expected_type4 = bounded::integer<-5, 10>;
	static_assert(std::is_same<expected_type4, common_type4>::value, "common_type wrong for 4 values.");


	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int, int>, int>::value,
		"Incorrect type for int, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int const, int>, int const>::value,
		"Incorrect type for int const, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int, int volatile>, int volatile>::value,
		"Incorrect type for int, int volatile."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int const volatile, int>, int const volatile>::value,
		"Incorrect type for int const volatile, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int volatile, int const volatile>, int const volatile>::value,
		"Incorrect type for int volatile, int const volatile."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int volatile, int const>, int const volatile>::value,
		"Incorrect type for int volatile, int const."
	);

	static_assert(
		std::is_same<bounded::detail::common_cv_type_t<int const volatile, int const volatile>, int const volatile>::value,
		"Incorrect type for int const volatile, int const volatile."
	);





	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int>, int>::value,
		"Incorrect type for int."
	);



	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int, int>, int>::value,
		"Incorrect type for int, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, int>, int>::value,
		"Incorrect type for int &, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int>, int>::value,
		"Incorrect type for int const &, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int>, int>::value,
		"Incorrect type for int &&, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int const &>, int const &>::value,
		"Incorrect type for int const &, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, int const &>, int const &>::value,
		"Incorrect type for int &, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int const &>, int>::value,
		"Incorrect type for int &&, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, int &>, int &>::value,
		"Incorrect type for int &, int &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &>, int>::value,
		"Incorrect type for int &&, int &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &&>, int &&>::value,
		"Incorrect type for int &&, int &&."
	);



	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int, long>, long>::value,
		"Incorrect type for int, long."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, long>, long>::value,
		"Incorrect type for int &, long."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long>, long>::value,
		"Incorrect type for int const &, long."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long>, long>::value,
		"Incorrect type for int &&, long."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long const &>, long>::value,
		"Incorrect type for int const &, long const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, long const &>, long>::value,
		"Incorrect type for int &, long const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long const &>, long>::value,
		"Incorrect type for int &&, long const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &, long &>, long>::value,
		"Incorrect type for int &, long &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &>, long>::value,
		"Incorrect type for int &&, long &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &&>, long>::value,
		"Incorrect type for int &&, long &&."
	);



	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &, int>, long>::value,
		"Incorrect type for long &, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int>, long>::value,
		"Incorrect type for long const &, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int>, long>::value,
		"Incorrect type for long &&, int."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int const &>, long>::value,
		"Incorrect type for long const &, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &, int const &>, long>::value,
		"Incorrect type for long &, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int const &>, long>::value,
		"Incorrect type for long &&, int const &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &, int &>, long>::value,
		"Incorrect type for long &, int &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &>, long>::value,
		"Incorrect type for long &&, int &."
	);

	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &&>, long>::value,
		"Incorrect type for long &&, int &&."
	);



	static_assert(
		std::is_same<bounded::detail::common_type_and_value_category_t<int, int, int>, int>::value,
		"Incorrect type for int, int, int."
	);
}



template<typename integer>
auto check_numeric_limits() {
	using int_limits = std::numeric_limits<integer>;
	using bounded_t = bounded::checked_integer<int_limits::min(), int_limits::max()>;
	static_assert(std::is_same<typename bounded_t::underlying_type, integer>::value, "Incorrect underlying_type.");
	using bounded_limits = std::numeric_limits<bounded_t>;
	static_assert(sizeof(bounded_t) == sizeof(integer), "checked_integer wrong size.");

	// I have to use the preprocessor here to create a string literal
	#define BOUNDED_INTEGER_CHECK_CONDITION(condition) \
		static_assert(int_limits::condition == bounded_limits::condition, #condition " is wrong.")
	BOUNDED_INTEGER_CHECK_CONDITION(is_specialized);
	BOUNDED_INTEGER_CHECK_CONDITION(is_integer);
	BOUNDED_INTEGER_CHECK_CONDITION(is_exact);
	BOUNDED_INTEGER_CHECK_CONDITION(has_infinity);
	BOUNDED_INTEGER_CHECK_CONDITION(has_quiet_NaN);
	BOUNDED_INTEGER_CHECK_CONDITION(has_signaling_NaN);
	BOUNDED_INTEGER_CHECK_CONDITION(has_denorm);
	BOUNDED_INTEGER_CHECK_CONDITION(has_denorm_loss);
	BOUNDED_INTEGER_CHECK_CONDITION(is_iec559);
	BOUNDED_INTEGER_CHECK_CONDITION(is_bounded);
	// is_modulo intentionally left out because bounded::integer may differ from
	// the behavior of built-ins. Instead, just instantiate it with a
	// do-nothing test to verify that it compiles.
	static_cast<void>(bounded_limits::is_modulo);
	// BOUNDED_INTEGER_CHECK_CONDITION(is_modulo);
	BOUNDED_INTEGER_CHECK_CONDITION(radix);
	BOUNDED_INTEGER_CHECK_CONDITION(digits);
	BOUNDED_INTEGER_CHECK_CONDITION(digits10);
	BOUNDED_INTEGER_CHECK_CONDITION(max_digits10);
	BOUNDED_INTEGER_CHECK_CONDITION(min_exponent);
	BOUNDED_INTEGER_CHECK_CONDITION(min_exponent10);
	BOUNDED_INTEGER_CHECK_CONDITION(max_exponent);
	BOUNDED_INTEGER_CHECK_CONDITION(max_exponent10);
	BOUNDED_INTEGER_CHECK_CONDITION(traps);
	BOUNDED_INTEGER_CHECK_CONDITION(tinyness_before);
	#undef BOUNDED_INTEGER_CHECK_CONDITION

	#define BOUNDED_INTEGER_CHECK_FUNCTION(function) \
		static_assert(int_limits::function() == bounded_limits::function().value(), #function "() is wrong.")
	// Some of the functions are meaningless for integers, so I do not compare
	#define BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(function) \
		static_cast<void>(bounded_limits::function())
	BOUNDED_INTEGER_CHECK_FUNCTION(min);
	BOUNDED_INTEGER_CHECK_FUNCTION(lowest);
	BOUNDED_INTEGER_CHECK_FUNCTION(max);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(epsilon);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(round_error);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(infinity);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(quiet_NaN);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(signaling_NaN);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(denorm_min);
	#undef BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION
	#undef BOUNDED_INTEGER_CHECK_FUNCTION
}
auto check_numeric_limits_all() {
	static_assert(std::numeric_limits<bounded::integer<1, 1000>>::digits == 0, "Meaningless digits not 0.");

	check_numeric_limits<int8_t>();
	check_numeric_limits<uint8_t>();
	check_numeric_limits<int16_t>();
	check_numeric_limits<uint16_t>();
	check_numeric_limits<int32_t>();
	check_numeric_limits<uint32_t>();
	check_numeric_limits<int64_t>();
	// Currently does not support unsigned types equal to uintmax_t
	// check_numeric_limits<uint64_t>();
}

namespace check_overlapping_range {
	using type = bounded::integer<0, 0>;
	static_assert(bounded::detail::type_overlaps_range<type>(0, 0), "Type should overlap its own range.");
	static_assert(bounded::detail::type_fits_in_range<type>(0, 0), "Type should fit in its own range.");
	static_assert(!bounded::detail::type_overlaps_range<type>(1, 1), "Type should not overlap a disjoint range.");
}

namespace check_class {
	constexpr auto min = std::numeric_limits<int>::min();
	constexpr auto max = std::numeric_limits<int>::max();
	using type = bounded::integer<min, max>;

	static_assert(
		bounded::detail::type_overlaps_range<std::decay_t<type>>(min, max),
		"Bounds of type do not overlap its own range."
	);

	static_assert(
		bounded::detail::is_explicitly_constructible_from<bounded::null_policy, type>(min, max),
		"Type is not explicitly constructible from itself."
	);

	static_assert(
		std::is_convertible<int, type>::value,
		"Cannot convert integer type to bounded::integer with same range."
	);
	static_assert(
		std::is_constructible<type, type, bounded::non_check_t>::value,
		"Cannot construct a type from itself with non_check_t."
	);

	constexpr bounded::integer<min, max, bounded::null_policy, bounded::storage_type::least> least(0);
}

namespace check_make {
	static_assert(
		std::is_same<
			bounded::detail::equivalent_overflow_policy<int>,
			bounded::null_policy
		>::value,
		"int should have a null_policy"
	);
	static_assert(
		std::is_same<
			bounded::detail::equivalent_overflow_policy<unsigned>,
			bounded::modulo_policy
		>::value,
		"unsigned should have a modulo_policy"
	);
	static_assert(
		std::is_same<
			bounded::detail::equivalent_overflow_policy<bounded::integer<0, 0, bounded::throw_policy>>,
			bounded::throw_policy
		>::value,
		"incorrect equivalent_overflow_policy for bounded::integer."
	);
}

namespace check_comparison {
	constexpr bounded::integer<1, 10> a(5);
	static_assert(
		a == a,
		"Values do not equal themselves"
	);
	static_assert(
		a == 5,
		"Values do not equal their underlying value"
	);
	constexpr bounded::integer<4, 36346, bounded::throw_policy> b(5);
	static_assert(
		a == b,
		"Values do not equal equivalent other bounded::integer types"
	);

	static_assert(
		bounded::make<5>() != bounded::make<6>(),
		"5 should not equal 6"
	);

	constexpr auto one = bounded::make<1>();
	static_assert(
		!std::numeric_limits<decltype(one)>::is_signed,
		"Value should be unsigned for this test."
	);
	constexpr auto negative_one = bounded::make<-1>();
	static_assert(
		std::numeric_limits<decltype(negative_one)>::is_signed,
		"Value should be signed for this test."
	);
	static_assert(
		negative_one < one,
		"Small negative values should be less than small positive values."
	);
	constexpr intmax_t int_min = std::numeric_limits<int>::min();
	constexpr intmax_t int_max = std::numeric_limits<int>::max();
	static_assert(
		bounded::make<int_min>() < bounded::make<int_max + 1>(),
		"Large negative values should be less than large positive values."
	);

	// I have to use the preprocessor here to create a string literal
	#define BOUNDED_INTEGER_SINGLE_COMPARISON(op, a, b) \
		static_assert( \
			((a) op (b)), \
			"Incorrect result for (" #a ") " #op " ( "#b ")" \
		)

	#define BOUNDED_INTEGER_MULTI_COMPARISON(op, a, b, c) \
		BOUNDED_INTEGER_SINGLE_COMPARISON(op, a, b); \
		BOUNDED_INTEGER_SINGLE_COMPARISON(op, b, c); \
		BOUNDED_INTEGER_SINGLE_COMPARISON(op, a, c); \
		static_assert( \
			!((c) op (a)), \
			"Incorrect result for !((" #c ") " #op " (" #a "))" \
		)
	
	#define BOUNDED_INTEGER_COMPARISON(op, a, b, c) \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, b, c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::make<a>(), b, c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, bounded::make<b>(), c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, b, bounded::make<c>()); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::make<a>(), bounded::make<b>(), c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::make<a>(), b, bounded::make<c>()); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, bounded::make<b>(), bounded::make<c>()); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::make<a>(), bounded::make<b>(), bounded::make<c>())

	BOUNDED_INTEGER_COMPARISON(<=, -4, -4, 16);
	BOUNDED_INTEGER_COMPARISON(<, -17, 0, 17);
	BOUNDED_INTEGER_COMPARISON(>=, 876, 876, 367);
	BOUNDED_INTEGER_COMPARISON(>, 1LL << 50LL, 1LL << 30LL, 7);

	#undef BOUNDED_INTEGER_COMPARISON
	#undef BOUNDED_INTEGER_MULTI_COMPARISON
	#undef BOUNDED_INTEGER_SINGLE_COMPARISON

}

namespace check_single_argument_minmax {
	constexpr auto value = bounded::make<5>();
	static_assert(bounded::min(value) == value, "A value does not have itself as the minimum.");
	static_assert(bounded::max(value) == value, "A value does not have itself as the maximum.");
}

namespace check_double_argument_minmax {
	constexpr auto lower_value = bounded::make<6>();
	constexpr auto greater_value = bounded::make<10>();
	static_assert(bounded::min(lower_value, greater_value) == lower_value, "Two argument min value incorrect.");
	static_assert(bounded::min(greater_value, lower_value) == lower_value, "Two argument min value incorrect.");
	static_assert(bounded::max(lower_value, greater_value) == greater_value, "Two argument max value incorrect.");
	static_assert(bounded::max(greater_value, lower_value) == greater_value, "Two argument max value incorrect.");
}

namespace check_many_argument_minmax {
	constexpr bounded::integer<-53, 1000> value(bounded::make<3>());
	constexpr auto minimum = bounded::min(bounded::make<0>(), bounded::make<10>(), bounded::make<5>(), value);
	using min_type = decltype(minimum);
	static_assert(minimum == bounded::make<0>(), "Incorrect minimum value.");
	static_assert(std::numeric_limits<min_type>::min() == -53, "Incorrect minimum minimum.");
	static_assert(std::numeric_limits<min_type>::max() == 0, "Incorrect maximum minimum.");

	constexpr auto maximum = bounded::max(bounded::make<0>(), bounded::make<10>(), bounded::make<5>(), value);
	using max_type = decltype(maximum);
	static_assert(maximum == bounded::make<10>(), "Incorrect maximum value.");
	static_assert(std::numeric_limits<max_type>::min() == 10, "Incorrect minimum maximum.");
	static_assert(std::numeric_limits<max_type>::max() == 1000, "Incorrect maximum maximum.");
}

namespace check_non_bounded_minmax {
	constexpr auto integer_min = bounded::min(0, 5);
	static_assert(std::is_same<decltype(integer_min), int const>::value, "Incorrect type of min for int arguments.");
	static_assert(integer_min == 0, "Incorrect value of min for int arguments.");
}

namespace check_non_integer_minmax {
	constexpr auto string_less(char const * lhs, char const * rhs) noexcept -> bool {
		return
			*lhs < *rhs ? true :
			*rhs < *lhs ? false :
			*lhs == '\0' ? false :
			string_less(lhs + 1, rhs + 1);
	}
	static_assert(string_less("0", "1"), "Incorrect string comparison function.");
	static_assert(string_less("00", "1"), "Incorrect string comparison function.");
	static_assert(string_less("0", "11"), "Incorrect string comparison function.");
	static_assert(string_less("09", "1"), "Incorrect string comparison function.");
	static_assert(string_less("09", "10"), "Incorrect string comparison function.");
	static_assert(string_less("1", "10"), "Incorrect string comparison function.");
	static_assert(!string_less("10", "1"), "Incorrect string comparison function.");
	static_assert(!string_less("1", "1"), "Incorrect string comparison function.");

	struct string_view {
		explicit constexpr string_view(char const * value) noexcept:
			m_value(value) {
		}
		constexpr auto operator<(string_view const other) const noexcept {
			return string_less(m_value, other.m_value);
		}
		constexpr auto operator==(string_view const other) const noexcept {
			// Don't care about efficiency here because it is just for tests
			return !string_less(m_value, other.m_value) and !string_less(other.m_value, m_value);
		}
	private:
		char const * m_value;
	};
	
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
auto check_specific_reference_minmax() {
	T m1{};
	T m2{};
	T const c1{};
	T const c2{};

	static_assert(
		std::is_same<decltype(bounded::min(m1, m2)), T &>::value,
		"Incorrect result type for bounded::min with T & and T &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(c1, c2)), T const &>::value,
		"Incorrect result type for bounded::min with T const & and T const &."
	);

	static_assert(
		std::is_same<decltype(bounded::min(m1, c1)), T const &>::value,
		"Incorrect result type for bounded::min with T & and T const &."
	);
	
	static_assert(
		std::is_same<decltype(bounded::min(T{}, T{})), T>::value,
		"Incorrect result type for bounded::min with T and T."
	);
	static_assert(
		std::is_same<decltype(bounded::min(m1, T{})), T>::value,
		"Incorrect result type for bounded::min with T & and T."
	);
	static_assert(
		std::is_same<decltype(bounded::min(c1, T{})), T>::value,
		"Incorrect result type for bounded::min with T const & and T."
	);
}

auto check_reference_minmax() {
	// Check that built-in and class types have the same behavior, unlike
	// operator?:
	check_specific_reference_minmax<int>();
	struct class_type {
		constexpr auto operator<(class_type) const noexcept -> bool {
			return true;
		}
	};
	check_specific_reference_minmax<class_type>();
	
	int const i{};
	long const l{};
	static_assert(
		std::is_same<decltype(bounded::min(i, l)), long>::value,
		"Incorrect result type for bounded::min with int const & and long const &."
	);
}

auto check_minmax() {
	check_reference_minmax();
}


namespace check_arithmetic {
	constexpr bounded::integer<1, 10, bounded::throw_policy> const x(9);
	static_assert(
		sizeof(x) == 1,
		"bounded::integer too big!"
	);
	constexpr bounded::integer<-3, 11, bounded::throw_policy> const y(x);
	constexpr bounded::integer<-3, 11, bounded::throw_policy> const z(4);
	static_assert(
		std::numeric_limits<decltype(z)>::is_signed,
		"bounded::integer with negative value in range should be signed."
	);

	constexpr auto sum = x + z;
	static_assert(
		std::numeric_limits<decltype(sum)>::min() == -2,
		"Minimum sum incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(sum)>::max() == 21,
		"Maximum sum incorrect."
	);
	static_assert(
		sum == 13,
		"Calculated sum incorrect."
	);

	constexpr auto integral_constant_sum = x + std::integral_constant<int, 5>{};
	static_assert(
		std::numeric_limits<decltype(integral_constant_sum)>::min() == 6,
		"Minimum sum incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(integral_constant_sum)>::max() == 15,
		"Maximum sum incorrect."
	);
	static_assert(
		integral_constant_sum == 14,
		"Calculated sum incorrect."
	);

	constexpr auto difference = x - z;
	static_assert(
		std::numeric_limits<decltype(difference)>::min() == -10,
		"Minimum difference incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(difference)>::max() == 13,
		"Maximum difference incorrect."
	);
	static_assert(
		difference == 5,
		"Calculated difference incorrect."
	);

	constexpr auto product = x * z;
	static_assert(
		std::numeric_limits<decltype(product)>::min() == -30,
		"Minimum product incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(product)>::max() == 110,
		"Maximum product incorrect."
	);
	static_assert(
		product == 36,
		"Calculated product incorrect."
	);

	constexpr auto quotient = x / z;
	static_assert(
		std::numeric_limits<decltype(quotient)>::min() == -10,
		"Minimum quotient incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(quotient)>::max() == 10,
		"Maximum quotient incorrect."
	);
	static_assert(
		quotient == 2,
		"Calculated quotient incorrect."
	);

	constexpr auto negation = -x;
	static_assert(
		std::numeric_limits<decltype(negation)>::min() == -10,
		"Minimum quotient incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(negation)>::max() == -1,
		"Maximum quotient incorrect."
	);
	static_assert(
		negation == -9,
		"Calculated negation incorrect."
	);

	static_assert(
		quotient < product,
		"quotient should be less than product."
	);
	static_assert(
		difference + 8 == sum,
		"difference + 8 should equal sum."
	);

	constexpr auto positive = +x;
	static_assert(
		positive == x,
		"Unary plus not a no-op."
	);

	constexpr bounded::integer<0, 2, bounded::throw_policy> left_shift_lhs(1);
	constexpr bounded::integer<0, 60, bounded::throw_policy> left_shift_rhs(3);
	constexpr auto left_shift_result = left_shift_lhs << left_shift_rhs;
	static_assert(
		std::numeric_limits<decltype(left_shift_result)>::min() == 0,
		"Minimum left shift result incorrect."
	);
	static_assert(
		std::numeric_limits<decltype(left_shift_result)>::max() == (2LL << 60LL),
		"Maximum left shift result incorrect."
	);
	static_assert(
		left_shift_result == (1 << 3),
		"Incorrect left shift result."
	);


	// modulo

	constexpr auto ten = bounded::make<10>();
	constexpr auto eleven = bounded::make<11>();
	constexpr auto ten_result = ten % eleven;
	static_assert(
		ten_result == ten,
		"Incorrect modulo with divisor one greater"
	);
	static_assert(
		std::is_same<decltype(ten_result), decltype(ten)>::value,
		"Incorrect modulo type with divisor one greater"
	);

	constexpr auto nine = bounded::make<9>();
	constexpr auto one = bounded::make<1>();
	constexpr auto one_result = ten % nine;
	static_assert(
		one_result == one,
		"Incorrect modulo with divisor one less"
	);
	static_assert(
		std::is_same<decltype(one_result), decltype(one)>::value,
		"Incorrect modulo type with divisor one less"
	);

	constexpr auto nine_result = nine % eleven;
	static_assert(
		nine_result == nine,
		"Incorrect modulo with divisor two less"
	);
	static_assert(
		std::is_same<decltype(nine_result), decltype(nine)>::value,
		"Incorrect modulo type with divisor two less"
	);

	constexpr auto two = bounded::make<2>();
	constexpr auto two_result = eleven % nine;
	static_assert(
		two_result == two,
		"Incorrect modulo with divisor two greater"
	);
	static_assert(
		std::is_same<decltype(two_result), decltype(two)>::value,
		"Incorrect modulo type with divisor two greater"
	);


	constexpr bounded::integer<17, 23, bounded::throw_policy> positive_range(20);
	constexpr bounded::integer<-54, -6, bounded::throw_policy> negative_range(-33);
	constexpr auto positive_negative_result = positive_range % negative_range;
	constexpr bounded::integer<0, 23, bounded::throw_policy> positive_negative(20 % -33);
	static_assert(
		positive_negative_result == positive_negative,
		"Incorrect modulo with mixed signs"
	);
	static_assert(
		std::is_same<decltype(positive_negative_result), decltype(positive_negative)>::value,
		"Incorrect modulo type with mixed signs"
	);

	constexpr auto negative_positive_result = negative_range % positive_range;
	constexpr bounded::integer<-22, 0, bounded::throw_policy> negative_positive(-33 % 20);
	static_assert(
		negative_positive_result == negative_positive,
		"Incorrect modulo with mixed signs"
	);
	static_assert(
		std::is_same<decltype(negative_positive_result), decltype(negative_positive)>::value,
		"Incorrect modulo type with mixed signs"
	);

	constexpr auto result = bounded::integer<0, 10>(10) % bounded::make<6>();
	#if 0
	static_assert(
		static_cast<intmax_t>(std::numeric_limits<decltype(result)>::min()) == 0,
		"uh oh"
	);
	static_assert(
		static_cast<intmax_t>(std::numeric_limits<decltype(result)>::max()) == 10,
		"uh oh"
	);
	#endif
	static_assert(
		result == 4,
		"wrong answer"
	);

	constexpr auto zero = bounded::make<0>();
	constexpr auto zero_result = zero % bounded::make<1>();
	static_assert(
		zero_result == zero,
		"Incorrect modulo with zero for the dividend"
	);
	static_assert(
		std::is_same<decltype(zero_result), decltype(zero)>::value,
		"Incorrect modulo type with zero for the dividend"
	);
	// auto undefined = 1 % zero;




	using array_type = int[5];
	constexpr array_type array{ 0, 1, 2, 3, 4 };
	static_assert(
		*(array + 0) == 0,
		"Incorrect pointer arithmetic with int."
	);
	static_assert(
		array[0] == 0,
		"Incorrect array indexing with int."
	);
	static_assert(
		*(array + bounded::make<0>()) == 0,
		"Incorrect pointer arithmetic with bounded::integer."
	);

	// Oops, not possible to overload array index operator
	#if 0
	static_assert(
		array[bounded::make<0>()] == 0,
		"Incorrect array indexing with bounded::integer."
	);
	#endif
}

namespace check_literal {
	// I have to use the preprocessor here to create an integer literal
	#define BOUNDED_INTEGER_CHECK_LITERAL(x) \
		static_assert( \
			std::numeric_limits<decltype(x ## _bi)>::min() == std::numeric_limits<decltype(x ## _bi)>::max(), \
			"Literal does not have a min possible value equal to a max possible value." \
		); \
		static_assert( \
			std::numeric_limits<decltype(x ## _bi)>::min() == x ## _bi, \
			"Literal does not have a value equal to the range." \
		); \
		\
		static_assert( \
			x ## _bi == static_cast<decltype(x ## _bi)::underlying_type>(x), \
			"Inaccurate value of " #x " (cast x)" \
		); \
		static_assert( \
			static_cast<decltype(x)>(x ## _bi) == x, \
			"Inaccurate value of " #x " (cast value)" \
		);

	BOUNDED_INTEGER_CHECK_LITERAL(0)
	BOUNDED_INTEGER_CHECK_LITERAL(1)
	BOUNDED_INTEGER_CHECK_LITERAL(10)
	BOUNDED_INTEGER_CHECK_LITERAL(1000)
	BOUNDED_INTEGER_CHECK_LITERAL(4294967295)
	BOUNDED_INTEGER_CHECK_LITERAL(4294967296)
	BOUNDED_INTEGER_CHECK_LITERAL(9223372036854775807)
	BOUNDED_INTEGER_CHECK_LITERAL(-1)
	BOUNDED_INTEGER_CHECK_LITERAL(-0)
	#undef BOUNDED_INTEGER_CHECK_LITERAL
}


namespace check_conditional {
	constexpr auto value = BOUNDED_CONDITIONAL(true, 7_bi, 9_bi);
	static_assert(value == 7_bi, "Wrong conditional value.");
	static_assert(std::is_same<decltype(value), bounded::integer<7, 9> const>::value, "Wrong conditional type.");
}


namespace check_null_policy {
	constexpr bounded::null_policy policy;
	constexpr auto value1 = policy.assignment(5, 0_bi, 10_bi);
	// This should not compile
	// constexpr auto value2 = policy.assignment(15, 0_bi, 10_bi);
}

auto check_throw_policy() {
	using bounded::checked_integer;
	static_assert(
		!noexcept(std::declval<checked_integer<0, 0> &>() = std::declval<checked_integer<0, 1> &>()),
		"Shouldn't be noexcept."
	);
	static constexpr auto minimum = 0_bi;
	static constexpr auto maximum = 10_bi;
	bounded::throw_policy policy;
	try {
		policy.assignment(20, minimum, maximum);
		assert(false);
	}
	catch (...) {
	}
	try {
		policy.assignment(-6, minimum, maximum);
		assert(false);
	}
	catch (...) {
	}
}

namespace check_modulo_policy {

	constexpr bounded::modulo_policy policy;

	static_assert(
		policy.assignment(5_bi, 0_bi, 9_bi) == 5_bi,
		"Incorrect result for a minimum of zero and value in range."
	);

	static_assert(
		policy.assignment(5_bi, -5_bi, 9_bi) == 5_bi,
		"Incorrect result for a negative minimum and value in range."
	);
	static_assert(
		policy.assignment(-5_bi, -5_bi, 9_bi) == -5_bi,
		"Incorrect result for a negative minimum and value equal to the minimum."
	);

	static_assert(
		policy.assignment(17_bi, 0_bi, 9_bi) == 7_bi,
		"Incorrect result for a minimum of zero and value too high."
	);

	static_assert(
		policy.assignment(1_bi, 2_bi, 9_bi) == 9_bi,
		"Incorrect result for a positive minimum and a positive value too low."
	);

	static_assert(
		policy.assignment(-1_bi, 0_bi, 9_bi) == 9_bi,
		"Incorrect result for a minimum of zero and value too low."
	);

	static_assert(
		policy.assignment(12_bi, -5_bi, 9_bi) == -3_bi,
		"Incorrect result for a negative minimum and value too high."
	);

	static_assert(
		policy.assignment(-10_bi, -5_bi, 9_bi) == 5_bi,
		"Incorrect result for a negative minimum and value too low."
	);

	static_assert(
		policy.assignment(-1000_bi, 4_bi, 4_bi) == 4_bi,
		"Incorrect result for a single-value range and value too low."
	);
	static_assert(
		policy.assignment(4_bi, 4_bi, 4_bi) == 4_bi,
		"Incorrect result for a single-value range and value in range."
	);
	static_assert(
		policy.assignment(678412_bi, 4_bi, 4_bi) == 4_bi,
		"Incorrect result for a single-value range and value too high."
	);

	static_assert(
		policy.assignment(0_bi, 0_bi, 0_bi) == 0_bi,
		"Incorrect result for a single-value range that can only hold 0 and a value of 0."
	);
	
	constexpr bounded::wrapping_integer<0, 9> x(15_bi);
	constexpr bounded::wrapping_integer<0, 1> y(x);
	static_assert(
		x == 5_bi,
		"Incorrect value stored with modulo policy."
	);
	static_assert(
		y == 1_bi,
		"Incorrect value stored with modulo policy."
	);
}	// namespace

namespace check_clamp_policy {
	static constexpr auto minimum = 27_bi;
	static constexpr auto maximum = 567_bi;
	constexpr bounded::clamp_policy policy;
	static_assert(
		policy.assignment(20_bi, minimum, maximum) == minimum,
		"Failure to properly clamp lesser positive values."
	);
	static_assert(
		policy.assignment(-25_bi, minimum, maximum) == minimum,
		"Failure to properly clamp negative values to a positive value."
	);
	static_assert(
		policy.assignment(1000_bi, minimum, maximum) == maximum,
		"Failure to properly clamp greater positive values."
	);
	static_assert(
		policy.assignment(2000_bi, minimum, maximum) == maximum,
		"Fail to clamp above range with a strictly greater type."
	);

	using type = bounded::integer<-100, 100, bounded::clamp_policy>;
	constexpr auto initial = std::numeric_limits<type::underlying_type>::max() + 1_bi;
	constexpr type value(initial);
	static_assert(
		value == std::numeric_limits<type>::max(),
		"Fail to clamp value when the source type is larger than the destination type."
	);


	constexpr bounded::integer<minimum.value(), maximum.value(), bounded::clamp_policy> integer(1000_bi);
	static_assert(
		integer == maximum,
		"Fail to clamp when using a bounded."
	);
}

auto check_policies() {
	check_throw_policy();
}


auto check_assignment() {
	bounded::integer<0, 10> x(5);
	// The following cannot compile due to out of range check
	// x = 11_bi;
	x = bounded::integer<10, 11>(10);
	assert(x == 10_bi);
}

auto check_compound_arithmetic() {
	bounded::integer<0, 10> x(5);
	x += 5_bi;
	assert(x == 10);
	bounded::checked_integer<-10, 10> y(-5);
	y += 5;
	assert(y == 0);
	y += x;	
	assert(y == 10);
	bounded::checked_integer<-1000, 1000> z(0);
	z += y;
	assert(z == 10);
	z *= x - 5;
	assert(z == 50);
	z -= 1;
	assert(z == 49);
	z /= 7;
	assert(z == 7);
	z = 0;
	assert(z == 0);
	x %= 6_bi;
	assert(x == 4);
	
	// This code doesn't work with -Wconversion, but should still compile.
	// short s = 0;
	// s += 4_bi;
	// assert(s == 4);
	int i = 9;
	i -= bounded::make(68);
	assert(i == -59);
	long l = -7;
	l *= z + 1_bi;
	assert(l == -7);
	i /= y;
	assert(i == -5);
	i %= bounded::make(6);
	assert(i == -5);
	i %= 4_bi;
	assert(i == -1);

	assert(++x == 5);
	assert(x == 5);
	assert(z++ == 0);
	assert(z == 1);
	
	bounded::clamped_integer<0, 10> clamped(5_bi);
	clamped -= 20_bi;
	assert(clamped == 0_bi);
}

auto check_algorithm() {
	constexpr auto array = bounded::make_array(0_bi, 3_bi, 2_bi, 3_bi, 5_bi);
	assert(bounded::count(std::begin(array), std::end(array), 3_bi) == 2_bi);
	assert(bounded::count(std::begin(array), std::end(array), 2_bi) == 1_bi);
	assert(bounded::count(std::begin(array), std::end(array), 7_bi) == 0_bi);
	assert(bounded::count_if(std::begin(array), std::end(array), [](auto){ return true; }) == array.size());
}

template<typename Initial, intmax_t initial_value, typename Expected, intmax_t expected_value>
auto check_absolute_value() {
	constexpr auto value = Initial(initial_value);
	constexpr auto absolute = bounded::abs(value);
	constexpr auto expected_absolute = Expected(expected_value);
	static_assert(
		std::is_same<decltype(expected_absolute), decltype(absolute)>::value,
		"Absolute value returns the wrong type."
	);
	static_assert(
		absolute == expected_absolute,
		"Absolute value returns the wrong value."
	);
}

auto check_math() {
	using bounded::checked_integer;
	check_absolute_value<checked_integer<-10, 4>, -5, checked_integer<0, 10>, 5>();
	check_absolute_value<checked_integer<-10, -10>, -10, checked_integer<10, 10>, 10>();
	check_absolute_value<checked_integer<0, 0>, 0, checked_integer<0, 0>, 0>();
	check_absolute_value<checked_integer<-7, 450>, -1, checked_integer<0, 450>, 1>();
	check_absolute_value<checked_integer<-7, 450>, 1, checked_integer<0, 450>, 1>();
}

template<typename T>
auto check_uncompressed_optional() {
	using type = bounded::integer<std::numeric_limits<T>::min(), std::numeric_limits<T>::max()>;
	static_assert(
		sizeof(type) < sizeof(bounded::optional<type>),
		"Compressing an optional that should not be compressed."
	);
}
template<intmax_t minimum, intmax_t maximum>
auto check_compressed_optional() {
	using type = bounded::integer<minimum, maximum>;
	using compressed_type = bounded::optional<type>;
	static_assert(
		sizeof(type) == sizeof(compressed_type),
		"compressed_optional too big."
	);
	static_assert(
		sizeof(compressed_type) == sizeof(bounded::optional<type>),
		"Incorrect optional selection."
	);
}

auto check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<intmax_t>();
	
	using integer_type = bounded::checked_integer<1, 10>;
	constexpr bounded::optional<integer_type> uninitialized_optional;
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	constexpr bounded::optional<integer_type> constexpr_optional_integer(integer_type(5));
	static_assert(static_cast<bool>(constexpr_optional_integer), "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");

	bounded::optional<integer_type> optional_integer(integer_type(4));
	optional_integer = uninitialized_optional;
	assert(!optional_integer);

	optional_integer = integer_type(7);
	assert(optional_integer);

	static_assert(std::is_same<decltype(*optional_integer), integer_type &>::value, "Incorrect type of *optional.");
	*optional_integer = integer_type(1);
	assert(optional_integer);
	assert(*optional_integer == 1);
	
	optional_integer = bounded::none;
	assert(!optional_integer);
}

auto check_to_string() {
	auto const result = bounded::to_string(4_bi);
	static_assert(std::is_same<decltype(result), std::string const>::value, "Incorrect type of to_string.");
	assert(result == "4");
}

template<typename integer>
auto streaming_test(int const initial, int const final) {
	integer value(initial);
	std::stringstream in;
	in << value;
	assert(in.str() == std::to_string(initial));
	std::stringstream out;
	out << final;
	out >> value;
	assert(value == final);
}

auto check_streaming() {
	streaming_test<bounded::checked_integer<0, 100>>(7, 0);
	constexpr auto large_initial = std::numeric_limits<int>::max() / 3;
	constexpr auto large_final = -49;
	streaming_test<bounded::equivalent_type<int>>(large_initial, large_final);
}

auto check_dynamic_policy() {
	constexpr bounded::dynamic_policy<0, 10, bounded::clamp_policy> clamp;
	static_assert(clamp.assignment(3_bi, 2_bi, 5_bi) == 3_bi, "Incorrect dynamic policy result when the static range is entirely within range.");
	static_assert(clamp.assignment(11_bi, 0_bi, 20_bi) == 10_bi, "Incorrect dynamic clamp policy result when the dynamic range is the limiting factor.");

	constexpr auto value = 3_bi;
	constexpr auto min = 1_bi;
	constexpr auto max = 7_bi;
	constexpr auto static_min = 0;
	constexpr auto static_max = 10;
	using policy_type = bounded::dynamic_policy<static_min, static_max, bounded::throw_policy>;
	using type = bounded::integer<static_min, static_max, policy_type>;
	
	constexpr policy_type policy(min, max);
	static_assert(policy.min() == min, "Incorrect dynamic min on the policy with dynamic bounds.");
	static_assert(policy.max() == max, "Incorrect dynamic max on the policy with dynamic bounds.");

	constexpr type integer(value, policy);
	static_assert(integer == value, "Incorrect value with dynamic bounds.");
	static_assert(integer.overflow_policy().min() == min, "Incorrect dynamic min on the integer with dynamic bounds.");
	static_assert(integer.overflow_policy().max() == max, "Incorrect dynamic max on the integer with dynamic bounds.");

	try {
		policy.assignment(min - 1_bi, static_min, static_max);
		assert(false);
	}
	catch (std::range_error const &) {
	}
	try {
		policy.assignment(max + 1_bi, static_min, static_max);
		assert(false);
	}
	catch (std::range_error const &) {
	}

	type run(value, policy);
	try {
		static_assert(!std::is_nothrow_constructible<type, decltype(min - 1_bi)>::value, "Should not be noexcept.");
		run = min - 1_bi;
		assert(false);
	}
	catch (std::range_error const &) {
	}
	assert(run == value);
	try {
		static_assert(!std::is_nothrow_constructible<type, decltype(max + 1_bi)>::value, "Should not be noexcept.");
		run = max + 1_bi;
		assert(false);
	}
	catch (std::range_error const &) {
	}
	assert(run == value);
}

auto check_iterator() {
	constexpr bounded::array<char, 1> a = { {} };
	assert(bounded::next(a.begin()) == a.end());
	assert(bounded::prev(a.end()) == a.begin());
}

enum class bounded_enum{};

}	// namespace
namespace bounded {

template<>
struct basic_numeric_limits<bounded_enum> {
	static constexpr auto min() noexcept -> intmax_t {
		return 0;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return 0;
	}
	static constexpr bool is_specialized = true;
	static constexpr bool is_integer = false;
};
}

namespace {

namespace check_enum_construction {
	enum unscoped_enum {};
	constexpr bounded::integer<0, 10> x(unscoped_enum{});
	constexpr auto a = bounded::make(unscoped_enum{});
	
	enum class scoped_enum {};
	constexpr bounded::integer<0, 10> y(scoped_enum{});
	// constexpr auto b = bounded::make(scoped_enum{});
	
	constexpr bounded::integer<0, 10> z(bounded_enum{});
	constexpr auto c = bounded::make(bounded_enum{});
}

}	// namespace

auto main() -> int {
	check_numeric_limits_all();
	check_minmax();
	check_policies();
	check_assignment();
	check_compound_arithmetic();
	check_algorithm();
	check_math();
	check_optional();
	check_to_string();
	check_streaming();
	check_dynamic_policy();
	check_iterator();
}
