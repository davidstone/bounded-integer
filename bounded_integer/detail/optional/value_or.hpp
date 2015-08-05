// value_or gets an optional value or something else
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

#include "../conditional.hpp"

#include <stdexcept>

namespace bounded {

template<typename Optional, typename Default>
constexpr decltype(auto) value_or(Optional && value, Default && other) {
	return BOUNDED_CONDITIONAL(value, *std::forward<Optional>(value), std::forward<Default>(other));
}

struct bad_optional_access : std::logic_error {
	bad_optional_access():
		std::logic_error("bad optional access") {
	}
};

template<typename Optional>
constexpr decltype(auto) value(Optional && value) {
	if (!value) {
		throw bad_optional_access{};
	}
	return *std::forward<Optional>(value);
}

}	// namespace bounded
