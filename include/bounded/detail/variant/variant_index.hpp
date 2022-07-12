// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/type.hpp>
#include <bounded/detail/variant/get_index.hpp>

#include <cstddef>
#include <concepts>
#include <utility>

namespace bounded {
namespace detail {

struct non_constructible {
	non_constructible() = delete;
};

template<std::size_t size>
struct variant_selector_c {
	using type = integer<0, normalize<size - 1U>>;
};

template<>
struct variant_selector_c<0> {
	using type = non_constructible;
};

template<std::size_t size>
using variant_selector = typename variant_selector_c<size>::type;

} // namespace detail

template<typename... Ts>
struct variant_index {
	using integer_index = detail::variant_selector<sizeof...(Ts)>;
	constexpr explicit variant_index(integer_index const index):
		m_index(index)
	{
	}
	constexpr auto integer() const -> integer_index {
		return m_index;
	}
	friend constexpr auto operator==(variant_index, variant_index) -> bool = default;
	friend constexpr auto operator==(variant_index const lhs, integer_index const rhs) -> bool {
		return lhs.integer() == rhs;
	}
	template<typename T> requires (... or std::same_as<T, Ts>)
	friend constexpr auto operator==(variant_index const lhs, types<T> const rhs) -> bool {
		return [=]<std::size_t... indexes>(std::index_sequence<indexes...>) {
			bool matched = false;
			(..., (bounded::constant<indexes> == lhs.integer() ?
				void(matched = (detail::get_type(bounded::constant<indexes>, types<Ts>()...) == rhs)) :
				void()
			));
			return matched;
		}(std::make_index_sequence<sizeof...(Ts)>());
	}
private:
	[[no_unique_address]] integer_index m_index;
};

} // namespace bounded
