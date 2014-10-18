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

template<typename integer>
auto check_numeric_limits() {
	using int_limits = std::numeric_limits<integer>;
	using bounded_t = bounded::checked_integer<int_limits::min(), int_limits::max()>;
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
		static_assert(int_limits::function() == bounded_limits::function(), #function "() is wrong.")
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

auto check_single_argument_minmax() {
	constexpr auto value = 5_bi;
	static_assert(bounded::min(value) == value, "A value does not have itself as the minimum.");
	static_assert(bounded::max(value) == value, "A value does not have itself as the maximum.");
}

auto check_double_argument_minmax() {
	constexpr auto lower_value = 6_bi;
	constexpr auto greater_value = 10_bi;
	static_assert(bounded::min(lower_value, greater_value) == lower_value, "Two argument min value incorrect.");
	static_assert(bounded::min(greater_value, lower_value) == lower_value, "Two argument min value incorrect.");
	static_assert(bounded::max(lower_value, greater_value) == greater_value, "Two argument max value incorrect.");
	static_assert(bounded::max(greater_value, lower_value) == greater_value, "Two argument max value incorrect.");
}

auto check_many_argument_minmax() {
	constexpr bounded::integer<-53, 1000> value(3_bi);
	constexpr auto minimum = bounded::min(0_bi, 10_bi, 5_bi, value);
	using min_type = decltype(minimum);
	static_assert(minimum == 0_bi, "Incorrect minimum value.");
	static_assert(std::numeric_limits<min_type>::min() == -53, "Incorrect minimum minimum.");
	static_assert(std::numeric_limits<min_type>::max() == 0, "Incorrect maximum minimum.");

	constexpr auto maximum = bounded::max(0_bi, 10_bi, 5_bi, value);
	using max_type = decltype(maximum);
	static_assert(maximum == 10_bi, "Incorrect maximum value.");
	static_assert(std::numeric_limits<max_type>::min() == 10, "Incorrect minimum maximum.");
	static_assert(std::numeric_limits<max_type>::max() == 1000, "Incorrect maximum maximum.");
}

auto check_non_bounded_minmax() {
	constexpr auto integer_min = bounded::min(0, 5);
	static_assert(std::is_same<decltype(integer_min), int const>::value, "Incorrect type of min for int arguments.");
	static_assert(integer_min == 0, "Incorrect value of min for int arguments.");
}

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

auto check_non_integer_minmax() {
	class string_view {
	public:
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
	class class_type {
	public:
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
	check_single_argument_minmax();
	check_double_argument_minmax();
	check_many_argument_minmax();
	check_non_bounded_minmax();
	check_non_integer_minmax();
	check_reference_minmax();
}

auto check_throw_policy() {
	using bounded::checked_integer;
	static_assert(
		!noexcept(std::declval<checked_integer<0, 0> &>() = std::declval<checked_integer<0, 1> &>()),
		"Shouldn't be noexcept."
	);
	static constexpr intmax_t minimum = 0;
	static constexpr intmax_t maximum = 10;
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
	constexpr Initial value(initial_value);
	constexpr auto absolute = bounded::abs(value);
	constexpr Expected expected_absolute(expected_value);
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
	constexpr auto value = 3_bi;
	constexpr auto min = 1_bi;
	constexpr auto max = 7_bi;
	constexpr auto static_min = 0;
	constexpr auto static_max = 10;
	using policy_type = bounded::dynamic_policy<static_min, static_max, bounded::throw_policy>;
	using type = bounded::integer<static_min, static_max, policy_type>;
	constexpr policy_type policy(min, max);
	constexpr type compile(value, policy);
	static_assert(compile == value, "Incorrect value with dynamic bounds.");
	static_assert(compile.overflow_policy().min() == min, "Incorrect dynamic min with dynamic bounds.");
	static_assert(compile.overflow_policy().max() == max, "Incorrect dynamic max with dynamic bounds.");

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
