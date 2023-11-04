// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.none;
 
namespace tv {

// none_t cannot be default constructible or we get an ambiguity in op = {};
export struct none_t {
	constexpr explicit none_t(int) {}
};
export constexpr auto none = none_t(0);

} // namespace tv
