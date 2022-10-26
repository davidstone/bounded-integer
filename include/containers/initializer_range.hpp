// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/is_range.hpp>
#include <containers/range_reference_t.hpp>
#include <containers/range_value_t.hpp>

#include <bounded/concepts.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Source>
using source_range_reference_t = std::conditional_t<
	is_container<Source>,
	range_value_t<Source> &&,
	range_reference_t<Source>
>;

} // namespace detail

template<typename Source, typename Target>
concept initializer_range =
	!std::is_array_v<std::remove_cvref_t<Source>> and
	range<Source> and
	bounded::constructible_from<range_value_t<Target>, detail::source_range_reference_t<Source>>;

// This avoids checking properties of an incomplete type
template<typename Source, typename Target>
concept constructor_initializer_range =
	!std::same_as<std::remove_cvref_t<Source>, Target> and
	initializer_range<Source, Target>;

} // namespace containers
