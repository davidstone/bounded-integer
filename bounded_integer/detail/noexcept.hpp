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

#define BOUNDED_NOEXCEPT_INITIALIZATION(initializer) noexcept(noexcept(initializer)): initializer

// This should probably be adjusted to include converting to the return type
#define BOUNDED_NOEXCEPT(body) noexcept(noexcept(body)) { return body; }

