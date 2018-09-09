// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward.hpp>

#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

// This would not be necessary (and the solution would be better, without any of
// the pitfalls mentioned below) if we had my paper

template<typename Iterator, typename Enable = void>
struct operator_arrow {
};

template<typename Iterator>
struct operator_arrow<Iterator, std::void_t<decltype(std::addressof(*std::declval<Iterator const &>()))>> {
	constexpr auto operator->() const noexcept(noexcept(*std::declval<Iterator const &>())) {
		return std::addressof(*static_cast<Iterator const &>(*this));
	}
};

#if 0
// Another possible implementation of operator-> is as follows, which works with
// any iterator, even those that return by value

struct temporary_operator_arrow {
	using reference = decltype(*std::declval<Iterator const &>()) &&;
	constexpr explicit temporary_operator_arrow(reference value) noexcept:
		m_value(BOUNDED_FORWARD(value))
	{
	}
	constexpr auto operator->() && noexcept {
		return std::addressof(m_value);
	}

private:
	reference m_value;
};

constexpr auto operator->() const BOUNDED_NOEXCEPT(
	temporary_operator_arrow(**this)
)

// However, this has the nasty effect of silently breaking lifetime extension
// with references. In other words, if *it returns a prvalue instead of a
// glvalue (by value instead of by reference), the following code does not work:

auto && ref = it->x;
use(x);

// But the following code does:

use(it->x);

// This is fragile behavior, so for now, disable the operator-> overload if *it
// is a prvalue.
#endif

}	// namespace detail
}	// namespace containers