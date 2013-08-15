#include "ranged_integer.hpp"
#include <cassert>
#include <limits>
#include <iostream>

namespace {
template<typename Integer>
void check_size() {
	static_assert(sizeof(checked_integer<std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::max()>) == sizeof(Integer), "checked_integer wrong size.");
}
void check_sizes() {
	check_size<int8_t>();
	check_size<uint8_t>();
	check_size<int16_t>();
	check_size<uint16_t>();
	check_size<int32_t>();
	check_size<uint32_t>();
	check_size<int64_t>();
	// Currently does not support unsigned types equal to uintmax_t
	// check_size<uint64_t>();
}

}	// namespace

int main() {
	check_sizes();
	constexpr checked_integer<1, 10> const x(9);
	static_assert(sizeof(x) == 1, "checked_integer too big!");
	constexpr checked_integer<-3, 11> const y(x);
	constexpr checked_integer<-3, 11> const z(4);

	constexpr auto sum = x + z;
	static_assert(sum.min == -2, "Minimum sum incorrect.");
	static_assert(sum.max == 21, "Maximum sum incorrect.");
	assert(sum.value() == 13);

	constexpr auto difference = x - z;
	static_assert(difference.min == -10, "Minimum difference incorrect.");
	static_assert(difference.max == 13, "Maximum difference incorrect.");
	assert(difference.value() == 5);

	constexpr auto product = x * z;
	static_assert(product.min == -30, "Minimum product incorrect.");
	static_assert(product.max == 110, "Maximum product incorrect.");
	assert(product.value() == 36);

	constexpr auto quotient = x / z;
	static_assert(quotient.min == -3, "Minimum quotient incorrect.");
	static_assert(quotient.max == 10, "Maximum quotient incorrect.");
	assert(quotient.value() == 2);
	
	assert(quotient < product);
	assert(difference + 8 == sum);

	// constexpr checked_integer<2, 8> const z(x);
	// checked_integer<13, 63> const non_overlapping(x);
}
