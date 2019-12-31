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
inline constexpr auto is_reference_wrapper = false;

template<typename T>
inline constexpr auto is_reference_wrapper<reference_wrapper<T>> = true;

template<typename T>
inline constexpr auto is_reference_wrapper<std::reference_wrapper<T>> = true;

// TODO: try having a conversion operator for function pointers?
template<typename Function>
struct function_wrapper {
	Function function;

	// TODO: Use terse syntax when clang doesn't crash
	template<typename... Args>
	constexpr auto operator()(Args && ... args) const OPERATORS_RETURNS(
		  function(BOUNDED_FORWARD(args)...)
	)
};

template<typename Result, typename Type>
struct function_wrapper<Result Type::*> {
	Result Type::* function;

	constexpr auto operator()(auto && object, auto && ... args) const {
		using Object = std::decay_t<decltype(object)>;
		if constexpr (is_reference_wrapper<Object>) {
			return operator()(function, object.get(), BOUNDED_FORWARD(args)...);
		} else if constexpr (std::is_member_function_pointer_v<decltype(function)>) {
			if constexpr (std::is_base_of_v<Type, Object>) {
				return (BOUNDED_FORWARD(object).*function)(BOUNDED_FORWARD(args)...);
			} else {
				return (BOUNDED_FORWARD(object)->*function)(BOUNDED_FORWARD(args)...);
			}
		} else {
			static_assert(std::is_member_object_pointer_v<decltype(function)>);
			static_assert(sizeof...(args) == 0);
			if constexpr (std::is_base_of_v<Type, Object>) {
				return BOUNDED_FORWARD(object).*function;
			} else {
				return BOUNDED_FORWARD(object)->*function;
			}
		}
	}
};

template<typename Function> requires(std::is_class_v<Function> and !std::is_final_v<Function> and std::is_empty_v<Function>)
struct function_wrapper<Function> : Function {
	using Function::operator();
};

template<typename UnaryFunction>
struct transform_traits : default_begin_end, default_add, default_subtract, default_compare, private function_wrapper<UnaryFunction> {
	transform_traits() = default;
	constexpr explicit transform_traits(UnaryFunction dereference_):
		function_wrapper<UnaryFunction>{std::move(dereference_)}
	{
	}
	
	constexpr auto dereference(iterator auto const it) const -> decltype(std::declval<function_wrapper<UnaryFunction> const &>()(*it)) {
		return static_cast<function_wrapper<UnaryFunction> const &>(*this)(*it);
	}
};

template<typename UnaryFunction>
struct transform_traits_dereference : default_add, default_subtract, default_compare, private function_wrapper<UnaryFunction> {
	constexpr explicit transform_traits_dereference(UnaryFunction dereference_):
		function_wrapper<UnaryFunction>{std::move(dereference_)}
	{
	}
	
	constexpr auto dereference(iterator auto const it) const -> decltype(std::declval<function_wrapper<UnaryFunction> const &>()(it)) {
		return static_cast<function_wrapper<UnaryFunction> const &>(*this)(it);
	}
};

}	// namespace detail

constexpr auto transform_iterator(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits(std::move(dereference)));
}

constexpr auto transform_iterator_dereference(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits_dereference(std::move(dereference)));
}

}	// namespace containers
