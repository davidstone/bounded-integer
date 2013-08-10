#include "ranged_integer.hpp"
#include <limits>

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
	constexpr checked_integer<0, 11> const y(x);
	// constexpr checked_integer<2, 8> const z(x);
	// checked_integer<13, 63> const non_overlapping(x);
}
