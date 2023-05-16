// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.map_tags;

namespace containers {

export struct assume_sorted_unique_t {};
export constexpr auto assume_sorted_unique = assume_sorted_unique_t();

export struct assume_unique_t {};
export constexpr auto assume_unique = assume_unique_t();

export template<typename Iterator>
struct inserted_t {
	Iterator iterator;
	bool inserted;
};

template<typename Iterator>
inserted_t(Iterator, bool) -> inserted_t<Iterator>;

} // namespace containers