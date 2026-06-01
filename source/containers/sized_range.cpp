// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.sized_range;

import containers.forward_random_access_range;
import containers.has_member_size;
import containers.range;

import bounded;
import std_module;

namespace containers {

export template<typename Range>
concept sized_range = (range<Range> and has_member_size<Range>) or forward_random_access_range<Range>;

template<typename T>
constexpr auto is_constant_integer = false;

template<auto value>
constexpr auto is_constant_integer<bounded::constant_t<value>> = true;

template<typename Range>
concept has_static_size =
	std::is_array_v<Range> or
	requires { Range::size(); } or
	is_constant_integer<decltype(bounded::declval<Range>().size())>;

export template<typename Range>
concept statically_sized_range = range<Range> and has_static_size<std::remove_cvref_t<Range>>;

} // namespace containers
