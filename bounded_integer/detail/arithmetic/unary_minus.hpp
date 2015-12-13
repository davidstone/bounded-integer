// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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

