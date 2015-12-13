// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "null_policy.hpp"
#include "../make.hpp"
#include "../minmax.hpp"

namespace bounded {

struct clamp_policy {
	constexpr clamp_policy() noexcept {}

	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept {
		using policy = bounded::null_policy;
		return min(max(bounded::make<policy>(std::forward<T>(value)), bounded::make<policy>(std::forward<Minimum>(minimum))), bounded::make<policy>(std::forward<Maximum>(maximum)));
	}

	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = false;
};

}	// namespace bounded
