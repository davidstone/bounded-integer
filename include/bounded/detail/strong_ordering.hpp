// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>

namespace bounded {

struct strong_ordering {
	constexpr explicit strong_ordering(int value_) noexcept:
		value(value_)
	{
	}

	friend constexpr auto operator==(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value == 0;
	}
	friend constexpr auto operator==(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 == rhs.value;
	}

	friend constexpr auto operator!=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value != 0;
	}
	friend constexpr auto operator!=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 != rhs.value;
	}

	friend constexpr auto operator<(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value < 0;
	}
	friend constexpr auto operator<(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 < rhs.value;
	}

	friend constexpr auto operator>(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value > 0;
	}
	friend constexpr auto operator>(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 > rhs.value;
	}

	friend constexpr auto operator<=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value <= 0;
	}
	friend constexpr auto operator<=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 <= rhs.value;
	}

	friend constexpr auto operator>=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value >= 0;
	}
	friend constexpr auto operator>=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 >= rhs.value;
	}

private:
	int value;
};

constexpr auto strong_ordering_less = strong_ordering(-1);
constexpr auto strong_ordering_equal = strong_ordering(0);
constexpr auto strong_ordering_greater = strong_ordering(1);

}	// namespace bounded