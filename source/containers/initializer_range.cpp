// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.initializer_range;

import containers.is_container;
import containers.is_range;
import containers.range_reference_t;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

template<typename Source>
using source_range_reference_t = std::conditional_t<
	is_container<Source>,
	range_value_t<Source> &&,
	range_reference_t<Source>
>;

export template<typename Source, typename Target>
concept initializer_range =
	!std::is_array_v<std::remove_cvref_t<Source>> and
	range<Source> and
	bounded::constructible_from<range_value_t<Target>, source_range_reference_t<Source>>;

// This avoids checking properties of an incomplete type
export template<typename Source, typename Target>
concept constructor_initializer_range =
	!std::same_as<std::remove_cvref_t<Source>, Target> and
	initializer_range<Source, Target>;

} // namespace containers
