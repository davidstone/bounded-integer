// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.common_type_and_value_category;

import std_module;

namespace bounded {

template<typename Target, typename... Sources>
using common_const = std::conditional_t<(... or std::is_const_v<Sources>), Target const, Target>;
template<typename Target, typename... Sources>
using common_volatile = std::conditional_t<(... or std::is_volatile_v<Sources>), Target volatile, Target>;
template<typename Target, typename... Sources>
using common_cv = common_volatile<common_const<Target, Sources...>, Sources...>;

template<typename Target, typename... Ts>
struct add_common_cv_reference_impl {
private:
	using target_cv = common_cv<Target, std::remove_reference_t<Ts>...>;
	static constexpr auto same_base_type = (... and std::same_as<Target, std::decay_t<Ts>>);
	static constexpr auto all_rvalue_references = (... and std::is_rvalue_reference_v<Ts>);
	static constexpr auto all_lvalue_references = (... and std::is_lvalue_reference_v<Ts>);
public:
	// This isn't perfect. A type could have a perfectly safe conversion to a
	// reference type defined, and this will still select the fallback prvalue
	// option. However, it should be safe in every case while still being
	// straightforward.
	using type =
		std::conditional_t<same_base_type and all_rvalue_references, std::add_rvalue_reference_t<target_cv>,
		std::conditional_t<same_base_type and all_lvalue_references, std::add_lvalue_reference_t<target_cv>,
		Target
	>>;
};

export template<typename Target, typename... Ts>
using add_common_cv_reference = typename add_common_cv_reference_impl<Target, Ts...>::type;

export template<typename... Ts>
using common_type_and_value_category = add_common_cv_reference<std::common_type_t<Ts...>, Ts...>;

} // namespace bounded


static_assert(std::same_as<
	bounded::common_cv<int>,
	int
>);

static_assert(std::same_as<
	bounded::common_cv<int, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const>,
	int const
>);

static_assert(std::same_as<
	bounded::common_cv<int, int volatile>,
	int volatile
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const volatile>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv<int, int, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const, int>,
	int const
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const, int volatile>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const, int const>,
	int const
>);

static_assert(std::same_as<
	bounded::common_cv<int, int const volatile, int>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv<int, int, int const volatile>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv<long, int const, int>,
	long const
>);


static_assert(std::same_as<
	bounded::common_type_and_value_category<int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const>,
	int
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const, int const>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, int const &>,
	int const &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int const &>,
	int const &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int const &>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int &>,
	int &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int &>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int &&>,
	int &&
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long &&>,
	long
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long const &, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long const &, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int &&>,
	long
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, int, int>,
	int
>);
