// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<typename... policies>
struct common_policy {
};

template<typename policy1, typename policy2, typename... policies>
struct common_policy<policy1, policy2, policies...> {
	using type = typename common_policy<typename common_policy<policy1, policy2>::type, policies...>::type;
};

template<typename policy>
struct common_policy<policy> {
	using type = policy;
};

template<typename policy>
struct common_policy<policy, policy> {
	using type = policy;
};

template<typename... policies>
using common_policy_t = typename common_policy<policies...>::type;

}	// namespace bounded

