// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/common_type.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/policy/common_policy.hpp>
#include <utility>

namespace bounded {
namespace detail {

template<typename T, typename U>
struct common_cv_type {
private:
	using const_type = std::conditional_t<std::is_const<U>::value, std::add_const_t<T>, T>;
public:
	using type = std::conditional_t<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<U>>::value,
		std::conditional_t<std::is_volatile<U>::value, std::add_volatile_t<const_type>, const_type>,
		std::common_type_t<std::decay_t<T>, std::decay_t<U>>
	>;
};
template<typename T, typename U>
using common_cv_type_t = typename common_cv_type<T, U>::type;


template<typename Target, typename Source0, typename Source1>
struct add_common_cv_reference {
private:
	using CVTarget = common_cv_type_t<common_cv_type_t<Target, std::remove_reference_t<Source0>>, std::remove_reference_t<Source1>>;

	static constexpr bool same_base_type = std::is_same<std::decay_t<Source0>, std::decay_t<Source1>>::value;
	static constexpr bool rvalue_reference =
		std::is_rvalue_reference<Source0>::value and
		std::is_rvalue_reference<Source1>::value and
		same_base_type;
	static constexpr bool lvalue_reference =
		std::is_lvalue_reference<Source0>::value and
		std::is_lvalue_reference<Source1>::value and
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

template<typename Target, typename Source0, typename Source1>
using add_common_cv_reference_t = typename add_common_cv_reference<Target, Source0, Source1>::type;

template<typename... Ts>
struct common_type_and_value_category;

template<typename... Ts>
using common_type_and_value_category_t = typename common_type_and_value_category<Ts...>::type;

template<typename T0>
struct common_type_and_value_category<T0> {
	using type = T0;
};

template<typename T0, typename T1>
struct common_type_and_value_category<T0, T1> {
	using type = std::remove_cv_t<
		detail::add_common_cv_reference_t<
			std::common_type_t<std::decay_t<T0>, std::decay_t<T1>>,
			T0,
			T1
		>
	>;
};

template<typename T0, typename T1, typename... Ts>
struct common_type_and_value_category<T0, T1, Ts...> {
	using type = common_type_and_value_category_t<common_type_and_value_category_t<T0, T1>, Ts...>;
};

}	// namespace detail
}	// namespace bounded
