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
import containers.begin_end;
import containers.default_adapt_traits;
import containers.default_begin_end_size;
import containers.iterator;
import containers.iterator_t;
import containers.iterator_to_sentinel;
import containers.range;
import containers.range_reference_t;
import containers.stored_function;
import containers.value_wrapper;

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

template<typename UnaryFunction, bool pass_iterator>
struct transform_iterator_traits_non_idempotent :
	default_add,
	default_subtract,
	default_compare
{
	constexpr transform_iterator_traits_non_idempotent() = default;
	constexpr explicit transform_iterator_traits_non_idempotent(UnaryFunction const & dereference_):
		m_dereference(dereference_)
	{
	}
	transform_iterator_traits_non_idempotent(transform_iterator_traits_non_idempotent &&) = default;
	transform_iterator_traits_non_idempotent(transform_iterator_traits_non_idempotent const &) = delete;
	auto operator=(transform_iterator_traits_non_idempotent &&) -> transform_iterator_traits_non_idempotent & = default;
	auto operator=(transform_iterator_traits_non_idempotent const &) -> transform_iterator_traits_non_idempotent & = delete;

	template<iterator It>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		if constexpr (pass_iterator) {
			return std::invoke(m_dereference.get(), it);
		} else {
			return std::invoke(m_dereference.get(), *it);
		}
	}

private:
	[[no_unique_address]] stored_function<UnaryFunction const> m_dereference;
};

template<typename UnaryFunction, bool pass_iterator>
struct transform_traits_non_idempotent : default_begin_end_size {
	constexpr transform_traits_non_idempotent() = default;
	constexpr explicit transform_traits_non_idempotent(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	transform_traits_non_idempotent(transform_traits_non_idempotent &&) = default;
	transform_traits_non_idempotent(transform_traits_non_idempotent const &) = delete;
	auto operator=(transform_traits_non_idempotent &&) -> transform_traits_non_idempotent & = default;
	auto operator=(transform_traits_non_idempotent const &) -> transform_traits_non_idempotent & = delete;
	
	static constexpr auto get_end(range auto && range) -> decltype(auto) {
		return iterator_to_sentinel(containers::end(OPERATORS_FORWARD(range)));
	}

	constexpr auto iterator_traits(auto const &) const -> value_wrapper<transform_iterator_traits_non_idempotent<UnaryFunction, pass_iterator>> {
		return value_wrapper(transform_iterator_traits_non_idempotent<UnaryFunction, pass_iterator>(m_dereference));
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

template<typename Function, typename Range>
concept element_invocable = std::invocable<Function, range_reference_t<Range>>;

template<typename Function, typename Range>
concept iterator_invocable = std::invocable<Function, iterator_t<Range>>;

export template<range Range, element_invocable<Range> UnaryFunction>
constexpr auto transform(Range && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits<UnaryFunction, false>(std::move(dereference))
	);
}

export template<range Range, iterator_invocable<Range> UnaryFunction>
constexpr auto transform_dereference(Range && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits<UnaryFunction, true>(std::move(dereference))
	);
}

export template<range Range, element_invocable<Range> UnaryFunction>
constexpr auto transform_non_idempotent(Range && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits_non_idempotent<UnaryFunction, false>(std::move(dereference))
	);
}

export template<range Range, iterator_invocable<Range> UnaryFunction>
constexpr auto transform_dereference_non_idempotent(Range && source, UnaryFunction dereference) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		transform_traits_non_idempotent<UnaryFunction, true>(std::move(dereference))
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

static_assert(containers::equal(
	containers::transform_non_idempotent(array, [](S const s) { return s; }),
	array
));

constexpr auto integer_array = containers::array{0, 1, 2, 3, 4};

static_assert(containers::equal(
	containers::transform(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform_non_idempotent(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s.f(); }),
	integer_array
));

static_assert(containers::equal(
	containers::transform_non_idempotent(array, [](S const s) { return s.f(); }),
	integer_array
));
