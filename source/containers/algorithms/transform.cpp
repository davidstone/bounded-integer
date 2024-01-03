// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.transform;

import containers.algorithms.compare;
import containers.array;
import containers.adapt;
import containers.default_adapt_traits;
import containers.default_begin_end_size;
import containers.iterator;
import containers.range;

import std_module;

namespace containers {

template<typename UnaryFunction, bool pass_iterator>
struct transform_traits :
	default_add,
	default_subtract,
	default_compare,
	default_begin_end_size
{
	constexpr transform_traits() = default;
	constexpr explicit transform_traits(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	template<iterator It>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		if constexpr (pass_iterator) {
			return std::invoke(m_dereference, it);
		} else {
			return std::invoke(m_dereference, *it);
		}
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

export template<typename UnaryFunction>
constexpr auto transform(range auto && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits<UnaryFunction, false>(std::move(dereference))
	);
}

export template<typename UnaryFunction>
constexpr auto transform_dereference(range auto && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits<UnaryFunction, true>(std::move(dereference))
	);
}

} // namespace containers

struct S {
	constexpr auto f() const { return n; }

	friend auto operator==(S, S) -> bool = default;

	int n;
};

constexpr auto array = containers::array{S{0}, S{1}, S{2}, S{3}, S{4}};

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s; }),
	array
));

constexpr auto integer_array = containers::array{0, 1, 2, 3, 4};
static_assert(containers::equal(
	containers::transform(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s.f(); }),
	integer_array
));
