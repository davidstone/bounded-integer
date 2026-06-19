// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.span;

import containers.begin_end;
import containers.c_array;
export import containers.common_iterator_functions;
import containers.data;
import containers.size;
import containers.span;
import containers.to_address;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename T>
struct pointer_size {
	static constexpr auto data() -> T * {
		return nullptr;
	}
	static constexpr auto size() -> bounded::constant_t<0> {
		return 0_bi;
	}
};

template<typename T>
struct simple_iterator {
	auto operator*() const -> T & {
		std::unreachable();
	}
	static constexpr auto to_address() -> T * {
		return nullptr;
	}
	friend constexpr auto operator+(simple_iterator, bounded::constant_t<0>) -> simple_iterator {
		return simple_iterator();
	}
	friend auto operator+(simple_iterator, bounded::constant_t<1>) -> simple_iterator {
		std::unreachable();
	}
	friend constexpr auto operator-(simple_iterator, simple_iterator) -> bounded::constant_t<0> {
		return 0_bi;
	}
	friend auto operator<=>(simple_iterator, simple_iterator) = default;
};

template<typename T>
struct begin_size {
	static constexpr auto begin() -> simple_iterator<T> {
		return simple_iterator<T>();
	}
	static constexpr auto size() -> bounded::constant_t<0> {
		return 0_bi;
	}
};
template<typename T>
struct begin_end {
	static constexpr auto begin() -> simple_iterator<T> {
		return simple_iterator<T>();
	}
	static constexpr auto end() -> simple_iterator<T> {
		return simple_iterator<T>();
	}
};

} // namespace containers

using containers::pointer_size;
using containers::begin_size;
using containers::begin_end;

static_assert(bounded::convertible_to<containers::span<int const>, containers::span<int const>>);
static_assert(!bounded::convertible_to<containers::span<int const>, containers::span<int>>);
static_assert(bounded::convertible_to<containers::span<int>, containers::span<int const>>);
static_assert(bounded::convertible_to<containers::span<int>, containers::span<int>>);

static_assert(bounded::convertible_to<pointer_size<int const>, containers::span<int const>>);
static_assert(!bounded::convertible_to<pointer_size<int const>, containers::span<int>>);
static_assert(bounded::convertible_to<begin_size<int const>, containers::span<int const>>);
static_assert(!bounded::convertible_to<begin_size<int const>, containers::span<int>>);
static_assert(bounded::convertible_to<begin_end<int const>, containers::span<int const>>);
static_assert(!bounded::convertible_to<begin_end<int const>, containers::span<int>>);

static_assert(bounded::convertible_to<pointer_size<int>, containers::span<int const>>);
static_assert(bounded::convertible_to<pointer_size<int>, containers::span<int>>);
static_assert(bounded::convertible_to<begin_size<int>, containers::span<int const>>);
static_assert(bounded::convertible_to<begin_size<int>, containers::span<int>>);
static_assert(bounded::convertible_to<begin_end<int>, containers::span<int const>>);
static_assert(bounded::convertible_to<begin_end<int>, containers::span<int>>);

struct base {
};
struct derived : base {
};

static_assert(bounded::convertible_to<pointer_size<base>, containers::span<base>>);
static_assert(!bounded::convertible_to<pointer_size<base>, containers::span<derived>>);
static_assert(!bounded::convertible_to<pointer_size<derived>, containers::span<base>>);
static_assert(bounded::convertible_to<pointer_size<derived>, containers::span<derived>>);

static_assert(bounded::convertible_to<containers::span<int const>, std::span<int const>>);
static_assert(!bounded::convertible_to<containers::span<int const>, std::span<int>>);
static_assert(bounded::convertible_to<containers::span<int>, std::span<int const>>);
static_assert(bounded::convertible_to<containers::span<int>, std::span<int>>);

static_assert(bounded::convertible_to<std::span<int const>, containers::span<int const>>);
static_assert(!bounded::convertible_to<std::span<int const>, containers::span<int>>);
static_assert(bounded::convertible_to<std::span<int>, containers::span<int const>>);
static_assert(bounded::convertible_to<std::span<int>, containers::span<int>>);

constexpr auto check_equal(auto const & input) {
	auto const output = containers::span(input);
	return
		containers::data(output) == containers::data(input) and
		containers::size(output) == containers::size(input);
}

constexpr containers::c_array<int, 3> a = {1, 2, 3};

static_assert(check_equal(a));

struct array_wrapper {
	static constexpr auto data() -> int const * {
		return a;
	}
	static constexpr auto size() {
		return 3_bi;
	}
};

static_assert(check_equal(array_wrapper()));
