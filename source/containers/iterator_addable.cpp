// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iterator_addable;

import std_module;

namespace containers {

export template<typename Iterator, typename Offset>
concept iterator_addable = requires(Iterator it, Offset const offset) {
	{ std::move(it) + offset } -> std::same_as<std::remove_cvref_t<Iterator>>;
};

} // namespace containers
