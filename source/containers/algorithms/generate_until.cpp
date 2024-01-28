// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>

export module containers.algorithms.generate_until;

export import containers.common_iterator_functions;
import containers.stored_function;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Function>
struct generate_until_sentinel {
	explicit constexpr generate_until_sentinel(Function & is_done_):
		m_is_done(is_done_)
	{
	}
	constexpr auto is_done() const -> bool {
		return std::invoke(m_is_done.get());
	}
private:
	[[no_unique_address]] stored_function<Function> m_is_done;
};

template<typename Function>
struct generate_until_iterator {
	using iterator_category = std::input_iterator_tag;
	// TODO: ???
	using difference_type = decltype(bounded::integer(std::int64_t()));
	
	explicit constexpr generate_until_iterator(Function & generator):
		m_generator(generator)
	{
	}
	generate_until_iterator(generate_until_iterator &&) = default;
	generate_until_iterator(generate_until_iterator const &) = delete;
	auto operator=(generate_until_iterator &&) & -> generate_until_iterator & = default;
	auto operator=(generate_until_iterator const &) & -> generate_until_iterator & = delete;

	constexpr auto operator*() const -> decltype(auto) {
		return std::invoke(m_generator.get());
	}
	OPERATORS_ARROW_DEFINITIONS

	template<typename UntilFunction>
	friend constexpr auto operator==(generate_until_iterator const &, generate_until_sentinel<UntilFunction> const rhs) -> bool {
		return rhs.is_done();
	}

	friend constexpr auto operator+(generate_until_iterator it, bounded::constant_t<1>) -> generate_until_iterator {
		return it;
	}
private:
	[[no_unique_address]] stored_function<Function> m_generator;
};

export template<std::invocable Function, bounded::invocable_r<bool> UntilFunction>
struct generate_until {
	constexpr generate_until(Function generator, UntilFunction until):
		m_generator(std::move(generator)),
		m_until(std::move(until))
	{
	}

	constexpr auto begin() const requires std::invocable<Function const &> {
		return generate_until_iterator(m_generator);
	}
	constexpr auto begin() {
		return generate_until_iterator(m_generator);
	}
	constexpr auto end() const requires std::invocable<UntilFunction const &> {
		return generate_until_sentinel(m_until);
	}
	constexpr auto end() {
		return generate_until_sentinel(m_until);
	}
private:
	[[no_unique_address]] Function m_generator;
	[[no_unique_address]] UntilFunction m_until;
};

} // namespace containers

constexpr auto empty_generator = containers::generate_until(
	[] -> int { std::unreachable(); },
	[] { return true; }
);
static_assert(empty_generator.begin() == empty_generator.end());

static_assert([] {
	for (auto const _ : empty_generator) {
	}
	return true;
}());

static_assert([] {
	int x = 2;
	auto const g = containers::generate_until(
		[&] { return x *= 2; },
		[&] { return x > 4; }
	);
	for (auto const _ : g) {
	}
	return x == 8;
}());

