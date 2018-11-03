// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/tuple.hpp>

#include <utility>

namespace bounded {
namespace detail {

// tuple_value is used to have something consistently derivable
template<typename Function>
using function_wrapper = tuple_value<0, Function>;

template<typename Function>
struct overload_impl : private function_wrapper<Function> {
	using function_wrapper<Function>::function_wrapper;
	
	template<typename... Args>
	constexpr auto operator()(Args && ... args) const & BOUNDED_NOEXCEPT_DECLTYPE(
		this->value()(BOUNDED_FORWARD(args)...)
	)
	template<typename... Args>
	constexpr auto operator()(Args && ... args) & BOUNDED_NOEXCEPT_DECLTYPE(
		this->value()(BOUNDED_FORWARD(args)...)
	)
	template<typename... Args>
	constexpr auto operator()(Args && ... args) && BOUNDED_NOEXCEPT_DECLTYPE(
		std::move(*this).value()(BOUNDED_FORWARD(args)...)
	)
};

} // namespace detail

template<typename... Functions>
struct overload : detail::overload_impl<Functions>... {
	constexpr explicit overload(Functions... functions):
		detail::overload_impl<Functions>(detail::not_piecewise_construct, std::move(functions))...
	{
	}
		
	using detail::overload_impl<Functions>::operator()...;
};

} // namespace bounded
