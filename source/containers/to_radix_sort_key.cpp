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


constexpr auto is_sorted_converted(auto... values) {
	auto const original = containers::array{values...};
	BOUNDED_TEST(containers::is_sorted(original));
	auto const converted = containers::array{containers::to_radix_sort_key(values)...};
	return containers::is_sorted(converted);
}

template<typename T>
constexpr auto test_floating_point() {
	return is_sorted_converted(
		-std::numeric_limits<T>::infinity(),
		T(-777777777.7),
		T(-1000001.0),
		T(-32769.0),
		T(-32768.0),
		T(-256.0),
		T(-129.0),
		T(-128.0),
		T(-55.0),
		T(-12.4),
		T(-4.0),
		T(-0.0000002),
		increase_float_magnitude(T(-0.0), 2),
		increase_float_magnitude(T(-0.0), 1),
		T(-0.0),
		T(0.0),
		increase_float_magnitude(T(0.0), 1),
		increase_float_magnitude(T(0.0), 2),
		T(0.1),
		T(2.0),
		T(2.5),
		T(5.0),
		T(6.0),
		T(7.0),
		T(8.0),
		T(17.8),
		T(19.0),
		T(23.0),
		T(99.0),
		T(127.0),
		T(1000.0),
		T(32767.0),
		T(32768.0),
		T(1000000.0),
		T(444444444444.4),
		std::numeric_limits<T>::infinity()
	);
}

static_assert(test_floating_point<float>());
static_assert(test_floating_point<double>());

} // namespace
