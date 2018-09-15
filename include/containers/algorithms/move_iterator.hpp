// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform_iterator.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

struct move_dereference {
private:
	template<typename Iterator, typename base_result = decltype(*std::declval<Iterator>())>
	using result = std::conditional_t<
		std::is_reference_v<base_result>,
		std::remove_reference_t<base_result> &&,
		base_result
	>;
public:
	template<typename Iterator>
	constexpr auto operator()(Iterator const it) const BOUNDED_NOEXCEPT_DECLTYPE(
		static_cast<result<Iterator>>(*it)
	)
};

}	// namespace detail

template<typename Iterator>
constexpr auto move_iterator(Iterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::transform_iterator_dereference(it, detail::move_dereference{})
)

}	// namespace containers
