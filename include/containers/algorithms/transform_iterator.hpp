// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_adapter.hpp>
#include <containers/reference_wrapper.hpp>

#include <functional>

namespace containers {
namespace detail {

// TODO: [[no_unique_address]]
// TODO: std::invoke

template<typename T>
constexpr auto is_reference_wrapper = false;

template<typename T>
constexpr auto is_reference_wrapper<reference_wrapper<T>> = true;

template<typename T>
constexpr auto is_reference_wrapper<std::reference_wrapper<T>> = true;

template<typename T, bool should_derive = std::is_class_v<T> and !std::is_final_v<T> and std::is_empty_v<T>>
struct function_wrapper;

template<typename F>
struct function_wrapper<F, true> : F {
	using F::operator();
};

template<typename F>
struct function_wrapper<F, false> {
	F function;

	template<typename... Args>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		  function(BOUNDED_FORWARD(args)...)
	)
};

template<typename Result, typename Type>
struct function_wrapper<Result Type::*, false> {
	Result Type::* function;

	template<typename T1, typename... Args>
	constexpr auto operator()(T1 && t1, Args && ... args) const noexcept(std::is_nothrow_invocable_v<decltype(function), T1, Args...>) -> std::invoke_result_t<decltype(function), T1, Args...> {
		if constexpr (is_reference_wrapper<std::decay_t<T1>>) {
			return operator()(function, t1.get(), BOUNDED_FORWARD(args)...);
		} else if constexpr (std::is_member_function_pointer_v<decltype(function)>) {
			if constexpr (std::is_base_of_v<Type, std::decay_t<T1>>) {
				return (BOUNDED_FORWARD(t1).*function)(BOUNDED_FORWARD(args)...);
			} else {
				return ((*BOUNDED_FORWARD(t1)).*function)(BOUNDED_FORWARD(args)...);
			}
		} else {
			static_assert(std::is_member_object_pointer_v<decltype(function)>);
			static_assert(sizeof...(args) == 0);
			if constexpr (std::is_base_of_v<Type, std::decay_t<T1>>) {
				return BOUNDED_FORWARD(t1).*function;
			} else {
				return (*BOUNDED_FORWARD(t1)).*function;
			}
		}
	}
};

template<typename UnaryFunction>
struct transform_traits : default_add, default_subtract, default_compare, private function_wrapper<UnaryFunction> {
	transform_traits() = default;
	constexpr explicit transform_traits(UnaryFunction dereference):
		function_wrapper<UnaryFunction>{std::move(dereference)}
	{
	}
	
	template<typename Iterator>
	constexpr auto dereference(Iterator const it) const
		noexcept(noexcept(std::declval<function_wrapper<UnaryFunction> const &>()(*it)))
		-> decltype(std::declval<function_wrapper<UnaryFunction> const &>()(*it))
	{
		return static_cast<function_wrapper<UnaryFunction> const &>(*this)(*it);
	}
};

template<typename UnaryFunction>
struct transform_traits_dereference : default_add, default_subtract, default_compare, private function_wrapper<UnaryFunction> {
	constexpr explicit transform_traits_dereference(UnaryFunction dereference):
		function_wrapper<UnaryFunction>{std::move(dereference)}
	{
	}
	
	template<typename Iterator>
	constexpr auto dereference(Iterator const it) const
		noexcept(noexcept(std::declval<function_wrapper<UnaryFunction> const &>()(it)))
		-> decltype(std::declval<function_wrapper<UnaryFunction> const &>()(it))
	{
		return static_cast<function_wrapper<UnaryFunction> const &>(*this)(it);
	}
};

}	// namespace detail

template<typename Iterator, typename UnaryFunction>
constexpr auto transform_iterator(Iterator it, UnaryFunction dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits(std::move(dereference)));
}

template<typename Iterator, typename UnaryFunction>
constexpr auto transform_iterator_dereference(Iterator it, UnaryFunction dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits_dereference(std::move(dereference)));
}

}	// namespace containers
