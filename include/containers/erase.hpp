// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterator.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/pop_back.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {

template<iterator Iterator>
constexpr auto erase(resizable_container auto & source, Iterator const first_, Iterator const last_) {
	auto const first = ::containers::detail::mutable_iterator(source, first_);
	auto const last = ::containers::detail::mutable_iterator(source, last_);
	auto const to_clear = ::containers::move(last, end(source), first).output;
	while (to_clear != end(source)) {
		pop_back(source);
	}
	return first;
}

template<resizable_container Container>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	BOUNDED_ASSERT(it != end(container));
	return erase(container, it, ::containers::next(it));
}

constexpr auto erase_if(resizable_container auto & source, auto predicate) {
	return erase(source, ::containers::remove_if(source, std::move(predicate)), end(source));
}

} // namespace containers
