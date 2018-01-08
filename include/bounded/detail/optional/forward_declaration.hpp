// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<typename T>
struct optional;

struct optional_tag {
private:
	template<typename>
	friend struct optional;
	
	optional_tag() = default;
	optional_tag(optional_tag const &) = default;
	optional_tag(optional_tag &&) = default;
};

// none_t cannot be default constructible or we get an ambiguity in op = {};
struct none_t {
	constexpr explicit none_t(int) noexcept {}
};
constexpr auto none = none_t(0);

}	// namespace bounded
