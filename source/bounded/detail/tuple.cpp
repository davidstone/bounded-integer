// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/tuple.hpp>
#include <bounded/detail/literal.hpp>

#include <type_traits>

namespace {

using namespace bounded::literal;
BOUNDED_COMPARISON

static_assert(std::is_empty_v<bounded::tuple<>>);
static_assert(bounded::tuple{} == bounded::tuple{});

struct empty {};

constexpr auto operator<=>(empty, empty) {
	return std::strong_ordering::equal;
}

constexpr auto operator==(empty, empty) {
	return true;
}

template<typename T, typename Index>
concept indexable_by = requires(T t, Index index) { t[index]; };

template<typename T, auto index>
concept constant_indexable_by = indexable_by<T, bounded::constant_t<index>>;

template<typename T, typename Index>
concept type_indexable_by = indexable_by<T, bounded::detail::types<Index>>;


namespace tuple_empty {

using type = bounded::tuple<empty>;
static_assert(std::is_empty_v<type>);
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::is_same_v<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::is_same_v<
	decltype(type()[bounded::detail::types<empty>()]),
	empty &&
>);

} // namespace tuple_empty

namespace tuple_empty_empty {

using type = bounded::tuple<empty, empty>;
static_assert(std::is_empty_v<type>);
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::is_same_v<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::is_same_v<
	decltype(type()[1_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_empty_empty

namespace tuple_tuple {

using type = bounded::tuple<bounded::tuple<>>;
static_assert(std::is_empty_v<type>);
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::is_same_v<
	decltype(type()[0_bi]),
	bounded::tuple<> &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::is_same_v<
	decltype(type()[bounded::detail::types<bounded::tuple<>>()]),
	bounded::tuple<> &&
>);

} // namespace tuple_tuple

namespace tuple_empty_tuple_empty {

using type = bounded::tuple<empty, bounded::tuple<empty>>;
static_assert(std::is_empty_v<type>);
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::is_same_v<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::is_same_v<
	decltype(type()[1_bi]),
	bounded::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(std::is_same_v<
	decltype(type()[bounded::detail::types<empty>()]),
	empty &&
>);
static_assert(std::is_same_v<
	decltype(type()[bounded::detail::types<bounded::tuple<empty>>()]),
	bounded::tuple<empty> &&
>);

} // namespace tuple_empty_tuple_empty

namespace tuple_tuple_empty_tuple_empty {

using type = bounded::tuple<bounded::tuple<empty>, bounded::tuple<empty>>;
static_assert(std::is_empty_v<type>);
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::is_same_v<
	decltype(type()[0_bi]),
	bounded::tuple<empty> &&
>);
static_assert(std::is_same_v<
	decltype(type()[1_bi]),
	bounded::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, bounded::tuple<empty>>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_tuple_empty_tuple_empty

static_assert(std::is_empty_v<bounded::tuple<void>>);

static_assert(std::is_empty_v<bounded::tuple<void, void, empty, void>>);

static_assert(sizeof(bounded::tuple<int>) == sizeof(int));
static_assert(sizeof(bounded::tuple<int, bounded::tuple<int>>) == sizeof(int) * 2);

static_assert(sizeof(bounded::tuple<int, empty>) == sizeof(int));

constexpr auto a = bounded::tuple<int, empty, int>(0, empty{}, 5);

static_assert(a[0_bi] == 0);

static_assert(std::is_same_v<decltype(a[1_bi]), empty const &>);

static_assert(std::is_same_v<decltype(bounded::tuple<int>{}[0_bi]), int &&>);
static_assert(std::is_same_v<decltype(bounded::tuple<empty>{}[0_bi]), empty &&>);

struct different_empty {};
static_assert(std::is_empty_v<bounded::tuple<empty, different_empty>>);
using nested_t = bounded::tuple<bounded::tuple<empty, different_empty>>;
constexpr auto nested = nested_t{};
static_assert(std::is_same_v<decltype(nested[0_bi][0_bi]), empty const &>);
static_assert(std::is_same_v<decltype(nested[0_bi][1_bi]), different_empty const &>);

static_assert(!indexable_by<nested_t, bounded::constant_t<1>>);

constexpr bounded::tuple<int, char const *> b = { 2, "Hello" };
static_assert(b[0_bi] == b[0_bi]);

struct non_movable {
	non_movable() = default;
	non_movable(non_movable const &) = delete;
	non_movable(non_movable &&) = delete;
};

constexpr auto operator<=>(non_movable const &, non_movable const &) {
	return std::strong_ordering::equal;
}
constexpr auto operator==(non_movable const &, non_movable const &) {
	return true;
}

static_assert(std::is_constructible_v<bounded::tuple<non_movable>>);


constexpr auto non_movable_constructor = []{ return non_movable(); };
static_assert(non_movable_constructor() == non_movable());
static_assert(bounded::detail::is_constructible<non_movable, non_movable>);
static_assert(bounded::detail::tuple_value<0, non_movable>(bounded::lazy_init, non_movable_constructor)[0_bi] == non_movable());
static_assert(bounded::detail::is_constructible<bounded::tuple<non_movable>, bounded::lazy_init_t, decltype(non_movable_constructor)>);
static_assert(std::is_same_v<bounded::tuple_element<0, bounded::tuple<non_movable>>, non_movable>);
constexpr bounded::tuple<non_movable> non_movable_tuple = {};
static_assert(non_movable_tuple[0_bi] == non_movable{});

static_assert(a == a);
static_assert(b == b);
static_assert(non_movable_tuple == non_movable_tuple);
static_assert(a <= a);
static_assert(b <= b);
static_assert(non_movable_tuple <= non_movable_tuple);

constexpr auto d = bounded::tuple(0, empty{}, 4);
static_assert(a != d);
static_assert(a > d);

constexpr struct {
	constexpr auto operator()(float const x) const {
		return static_cast<int>(x + 1);
	}
	constexpr auto operator()(float const x, float const y) const {
		return static_cast<int>(x + y);
	}
	constexpr auto operator()(int const x) const {
		return static_cast<float>(x + 1);
	}
	constexpr auto operator()(int const x, int const y) const {
		return static_cast<float>(x + y);
	}
} increment_and_swap_types{};

constexpr auto input_tuple = bounded::make_tuple(0, 1.0F, 2, 3.0F, 4);
constexpr auto type_swapped = bounded::transform(increment_and_swap_types, input_tuple);
static_assert(std::is_same_v<decltype(type_swapped), bounded::tuple<float, int, float, int, float> const>);
static_assert(type_swapped == bounded::make_tuple(1.0F, 2, 3.0F, 4, 5.0F));
constexpr auto merged = bounded::transform(increment_and_swap_types, input_tuple, input_tuple);
static_assert(std::is_same_v<decltype(merged), bounded::tuple<float, int, float, int, float> const>);
static_assert(merged == bounded::make_tuple(0.0F, 2, 4.0F, 6, 8.0F));


}	// namespace
