// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.supports_lazy_insert_after;

import containers.iterator_t;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

export template<typename Container>
concept supports_lazy_insert_after =
	requires(Container container, bounded::function_ptr<range_value_t<Container>> make) {
		{ container.lazy_insert_after(container.before_begin(), make) } -> std::same_as<iterator_t<Container &>>;
	};

} // namespace containers

struct has_lazy_insert_after {
	auto before_begin() const -> int const *;
	auto begin() const -> int const *;
	auto end() const -> int const *;
	auto lazy_insert_after(int const *, bounded::construct_function_for<int> auto) -> int const *;
};

struct has_no_lazy_insert_after {
	auto begin() const -> int const *;
	auto end() const -> int const *;
};

static_assert(containers::supports_lazy_insert_after<has_lazy_insert_after>);
static_assert(!containers::supports_lazy_insert_after<has_no_lazy_insert_after>);
static_assert(!containers::supports_lazy_insert_after<int>);
static_assert(!containers::supports_lazy_insert_after<void>);