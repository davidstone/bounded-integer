// Specialization of numeric_limits
// Copyright (C) 2015 David Stone
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

#pragma once

#include "forward_declaration.hpp"
#include <cstdint>
#include <limits>
#include <type_traits>

namespace bounded {

// This does not requiring having a definition of bounded::integer to get at the
// minimum and maximum, so it can be used in the definition of bounded::integer.
template<typename T>
struct basic_numeric_limits {
private:
	using real_numeric_limits = std::conditional_t<std::is_same<T, std::decay_t<T>>::value,
		std::numeric_limits<T>,
		basic_numeric_limits<std::decay_t<T>>
	>;
public:
	static constexpr auto min() noexcept {
		return real_numeric_limits::min();
	}
	static constexpr auto max() noexcept {
		return real_numeric_limits::max();
	}
	static constexpr auto is_specialized = real_numeric_limits::is_specialized;
	static constexpr auto is_integer = real_numeric_limits::is_integer;
	static constexpr auto is_bounded = real_numeric_limits::is_bounded;
};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
struct basic_numeric_limits<integer<minimum, maximum, overflow_policy, storage, poisoned>> {
	static constexpr auto min() noexcept {
		return minimum;
	}
	static constexpr auto max() noexcept {
		return maximum;
	}
	static constexpr auto is_specialized = true;
	static constexpr auto is_integer = true;
	static constexpr auto is_bounded = true;
};

template<typename T, T value>
struct basic_numeric_limits<std::integral_constant<T, value>> {
	static constexpr auto min() noexcept {
		return value;
	}
	static constexpr auto max() noexcept {
		return value;
	}
	static constexpr auto is_specialized = std::numeric_limits<T>::is_specialized;
	static constexpr auto is_integer = std::numeric_limits<T>::is_integer;
	static constexpr auto is_bounded = true;
};

}	// namespace bounded
