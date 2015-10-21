// Unary minus
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

#include "../common_type.hpp"
#include "../forward_declaration.hpp"

namespace bounded {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
constexpr auto operator-(integer<minimum, maximum, overflow_policy, storage, poisoned> const value) noexcept {
	using result_type = integer<-maximum, -minimum, overflow_policy, storage, poisoned>;
	using common_type = std::common_type_t<result_type, std::decay_t<decltype(value)>>;
	return result_type(-static_cast<typename common_type::underlying_type>(value), non_check);
}

}	// namespace bounded

