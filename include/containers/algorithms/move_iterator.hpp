// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform_iterator.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

template<typename Iterator>
constexpr auto move_iterator(Iterator it_) {
	// TODO: lambda must take "auto" to work around some clang bug
	return ::containers::transform_iterator_dereference(it_, [](auto const it) -> decltype(auto) {
		using base_result = decltype(*it);
		using result = std::conditional_t<
			std::is_reference_v<base_result>,
			std::remove_reference_t<base_result> &&,
			base_result
		>;
		return static_cast<result>(*it);
	});
}

}	// namespace containers
