// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.add_common_cv_reference;

import std_module;

namespace bounded {

template<typename T, typename U>
struct common_cv_type_impl {
private:
	using const_type = std::conditional_t<std::is_const_v<U>, std::add_const_t<T>, T>;
public:
	using type = std::conditional_t<std::same_as<std::remove_cv_t<T>, std::remove_cv_t<U>>,
		std::conditional_t<std::is_volatile_v<U>, std::add_volatile_t<const_type>, const_type>,
		std::common_type_t<std::decay_t<T>, std::decay_t<U>>
	>;
};
template<typename T, typename U>
using common_cv_type = typename common_cv_type_impl<T, U>::type;


template<typename Target, typename Source0, typename Source1>
struct add_common_cv_reference_impl {
private:
	using CVTarget = common_cv_type<common_cv_type<Target, std::remove_reference_t<Source0>>, std::remove_reference_t<Source1>>;

	static constexpr bool same_base_type = std::same_as<std::decay_t<Source0>, std::decay_t<Source1>>;
	static constexpr bool rvalue_reference =
		std::is_rvalue_reference_v<Source0> and
		std::is_rvalue_reference_v<Source1> and
		same_base_type;
	static constexpr bool lvalue_reference =
		std::is_lvalue_reference_v<Source0> and
		std::is_lvalue_reference_v<Source1> and
		same_base_type;
public:
	// This isn't perfect. A type could have a perfectly safe conversion to a
	// reference type defined, and this will still select the fallback prvalue
	// option. However, it should be safe in every case while still being
	// straightforward.
	using type =
		std::conditional_t<rvalue_reference, CVTarget &&,
		std::conditional_t<lvalue_reference, CVTarget &,
		Target
	>>;
};

export template<typename Target, typename Source0, typename Source1>
using add_common_cv_reference = typename add_common_cv_reference_impl<Target, Source0, Source1>::type;

} // namespace bounded

namespace {

static_assert(std::same_as<
	bounded::common_cv_type<int, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_cv_type<int const, int>,
	int const
>);

static_assert(std::same_as<
	bounded::common_cv_type<int, int volatile>,
	int volatile
>);

static_assert(std::same_as<
	bounded::common_cv_type<int const volatile, int>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv_type<int volatile, int const volatile>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv_type<int volatile, int const>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::common_cv_type<int const volatile, int const volatile>,
	int const volatile
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int, int, int>,
	int
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int const, int, int>,
	int const
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int const, int, int volatile>,
	int const
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int, int &, int &>,
	int &
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int const, int &, int &>,
	int const &
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int, int const &, int &>,
	int const &
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int const, int volatile &, int &>,
	int const volatile &
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<int, int const &, int volatile &>,
	int const volatile &
>);

static_assert(std::same_as<
	bounded::add_common_cv_reference<long, int, int>,
	long
>);

} // namespace
