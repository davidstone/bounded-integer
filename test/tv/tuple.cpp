// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.tuple;

import tv.tuple;

import bounded;
import std_module;

using namespace bounded::literal;

namespace {

static_assert(std::is_empty_v<tv::tuple<>>);
static_assert(tv::tuple() == tv::tuple());

struct empty {
	friend auto operator<=>(empty, empty) = default;
};

template<typename T, typename Index>
concept indexable_by = requires(T t, Index index) { t[index]; };

template<typename T, auto index>
concept constant_indexable_by = indexable_by<T, bounded::constant_t<index>>;

template<typename T, typename Index>
concept type_indexable_by = indexable_by<T, bounded::type_t<Index>>;


namespace tuple_empty {

using type = tv::tuple<empty>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(bounded::trivially_default_constructible<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::same_as<
	decltype(type()[bounded::type<empty>]),
	empty &&
>);

} // namespace tuple_empty

namespace tuple_empty_empty {

using type = tv::tuple<empty, empty>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(bounded::trivially_default_constructible<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_empty_empty

namespace tuple_tuple {

using type = tv::tuple<tv::tuple<>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(bounded::trivially_default_constructible<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	tv::tuple<> &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::same_as<
	decltype(type()[bounded::type_t<tv::tuple<>>()]),
	tv::tuple<> &&
>);

} // namespace tuple_tuple

namespace tuple_empty_tuple_empty {

using type = tv::tuple<empty, tv::tuple<empty>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(bounded::trivially_default_constructible<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	tv::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(std::same_as<
	decltype(type()[bounded::type<empty>]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[bounded::type_t<tv::tuple<empty>>()]),
	tv::tuple<empty> &&
>);

} // namespace tuple_empty_tuple_empty

namespace tuple_tuple_empty_tuple_empty {

using type = tv::tuple<tv::tuple<empty>, tv::tuple<empty>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(bounded::trivially_default_constructible<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	tv::tuple<empty> &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	tv::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, tv::tuple<empty>>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_tuple_empty_tuple_empty

static_assert(std::is_empty_v<tv::tuple<void>>);

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<tv::tuple<void, void, empty, void>>);
#endif

static_assert(sizeof(tv::tuple<int>) == sizeof(int));
static_assert(sizeof(tv::tuple<int, tv::tuple<int>>) == sizeof(int) * 2);

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(sizeof(tv::tuple<int, empty>) == sizeof(int));
#endif

constexpr auto a = tv::tuple<int, empty, int>(0, empty{}, 5);

static_assert(a[0_bi] == 0);

static_assert(std::same_as<decltype(a[1_bi]), empty const &>);

static_assert(std::same_as<decltype(tv::tuple<int>{}[0_bi]), int &&>);
static_assert(std::same_as<decltype(tv::tuple<empty>{}[0_bi]), empty &&>);

struct different_empty {};
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<tv::tuple<empty, different_empty>>);
#endif
using nested_t = tv::tuple<tv::tuple<empty, different_empty>>;
constexpr auto nested = nested_t{};
static_assert(std::same_as<decltype(nested[0_bi][0_bi]), empty const &>);
static_assert(std::same_as<decltype(nested[0_bi][1_bi]), different_empty const &>);

static_assert(!indexable_by<nested_t, bounded::constant_t<1>>);

constexpr tv::tuple<int, char const *> b = { 2, "Hello" };
static_assert(b[0_bi] == b[0_bi]);

struct non_movable {
	non_movable() = default;
	non_movable(non_movable const &) = delete;
	non_movable(non_movable &&) = delete;

	friend auto operator<=>(non_movable const &, non_movable const &) = default;
};

static_assert(bounded::constructible_from<tv::tuple<non_movable>>);


constexpr auto non_movable_constructor = []{ return non_movable(); };
static_assert(non_movable_constructor() == non_movable());
static_assert(bounded::constructible_from<non_movable, non_movable>);
static_assert(bounded::constructible_from<tv::tuple<non_movable>, bounded::lazy_init_t, decltype(non_movable_constructor)>);
static_assert(std::same_as<tv::tuple_element<0, tv::tuple<non_movable>>, non_movable>);
constexpr tv::tuple<non_movable> non_movable_tuple = {};
static_assert(non_movable_tuple[0_bi] == non_movable{});

static_assert(a == a);
static_assert(b == b);
static_assert(non_movable_tuple == non_movable_tuple);
static_assert(a <= a);
static_assert(b <= b);
static_assert(non_movable_tuple <= non_movable_tuple);

constexpr auto d = tv::tuple(0, empty{}, 4);
static_assert(a != d);
static_assert(a > d);

} // namespace