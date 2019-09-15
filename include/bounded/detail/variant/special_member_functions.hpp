// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/variant/base.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

// These types exist because the special member functions cannot be templates.
// Therefore, they cannot be easily constrained as member functions. Instead we
// use the CRTP pattern to define them in these classes.


template<bool generate_destructor, typename GetFunction, typename... Ts>
struct variant_destructor_impl;

template<typename GetFunction, typename... Ts>
using variant_destructor = variant_destructor_impl<
	not (... and std::is_trivially_destructible_v<Ts>),
	GetFunction,
	Ts...
>;

template<typename GetFunction, typename... Ts>
struct variant_destructor_impl<false, GetFunction, Ts...> : basic_variant_base<GetFunction, Ts...>
{
	using basic_variant_base<GetFunction, Ts...>::basic_variant_base;
};


template<typename GetFunction, typename... Ts>
struct variant_destructor_impl<true, GetFunction, Ts...> : basic_variant_base<GetFunction, Ts...>
{
	using basic_variant_base<GetFunction, Ts...>::basic_variant_base;
	~variant_destructor_impl() {
		visit(*this, destroy);
	}
};



template<bool generate_constructor, template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_constructor_impl;

// Trivially copyable types do not need the copy / move defined specially, as
// they get a compiler generated constructor that does the right thing
template<template<typename> typename AddReference, typename Base, typename... Ts>
using variant_copy_move_constructor = variant_copy_move_constructor_impl<
	(... and std::is_constructible_v<Ts, AddReference<Ts>>) and not (... and std::is_trivially_constructible_v<Ts, AddReference<Ts>>),
	AddReference,
	Base,
	Ts...
>;




template<template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_constructor_impl<false, AddReference, Base, Ts...> : Base
{
	using Base::Base;
};



template<template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_constructor_impl<true, AddReference, Base, Ts...> : Base
{
	using Base::Base;

	constexpr variant_copy_move_constructor_impl(AddReference<variant_copy_move_constructor_impl> other)
		noexcept((... and std::is_nothrow_constructible_v<Ts, AddReference<Ts>>)):
		Base(visit_with_index(
			BOUNDED_FORWARD(other),
			[&](auto parameter) {
				return Base(
					other.get_function(),
					parameter.index,
					std::move(parameter).value
				);
			}
		))
	{
	}
};



template<typename GetFunction, typename... Ts>
using variant_move_constructor = variant_copy_move_constructor<
	std::add_rvalue_reference_t,
	variant_destructor<GetFunction, Ts...>,
	Ts...
>;

template<typename T>
using add_const_lvalue_reference_t = T const &;

template<typename GetFunction, typename... Ts>
using variant_copy_constructor = variant_copy_move_constructor<
	add_const_lvalue_reference_t,
	variant_move_constructor<GetFunction, Ts...>,
	Ts...
>;



template<bool generate_assignment, template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_assignment_impl;

template<template<typename> typename AddReference, typename Base, typename... Ts>
using variant_copy_move_assignment = variant_copy_move_assignment_impl<
	(... and std::is_assignable_v<Ts, AddReference<Ts>>) and not (... and std::is_trivially_assignable_v<Ts, AddReference<Ts>>),
	AddReference,
	Base,
	Ts...
>;




template<template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_assignment_impl<false, AddReference, Base, Ts...> : Base
{
	using Base::Base;
};


template<template<typename> typename AddReference, typename Base, typename... Ts>
struct variant_copy_move_assignment_impl<true, AddReference, Base, Ts...> : Base
{
	using reference = AddReference<variant_copy_move_assignment_impl>;

	using Base::Base;
	
	variant_copy_move_assignment_impl(variant_copy_move_assignment_impl &&) = default;
	variant_copy_move_assignment_impl(variant_copy_move_assignment_impl const &) = default;

	constexpr auto & operator=(reference other) & noexcept(
		(... and std::is_nothrow_constructible_v<Ts, AddReference<Ts>>) and
		(... and std::is_nothrow_assignable_v<Ts, AddReference<Ts>>)
	)
	{
		visit_with_index(
			*this,
			static_cast<reference>(other),
			[&](auto lhs, auto rhs) {
				if constexpr (lhs.index == rhs.index) {
					lhs.value = BOUNDED_FORWARD(rhs.value);
				} else {
					this->emplace(rhs.index, BOUNDED_FORWARD(rhs.value));
				}
				this->get_function() = other.get_function();
			}
		);
		return *this;
	}
};


template<typename GetFunction, typename... Ts>
using variant_move_assignment = variant_copy_move_assignment<
	std::add_rvalue_reference_t,
	variant_copy_constructor<GetFunction, Ts...>,
	Ts...
>;

template<typename GetFunction, typename... Ts>
using variant_copy_assignment = variant_copy_move_assignment<
	add_const_lvalue_reference_t,
	variant_move_assignment<GetFunction, Ts...>,
	Ts...
>;


}	// namespace detail
}	// namespace bounded
