// Adds value category (reference qualifier) to std::common_type
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "common_type.hpp"
#include "forward_declaration.hpp"
#include "numeric_limits.hpp"
#include "overlapping_range.hpp"
#include "operators/comparison.hpp"
#include "policy/common_policy.hpp"
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
