// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/supports_lazy_insert_after.hpp>

#include <containers/bidirectional_linked_list.hpp>

#include <list>

namespace {

static_assert(!containers::detail::supports_lazy_insert_after<containers::bidirectional_linked_list<int>>);
static_assert(!containers::detail::supports_lazy_insert_after<std::list<int>>);
static_assert(!containers::detail::supports_lazy_insert_after<int>);
static_assert(!containers::detail::supports_lazy_insert_after<void>);

} // namespace