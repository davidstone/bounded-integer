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
import containers.iter_reference_t;
import containers.iterator;
import containers.range;

import std_module;

namespace containers {

template<typename UnaryFunction>
struct transform_traits :
	default_add,
	default_subtract,
	default_compare,
	default_begin_end_size
{
	constexpr explicit transform_traits(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	template<iterator It> requires std::invocable<UnaryFunction const &, iter_reference_t<It>>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		return std::invoke(m_dereference, *it);
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

template<typename UnaryFunction>
transform_traits(UnaryFunction) -> transform_traits<UnaryFunction>;

template<typename UnaryFunction>
struct transform_traits_dereference :
	default_add,
	default_subtract,
	default_compare,
	default_begin_end_size
{
	constexpr explicit transform_traits_dereference(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	template<iterator It> requires std::invocable<UnaryFunction const &, It>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		return std::invoke(m_dereference, it);
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

template<typename UnaryFunction>
transform_traits_dereference(UnaryFunction) -> transform_traits_dereference<UnaryFunction>;

export constexpr auto transform(range auto && source, auto dereference) -> range auto {
	return adapt(OPERATORS_FORWARD(source), transform_traits(std::move(dereference)));
}

export constexpr auto transform_dereference(range auto && source, auto dereference) -> range auto {
	return adapt(OPERATORS_FORWARD(source), transform_traits_dereference(std::move(dereference)));
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
