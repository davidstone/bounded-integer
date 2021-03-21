// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/to_radix_sort_key.hpp>

#include <containers/algorithms/sort.hpp>

#include <containers/array/array.hpp>
#include <containers/c_array.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

// nextafter is not constexpr. This signature is easier to implement
template<typename T>
constexpr auto increase_float_magnitude(T const value, unsigned const offset) {
	return __builtin_bit_cast(
		T,
		__builtin_bit_cast(containers::detail::float_to_unsigned<T>, value) + offset
	);
}

static_assert(increase_float_magnitude(0.0F, 1) > 0.0F);
static_assert(increase_float_magnitude(0.0F, 1) < 0.1F);
static_assert(increase_float_magnitude(-0.0F, 1) < -0.0F);
static_assert(increase_float_magnitude(-0.0F, 1) > -0.1F);

static_assert(increase_float_magnitude(0.0, 1) > 0.0);
static_assert(increase_float_magnitude(0.0, 1) < 0.1);
static_assert(increase_float_magnitude(-0.0, 1) < -0.0);
static_assert(increase_float_magnitude(-0.0, 1) > -0.1);


constexpr auto is_sorted_to_radix(auto... values) {
	auto const original = containers::array{values...};
	BOUNDED_TEST(containers::is_sorted(original));
	auto const converted = containers::array{containers::to_radix_sort_key(values)...};
	return containers::is_sorted(converted);
}

template<typename T>
constexpr auto is_sorted_converted_to_radix(auto... values) {
	return is_sorted_to_radix(static_cast<T>(values)...);
}


static_assert(is_sorted_converted_to_radix<std::int8_t>(
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127
));

static_assert(is_sorted_converted_to_radix<std::int16_t>(
	-32768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1000,
	32767
));

static_assert(is_sorted_converted_to_radix<std::int32_t>(
	bounded::min_value<std::int32_t>,
	bounded::min_value<std::int32_t> + 1,
	-1000001,
	-32769,
	-32768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1000,
	32767,
	32768,
	1000000,
	bounded::max_value<std::int32_t> - 1,
	bounded::max_value<std::int32_t>
));

static_assert(is_sorted_converted_to_radix<std::int64_t>(
	bounded::min_value<int64_t>,
	-1'000'000'000'000,
	static_cast<std::int64_t>(bounded::min_value<std::int32_t>) - 1,
	bounded::min_value<std::int32_t>,
	bounded::min_value<std::int32_t> + 1,
	-1'000'001,
	-32'769,
	-32'768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1'000,
	32'767,
	32'768,
	1'000'000,
	bounded::max_value<std::int32_t> - 1,
	bounded::max_value<std::int32_t>,
	bounded::max_value<std::int32_t>,
	1'000'000'000'000,
	bounded::max_value<std::int64_t>
));


template<typename T>
constexpr auto test_floating_point() {
	return is_sorted_converted_to_radix<T>(
		-std::numeric_limits<T>::infinity(),
		-777777777.7,
		-1000001.0,
		-32769.0,
		-32768.0,
		-256.0,
		-129.0,
		-128.0,
		-55.0,
		-12.4,
		-4.0,
		-0.0000002,
		increase_float_magnitude(T(-0.0), 2),
		increase_float_magnitude(T(-0.0), 1),
		-0.0,
		0.0,
		increase_float_magnitude(T(0.0), 1),
		increase_float_magnitude(T(0.0), 2),
		0.1,
		2.0,
		2.5,
		5.0,
		6.0,
		7.0,
		8.0,
		17.8,
		19.0,
		23.0,
		99.0,
		127.0,
		1000.0,
		32767.0,
		32768.0,
		1000000.0,
		444444444444.4,
		std::numeric_limits<T>::infinity()
	);
}

static_assert(test_floating_point<float>());
static_assert(test_floating_point<double>());

} // namespace

int main() {
	constexpr int array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	BOUNDED_TEST(is_sorted_to_radix(
		array + 0,
		array + 1,
		array + 2,
		array + 3,
		array + 4,
		array + 5,
		array + 6,
		array + 7,
		array + 8
	));
}