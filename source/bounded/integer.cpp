// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/integer.hpp>
#include <bounded/integer_range.hpp>
#include <bounded/optional.hpp>

#include <cassert>
#include <unordered_map>
#include <sstream>

namespace {

using namespace bounded::literal;

template<typename LHS, typename RHS>
constexpr auto same_bounds =
	std::numeric_limits<LHS>::min() == std::numeric_limits<RHS>::min() and
	std::numeric_limits<LHS>::max() == std::numeric_limits<RHS>::max();

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


template<typename Integer>
constexpr auto test_log(Integer const value, bounded::constant_t<2>) {
	switch (static_cast<uintmax_t>(value)) {
		case 1: return 0;
		case 2: return 1;
		case 3: return 1;
		case 8: return 3;
		case 9: return 3;
		case 10: return 3;
		case 11: return 3;
		case 255: return 7;
		case 998: return 9;
		case 999: return 9;
		case 1000: return 9;
		case 1022: return 9;
		case 1023: return 9;
		case 1024: return 10;
		case 1025: return 10;
		case static_cast<uint64_t>(std::numeric_limits<int64_t>::max()): return 62;
		case -static_cast<uint64_t>(std::numeric_limits<int64_t>::min()): return 63;
		// doesn't matter what we throw, compilation error
		default: throw 0;
	}
}

template<typename Integer>
constexpr auto test_log(Integer const value, bounded::constant_t<10>) {
	switch (static_cast<uintmax_t>(value)) {
		case 1: return 0;
		case 2: return 0;
		case 3: return 0;
		case 8: return 0;
		case 9: return 0;
		case 10: return 1;
		case 11: return 1;
		case 255: return 2;
		case 998: return 2;
		case 999: return 2;
		case 1000: return 3;
		case 1022: return 3;
		case 1023: return 3;
		case 1024: return 3;
		case 1025: return 3;
		case static_cast<uint64_t>(std::numeric_limits<int64_t>::max()): return 18;
		case -static_cast<uint64_t>(std::numeric_limits<int64_t>::min()): return 18;
		// doesn't matter what we throw, compilation error
		default: throw 0;
	}
}

#define BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(value, base) \
	static_assert(bounded::log(value, base) == test_log(value, base), "Incorrect log for " #value)

#define BOUNDED_INTEGER_LOG_TEST(value) \
	BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(bounded::constant<value>, bounded::constant<2>); \
	BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(bounded::constant<value>, bounded::constant<10>)

BOUNDED_INTEGER_LOG_TEST(1);
BOUNDED_INTEGER_LOG_TEST(2);
BOUNDED_INTEGER_LOG_TEST(3);
BOUNDED_INTEGER_LOG_TEST(8);
BOUNDED_INTEGER_LOG_TEST(9);
BOUNDED_INTEGER_LOG_TEST(10);
BOUNDED_INTEGER_LOG_TEST(11);
BOUNDED_INTEGER_LOG_TEST(255);
BOUNDED_INTEGER_LOG_TEST(998);
BOUNDED_INTEGER_LOG_TEST(999);
BOUNDED_INTEGER_LOG_TEST(1000);
BOUNDED_INTEGER_LOG_TEST(1022);
BOUNDED_INTEGER_LOG_TEST(1023);
BOUNDED_INTEGER_LOG_TEST(1024);
BOUNDED_INTEGER_LOG_TEST(1025);
BOUNDED_INTEGER_LOG_TEST(std::numeric_limits<int64_t>::max());

#undef BOUNDED_INTEGER_LOG_TEST
#undef BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL


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

template<bounded::storage_type storage_type>
auto check_constructibility_specific() {
	constexpr auto min = std::numeric_limits<int>::min();
	constexpr auto max = std::numeric_limits<int>::max();
	using type = bounded::integer<min, max, bounded::null_policy, storage_type>;
	static_assert(
		bounded::detail::type_overlaps_range<type>(min, max),
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

	static_assert(
		!std::is_convertible<bool, type>::value,
		"Should not be able to convert a bool to a bounded::integer."
	);
	static_assert(
		std::is_constructible<type, bool>::value,
		"Should be able to construct a bounded::integer from a bool."
	);
}

auto check_constructibility() {
	check_constructibility_specific<bounded::storage_type::fast>();
	check_constructibility_specific<bounded::storage_type::least>();
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
			bounded::detail::equivalent_overflow_policy<bounded::checked_integer<0, 0>>,
			bounded::throw_policy<>
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
	constexpr bounded::checked_integer<4, 36346> b(5);
	static_assert(
		a == b,
		"Values do not equal equivalent other bounded::integer types"
	);

	static_assert(
		bounded::constant<5> != bounded::constant<6>,
		"5 should not equal 6"
	);

	constexpr auto one = bounded::constant<1>;
	static_assert(
		!std::numeric_limits<decltype(one)>::is_signed,
		"Value should be unsigned for this test."
	);
	constexpr auto negative_one = bounded::constant<-1>;
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
		bounded::constant<int_min> < bounded::constant<int_max + 1>,
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
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::constant<a>, b, c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, bounded::constant<b>, c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, b, bounded::constant<c>); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::constant<a>, bounded::constant<b>, c); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::constant<a>, b, bounded::constant<c>); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, a, bounded::constant<b>, bounded::constant<c>); \
		BOUNDED_INTEGER_MULTI_COMPARISON(op, bounded::constant<a>, bounded::constant<b>, bounded::constant<c>)

	BOUNDED_INTEGER_COMPARISON(<=, -4, -4, 16);
	BOUNDED_INTEGER_COMPARISON(<, -17, 0, 17);
	BOUNDED_INTEGER_COMPARISON(>=, 876, 876, 367);
	BOUNDED_INTEGER_COMPARISON(>, 1LL << 50LL, 1LL << 30LL, 7);

	#undef BOUNDED_INTEGER_COMPARISON
	#undef BOUNDED_INTEGER_MULTI_COMPARISON
	#undef BOUNDED_INTEGER_SINGLE_COMPARISON

	auto non_constexpr_five = bounded::constant<5>;
	auto non_constexpr_four = bounded::constant<4>;
	static_assert(non_constexpr_five == non_constexpr_five, "operator== not constexpr for non-constexpr arguments.");
	static_assert(non_constexpr_five != non_constexpr_four, "operator!= not constexpr for non-constexpr arguments.");
	static_assert(non_constexpr_four < non_constexpr_five, "operator< not constexpr for non-constexpr arguments.");
	static_assert(non_constexpr_four <= non_constexpr_four, "operator<= not constexpr for non-constexpr arguments.");
}


template<typename Result, typename Expected>
constexpr bool homogenous_equals(Result const & result, Expected const & expected) noexcept {
	static_assert(std::is_same<Result, Expected>{}, "Mismatched types.");
	return result == expected;
}


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
	static_assert(homogenous_equals(
		bounded::min(lower_value, greater_value),
		lower_value
	));
	static_assert(homogenous_equals(
		bounded::min(greater_value, lower_value),
		lower_value
	));
	static_assert(homogenous_equals(
		bounded::max(lower_value, greater_value),
		greater_value
	));
	static_assert(homogenous_equals(
		bounded::max(greater_value, lower_value),
		greater_value
	));
}

namespace check_many_argument_minmax {
	constexpr auto value = bounded::integer<-53, 1000>(bounded::constant<3>);
	static_assert(homogenous_equals(
		bounded::min(bounded::constant<0>, bounded::constant<10>, bounded::constant<5>, value),
		bounded::integer<-53, 0>(0)
	));

	static_assert(homogenous_equals(
		bounded::max(bounded::constant<0>, bounded::constant<10>, bounded::constant<5>, value),
		bounded::integer<10, 1000>(10)
	));

	static_assert(homogenous_equals(
		bounded::min(0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
		0
	));
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
	constexpr bounded::checked_integer<1, 10> const x(9);
	static_assert(
		sizeof(x) == 1,
		"bounded::integer too big!"
	);
	constexpr bounded::checked_integer<-3, 11> const z(4);
	static_assert(
		std::numeric_limits<decltype(z)>::is_signed,
		"bounded::integer with negative value in range should be signed."
	);


	// unary plus
	static_assert(homogenous_equals(
		+x,
		x
	));

	// unary minus
	static_assert(homogenous_equals(
		-x,
		bounded::checked_integer<-10, -1>(-9)
	));

	// plus
	static_assert(homogenous_equals(
		x + z,
		bounded::checked_integer<-2, 21>(13)
	));
	static_assert(homogenous_equals(
		x + std::integral_constant<int, 5>{},
		bounded::integer<6, 15>(14)
	));

	// minus
	static_assert(homogenous_equals(
		x - z,
		bounded::checked_integer<-10, 13>(5)
	));

	// multiplies
	static_assert(homogenous_equals(
		x * z,
		bounded::checked_integer<-30, 110>(36)
	));

	// divides
	static_assert(homogenous_equals(
		x / z,
		bounded::checked_integer<-10, 10>(2)
	));

	// constexpr auto fails_to_compile = bounded::constant<1> / bounded::constant<0>;


	// modulus
	static_assert(homogenous_equals(
		bounded::constant<10> % bounded::constant<11>,
		bounded::constant<10>
	));
	static_assert(homogenous_equals(
		bounded::constant<10> % bounded::constant<9>,
		bounded::constant<1>
	));
	static_assert(homogenous_equals(
		bounded::constant<9> % bounded::constant<11>,
		bounded::constant<9>
	));
	static_assert(homogenous_equals(
		bounded::constant<11> % bounded::constant<9>,
		bounded::constant<2>
	));
	static_assert(homogenous_equals(
		bounded::constant<13> % bounded::constant<6>,
		bounded::constant<1>
	));
	static_assert(homogenous_equals(
		bounded::integer<17, 23>(20) % bounded::integer<-54, -6>(-33),
		bounded::integer<0, 23>(20 % -33)
	));
	static_assert(homogenous_equals(
		bounded::integer<-54, -6>(-33) % bounded::integer<17, 23>(20),
		bounded::integer<-22, 0>(-33 % 20)
	));
	static_assert(homogenous_equals(
		bounded::integer<-22, 0>(-33 % 20) % bounded::integer<0, 23>(20 % -33),
		bounded::integer<-22, 0>(-13)
	));
	static_assert(homogenous_equals(
		bounded::integer<0, 10>(10) % bounded::constant<6>,
		bounded::integer<0, 5>(4)
	));
	static_assert(homogenous_equals(
		bounded::constant<0> % bounded::constant<1>,
		bounded::constant<0>
	));
	static_assert(homogenous_equals(
		bounded::integer<73, 76>(73) % bounded::integer<7, 8>(7),
		bounded::integer<1, 6>(73 % 7)
	));
	static_assert(homogenous_equals(
		bounded::integer<74, 75>(74) % bounded::integer<7, 8>(7),
		bounded::integer<2, 5>(74 % 7)
	));

	// auto undefined = 1 % zero;
	
	constexpr auto bounded_intmax_t = bounded::integer<std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()>(std::numeric_limits<intmax_t>::min());
	static_assert(homogenous_equals(
		bounded_intmax_t % bounded_intmax_t,
		bounded::integer<std::numeric_limits<intmax_t>::min() + 1, -(std::numeric_limits<intmax_t>::min() + 1)>(0)
	));


	// left shift
	static_assert(homogenous_equals(
		bounded::integer<0, 2>(1) << bounded::integer<0, 60>(3),
		bounded::integer<0, 2LL << 60LL>(1 << 3)
	));

	// right shift
	static_assert(homogenous_equals(
		bounded::constant<100> >> bounded::integer<0, 50>(1),
		bounded::integer<0, 100>(100 >> 1)
	));
	

	// bitwise and
	static_assert((bounded::constant<17> & 1) == bounded::constant<1>);
	static_assert((bounded::constant<18> & 1) == bounded::constant<0>);


	
	using array_type = int[5];
	constexpr array_type array{ 0, 1, 2, 3, 4 };
	
	static_assert(
		*(std::begin(array) + bounded::constant<0>) == 0,
		"Incorrect pointer arithmetic with bounded::integer."
	);
	static_assert(
		*(array + bounded::constant<0>) == 0,
		"Incorrect array arithmetic with bounded::integer."
	);
	static_assert(
		array + bounded::constant<5> == std::end(array),
		"Incorrect array arithmetic with bounded::integer."
	);



#if 0
	// TODO: Test that this does not compile, improve error message
	array + bounded::constant<6>
#endif

	// Oops, not possible to overload array index operator
	#if 0
	static_assert(
		array[bounded::constant<0>] == 0,
		"Incorrect array indexing with bounded::integer."
	);
	#endif
}


// I have to use the preprocessor here to create an integer literal
#define BOUNDED_INTEGER_CHECK_LITERAL(x) \
	static_assert(homogenous_equals( \
		x ## _bi, \
		bounded::constant<x> \
	))

BOUNDED_INTEGER_CHECK_LITERAL(0);
BOUNDED_INTEGER_CHECK_LITERAL(1);
BOUNDED_INTEGER_CHECK_LITERAL(10);
BOUNDED_INTEGER_CHECK_LITERAL(1000);
BOUNDED_INTEGER_CHECK_LITERAL(4294967295);
BOUNDED_INTEGER_CHECK_LITERAL(4294967296);
BOUNDED_INTEGER_CHECK_LITERAL(9223372036854775807);
#undef BOUNDED_INTEGER_CHECK_LITERAL


static_assert(homogenous_equals(
	BOUNDED_CONDITIONAL(true, 7_bi, 9_bi),
	bounded::integer<7, 9>(7)
));


auto check_null_policy() {
	constexpr bounded::null_policy policy;
	constexpr auto value1 = policy.assignment(5, 0_bi, 10_bi);
	static_cast<void>(value1);
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
	bounded::throw_policy<> policy;
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
	check_null_policy();
	check_throw_policy();
}


auto check_assignment() {
	bounded::integer<0, 10> x(5);
	// The check for assignability fails with a hard error here currently
	// static_assert(!std::is_assignable<decltype((x)), decltype(11_bi)>::value, "Should not be assignable.");
	x = bounded::integer<10, 11>(10);
	assert(x == 10_bi);

	bounded::clamped_integer<0, 10> y(5);
	y = 11_bi;
	assert(y == 10_bi);
}

template<typename LHS, typename RHS>
constexpr auto plus_equals(LHS lhs, RHS rhs) {
	return lhs += rhs;
}
template<typename LHS, typename RHS>
constexpr auto minus_equals(LHS lhs, RHS rhs) {
	return lhs -= rhs;
}
template<typename LHS, typename RHS>
constexpr auto times_equals(LHS lhs, RHS rhs) {
	return lhs *= rhs;
}
template<typename LHS, typename RHS>
constexpr auto divide_equals(LHS lhs, RHS rhs) {
	return lhs /= rhs;
}
template<typename LHS, typename RHS>
constexpr auto modulo_equals(LHS lhs, RHS rhs) {
	return lhs %= rhs;
}

auto check_compound_arithmetic() {
	using x_type = bounded::checked_integer<0, 10>;
	using y_type = bounded::checked_integer<-10, 10>;
	static_assert(plus_equals(x_type(5_bi), 5_bi) == 10_bi);
	static_assert(plus_equals(y_type(-5_bi), 5) == 0_bi);
	static_assert(plus_equals(y_type(0_bi), x_type(10_bi)) == 10_bi);
	
	using z_type = bounded::checked_integer<-1000, 1000>;
	static_assert(plus_equals(z_type(0_bi), x_type(10_bi)) == 10_bi);
	static_assert(times_equals(z_type(10_bi), x_type(10_bi) - 5) == 50_bi);
	static_assert(times_equals(z_type(10_bi), x_type(10_bi) - 5) == 50_bi);
	static_assert(minus_equals(z_type(50_bi), 1) == 49_bi);
	static_assert(divide_equals(z_type(49_bi), 7) == 7_bi);
	static_assert(modulo_equals(x_type(10_bi), 6_bi) == 4_bi);
	
	short s = 0;
	s += 4_bi;
	assert(s == 4);
	static_assert(minus_equals(9, 68_bi) == -59);
	// long l = -7;
	// l *= z + 1_bi;
	// assert(l == -7);
	static_assert(divide_equals(-59, y_type(10_bi)) == -5);
	static_assert(modulo_equals(-5, 4_bi) == -1);

	auto x = x_type(4_bi);
	auto const pre_increment = ++x;
	assert(pre_increment == 5_bi);
	assert(x == 5_bi);
	x = 0;
	assert(x == 0);
	auto z = z_type(0_bi);
	auto const post_increment = z++;
	assert(post_increment == 0);
	assert(z == 1);

	static_assert(minus_equals(bounded::clamped_integer<0, 10>(5_bi), 20_bi) == 0_bi);	
}

auto check_math() {
	static_assert(homogenous_equals(
		bounded::abs(bounded::integer<-10, 4>(-5)),
		bounded::integer<0, 10>(5)
	));
	static_assert(homogenous_equals(
		bounded::abs(bounded::integer<-10, -10>(-10)),
		bounded::integer<10, 10>(10)
	));
	static_assert(homogenous_equals(
		bounded::abs(bounded::integer<0, 0>(0)),
		bounded::integer<0, 0>(0)
	));
	static_assert(homogenous_equals(
		bounded::abs(bounded::integer<-7, 450>(-1)),
		bounded::integer<0, 450>(1)
	));
	static_assert(homogenous_equals(
		bounded::abs(bounded::integer<-7, 450>(1)),
		bounded::integer<0, 450>(1)
	));
}


// I would rather test the public interface of my class instead of the
// implementation detail functions that are used to generate the bounds.
// However, testing that way seems to require O(n^8) space in the program (and
// thus memory at compile time) and at least that much time at compile-time,
// followed by at least O(n^6) runtime, in addition to significantly more
// complicated code. By going through the bounded::detail::*operator_range
// functions, we can just do the O(n^6) runtime.
template<typename Operation, typename Range>
auto check_arithmetic_range(Operation const operation, Range const range) {
	constexpr auto division_based_function = std::is_same<Operation, std::divides<>>{} or std::is_same<Operation, std::modulus<>>{};
	constexpr auto bound = 7_bi;
	using bounded::integer_range;
	for (auto const rhs_min : integer_range(-bound, bound + 1_bi)) {
		if (division_based_function and rhs_min == 0_bi) {
			continue;
		}
		for (auto const rhs_max : integer_range(rhs_min, bound + 1_bi)) {
			if (division_based_function and rhs_max == 0_bi) {
				continue;
			}
			for (auto const lhs_min : integer_range(-bound, bound + 1_bi)) {
				for (auto const lhs_max : integer_range(lhs_min, bound + 1_bi)) {

					using bounded::detail::min_max;
					auto const expected = range(min_max(lhs_min.value(), lhs_max.value()), min_max(rhs_min.value(), rhs_max.value()));
					auto current_max = std::numeric_limits<intmax_t>::min();
					auto current_min = std::numeric_limits<intmax_t>::max();

					for (auto const rhs : integer_range(rhs_min, rhs_max + 1_bi)) {
						if (division_based_function and rhs == 0_bi) {
							continue;
						}
						for (auto const lhs : integer_range(lhs_min, lhs_max + 1_bi)) {
							auto const result = operation(lhs.value(), rhs.value());
							current_max = bounded::max(current_max, result);
							current_min = bounded::min(current_min, result);
						}
					}
					assert(current_min == expected.min);
					assert(current_max == expected.max);
				}
			}
		}
	}
}

auto check_arithmetic_range() {
	using namespace bounded::detail;
	check_arithmetic_range(std::plus<>{}, [](auto const & lhs, auto const & rhs) { return plus_operator_range(lhs, rhs); });
	check_arithmetic_range(std::multiplies<>{}, [](auto const & lhs, auto const & rhs) { return multiplies_operator_range(lhs, rhs); });
	check_arithmetic_range(std::divides<>{}, [](auto const & lhs, auto const & rhs) { return divides_operator_range(lhs, rhs); });
	check_arithmetic_range(std::modulus<>{}, [](auto const & lhs, auto const & rhs) { return modulus_operator_range(lhs, rhs); });
}


template<typename Optional>
auto check_empty_braces() {
	Optional op = {};
	assert(!op);
	op = {};
	assert(!op);
}

namespace check_optional_common_type {

	static_assert(
		std::is_same<std::common_type_t<bounded::none_t, int>, bounded::optional<int>>::value,
		"common_type with none_t first wrong."
	);
	static_assert(
		std::is_same<std::common_type_t<int, bounded::none_t>, bounded::optional<int>>::value,
		"common_type with none_t last wrong."
	);
	static_assert(
		std::is_same<
			std::common_type_t<decltype(1_bi), bounded::none_t, decltype(5_bi), bounded::none_t>,
			bounded::optional<bounded::integer<1, 5>>
		>::value,
		"common_type with bounded::integer and none_t wrong."
	);

}	// namespace check_optional_common_type

template<typename T>
auto check_uncompressed_optional() {
	using type = bounded::integer<std::numeric_limits<T>::min(), std::numeric_limits<T>::max()>;
	static_assert(
		sizeof(type) < sizeof(bounded::optional<type>),
		"Compressing an optional that should not be compressed."
	);
	check_empty_braces<type>();
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
	check_empty_braces<compressed_type>();
}

template<typename integer_type>
auto check_integer_optional() {
	constexpr bounded::optional<integer_type> uninitialized_optional;
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	static_assert(value_or(uninitialized_optional, integer_type(9)) == integer_type(9), "value_or incorrect for uninitialized");
	constexpr bounded::optional<integer_type> constexpr_optional_integer(integer_type(5));
	static_assert(constexpr_optional_integer, "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");
	static_assert(value_or(constexpr_optional_integer, integer_type(9)) == integer_type(5), "value_or incorrect for initialized");

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

	check_empty_braces<bounded::optional<integer_type>>();
}

auto check_non_trivial_optional() {
	using type = std::string;
	bounded::optional<type> uninitialized_optional;
	assert(!uninitialized_optional);
	decltype(auto) uninitialized_value_or = value_or(uninitialized_optional, "spork");
	assert(uninitialized_value_or == "spork");
	static_assert(std::is_same<decltype(uninitialized_value_or), std::string>::value, "value_or incorrect for uninitialized");
	static_assert(std::is_same<decltype(value_or(std::move(uninitialized_optional), std::string("spoon"))), std::string &&>::value, "value_or incorrect for uninitialized");
	bounded::optional<type> optional_string("Hello");
	assert(optional_string);
	auto const default_value = std::string("knife");
	decltype(auto) initialized_value_or = value_or(optional_string, default_value);
	assert(initialized_value_or == "Hello");
	static_assert(std::is_same<decltype(initialized_value_or), std::string const &>::value, "value_or incorrect for initialized");
	assert(*optional_string == "Hello");

	optional_string = uninitialized_optional;
	assert(!optional_string);

	optional_string = type("Hi");
	assert(optional_string);
	optional_string = "Yo";
	assert(optional_string);
	assert(optional_string == "Yo");
	assert(optional_string != "Sup");

	static_assert(std::is_same<decltype(*optional_string), type &>::value, "Incorrect type of *optional.");
	*optional_string = type("Hiya");
	assert(optional_string);
	assert(*optional_string == "Hiya");
	
	optional_string = bounded::none;
	assert(!optional_string);

	check_empty_braces<bounded::optional<type>>();
}

auto check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<intmax_t>();
	
	check_integer_optional<int>();
	check_integer_optional<bounded::checked_integer<1, 10>>();
	check_non_trivial_optional();

	constexpr auto original = bounded::make_optional(bounded::constant<0>);
	constexpr auto copy = original;
	static_assert(original == copy);
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
	static constexpr bool is_bounded = true;
};
}

namespace {

auto check_enum_construction() {
	enum unscoped_enum : int {};
	static_assert(std::is_constructible<bounded::integer<0, 10>, unscoped_enum>::value);
	static_assert(!std::is_convertible<unscoped_enum, bounded::integer<0, 10>>::value);
	constexpr auto a = bounded::make(unscoped_enum{});
	static_assert(std::is_same<
		std::remove_const_t<decltype(a)>,
		bounded::equivalent_type<std::underlying_type_t<unscoped_enum>>
	>::value);
	
	enum class scoped_enum {};
	static_assert(std::is_constructible<bounded::integer<0, 10>, scoped_enum>::value);
	static_assert(!std::is_convertible<scoped_enum, bounded::integer<0, 10>>::value);
	// constexpr auto b = bounded::make(scoped_enum{});
	
	static_assert(std::is_constructible<bounded::integer<0, 10>, bounded_enum>::value);
	// TODO: Should this be convertible?
	static_assert(std::is_convertible<bounded_enum, bounded::integer<0, 10>>::value);
	constexpr auto c = bounded::make(bounded_enum{});
	static_cast<void>(c);
}

// check poisoning
static_assert(std::is_convertible<int, bounded::integer<0, 10>>::value);
static_assert(std::is_convertible<decltype(std::declval<bounded::integer<0, 10>>() + 100), bounded::integer<0, 100>>::value);
static_assert(std::is_convertible<decltype(bounded::make(5)), bounded::integer<5, 5>>::value);

auto check_volatile() {
	bounded::integer<0, 6> volatile x = 3_bi;
	assert(x.value() == 3);
	x = 2_bi;
	assert(x.value() == 2);
}

auto check_hash() {
	std::unordered_map<bounded::integer<0, 100>, bounded::integer<0, 100>> const map = {
		{ 1_bi, 2_bi },
		{ 2_bi, 3_bi },
		{ 3_bi, 5_bi },
		{ 4_bi, 7_bi },
		{ 5_bi, 11_bi },
		{ 1_bi, 0_bi }
	};
	
	assert(map.size() == 5);
	assert(map.at(1_bi) == 2_bi);
	assert(map.at(3_bi) == 5_bi);
}

}	// namespace

auto main() -> int {
	check_numeric_limits_all();
	check_constructibility();
	check_minmax();
	check_policies();
	check_assignment();
	check_compound_arithmetic();
	check_math();
	check_arithmetic_range();
	check_optional();
	check_to_string();
	check_streaming();
	check_enum_construction();
	check_volatile();
	check_hash();
}
