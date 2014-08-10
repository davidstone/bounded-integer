// Specifies which policy should be used in the result type of some operations
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_POLICY_COMMON_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_COMMON_POLICY_HPP_

#include "null_policy.hpp"

namespace bounded {

template<typename... policies>
class common_policy {
};

template<typename policy1, typename policy2, typename... policies>
class common_policy<policy1, policy2, policies...> {
public:
	using type = typename common_policy<typename common_policy<policy1, policy2>::type, policies...>::type;
};

template<typename policy>
class common_policy<policy> {
public:
	using type = policy;
};

template<typename policy>
class common_policy<policy, policy> {
public:
	using type = policy;
};

template<typename policy>
class common_policy<policy, null_policy> {
public:
	using type = policy;
};
template<typename policy>
class common_policy<null_policy, policy> {
public:
	using type = policy;
};
template<>
class common_policy<null_policy, null_policy> {
public:
	using type = null_policy;
};

template<typename... policies>
using common_policy_t = typename common_policy<policies...>::type;

}	// namespace bounded

#endif	// BOUNDED_INTEGER_POLICY_COMMON_POLICY_HPP_
