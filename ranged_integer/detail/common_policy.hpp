// Specifies which policy should be used in the result type of some operations
// Copyright (C) 2013 David Stone
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

#ifndef RANGED_INTEGER_COMMON_POLICY_HPP_
#define RANGED_INTEGER_COMMON_POLICY_HPP_

#include "forward_declaration.hpp"
#include "policy.hpp"
#include <cstdint>

// I wanted the type alias to just be the policy, but that ends up not working
// for nested common_policy instantiations, so I use this instead. See
// http://stackoverflow.com/questions/13896509/is-it-possible-to-define-an-alias-for-a-template-template-parameter

template<typename policy1, typename policy2>
class common_policy {
};

template<typename policy>
class common_policy<policy, policy> {
public:
	template<intmax_t minimum, intmax_t maximum>
	using type = ranged_integer<minimum, maximum, policy>;
};

template<typename policy>
class common_policy<policy, null_policy> {
public:
	template<intmax_t minimum, intmax_t maximum>
	using type = ranged_integer<minimum, maximum, policy>;
};
template<typename policy>
class common_policy<null_policy, policy> {
public:
	template<intmax_t minimum, intmax_t maximum>
	using type = ranged_integer<minimum, maximum, policy>;
};
template<>
class common_policy<null_policy, null_policy> {
public:
	template<intmax_t minimum, intmax_t maximum>
	using type = ranged_integer<minimum, maximum, null_policy>;
};

#endif	// RANGED_INTEGER_COMMON_POLICY_HPP_
