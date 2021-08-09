// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/mutable_iterator.hpp>

#include <containers/array/array.hpp>
#include <containers/iterator_t.hpp>

#include "../test_assert.hpp"

namespace {

using container = containers::array<int, 1>;

template<typename Range, typename Iterator>
concept can_get_mutable_iterator_from = requires(Iterator it) {
	{ containers::detail::mutable_iterator(std::declval<Range>(), it) } -> std::same_as<containers::iterator_t<container &>>;
};

template<typename Range>
concept can_get_mutable_iterator =
	can_get_mutable_iterator_from<Range, decltype(containers::begin(std::declval<container const &>()))> and
	can_get_mutable_iterator_from<Range, decltype(containers::begin(std::declval<container &>()))>;

static_assert(!can_get_mutable_iterator<container const &>);
static_assert(!can_get_mutable_iterator<container &&>);
static_assert(can_get_mutable_iterator<container &>);

} // namespace