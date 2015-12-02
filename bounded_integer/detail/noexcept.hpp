// Macro to simplify noexcept declarations
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

#include <type_traits>

// These must use a variadic macro in case the argument has a comma

#define BOUNDED_NOEXCEPT_INITIALIZATION(...) noexcept(noexcept(__VA_ARGS__)): __VA_ARGS__

// No SFINAE
#define BOUNDED_NOEXCEPT(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

// Same as returning decltype(auto), but with expression SFINAE
#define BOUNDED_NOEXCEPT_DECLTYPE(...) \
	noexcept( \
		noexcept(__VA_ARGS__) and \
		std::is_nothrow_move_constructible<decltype(__VA_ARGS__)>::value \
	) -> decltype(__VA_ARGS__) { \
		return __VA_ARGS__; \
	}

// Same as returning auto, but with expression SFINAE
#define BOUNDED_NOEXCEPT_VALUE(...) \
	noexcept( \
		noexcept(__VA_ARGS__) and \
		std::is_nothrow_constructible<std::decay_t<decltype(__VA_ARGS__)>, decltype(__VA_ARGS__)>::value \
	) -> std::decay_t<decltype(__VA_ARGS__)> { \
		return __VA_ARGS__; \
	}

// Same as returning auto &, but with expression SFINAE
#define BOUNDED_NOEXCEPT_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) & { \
		return __VA_ARGS__; \
	}

// Same as returning auto const &, but with expression SFINAE
#define BOUNDED_NOEXCEPT_CONST_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) const & { \
		return __VA_ARGS__; \
	}

// Same as returning auto &&, but with expression SFINAE
#define BOUNDED_NOEXCEPT_REF_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) && { \
		return __VA_ARGS__; \
	}

