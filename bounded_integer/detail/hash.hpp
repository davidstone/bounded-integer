// std::hash specialization
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

#include "class.hpp"

#include <functional>

namespace std {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded::storage_type storage, bool poisoned>
struct hash<bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>> {
	using argument_type = bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>;
	constexpr decltype(auto) operator()(argument_type const & argument) const {
		return hash<typename argument_type::underlying_type>{}(argument.value());
	}
};

}	// namespace std
