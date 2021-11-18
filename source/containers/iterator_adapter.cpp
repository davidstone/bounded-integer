// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/iterator_adapter.hpp>

#include <containers/contiguous_iterator.hpp>
#include <containers/default_adapt_traits.hpp>

namespace {

struct traits :
	containers::default_dereference,
	containers::default_add,
	containers::default_subtract,
	containers::default_compare
{
};

template<typename T>
using iterator = containers::adapt_iterator<containers::contiguous_iterator<T, 2>, traits>;

using const_iterator = iterator<int const>;
using mutable_iterator = iterator<int>;

static_assert(std::convertible_to<const_iterator, const_iterator>);
static_assert(std::convertible_to<mutable_iterator, mutable_iterator>);
static_assert(std::convertible_to<mutable_iterator, const_iterator>);
static_assert(!std::convertible_to<const_iterator, mutable_iterator>);

} // namespace