// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/optional/common_type.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/common_type.hpp>

static_assert(
	std::is_same<std::common_type_t<bounded::none_t, int>, bounded::optional<int>>{},
	"common_type with none_t first wrong."
);
static_assert(
	std::is_same<std::common_type_t<int, bounded::none_t>, bounded::optional<int>>{},
	"common_type with none_t last wrong."
);
static_assert(
	std::is_same<
		std::common_type_t<bounded::constant_t<1>, bounded::none_t, bounded::constant_t<5>, bounded::none_t>,
		bounded::optional<bounded::integer<bounded::detail::normalize<1>, bounded::detail::normalize<5>>>
	>{},
	"common_type with bounded::integer and none_t wrong."
);


