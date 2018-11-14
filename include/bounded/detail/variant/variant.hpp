// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/special_member_functions.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <cassert>
#include <type_traits>

namespace bounded {

template<typename GetFunction, typename... Ts>
struct basic_variant :
	private detail::variant_copy_assignment<GetFunction, Ts...>
{
private:
	using base = detail::variant_copy_assignment<GetFunction, Ts...>;
public:
	static_assert(std::is_trivially_copyable<GetFunction>{});

	using base::base;

	constexpr basic_variant(basic_variant const &) = default;
	constexpr basic_variant(basic_variant &&) = default;
	constexpr auto operator=(basic_variant const &) & -> basic_variant & = default;
	constexpr auto operator=(basic_variant &&) & -> basic_variant & = default;
	template<typename T>
	constexpr auto operator=(T && value) & BOUNDED_NOEXCEPT_REF(
		(this->assignment(BOUNDED_FORWARD(value)), *this)
	)
	
	using base::index;
	using base::operator[];
	using base::emplace;
};

template<typename GetFunction, typename... Ts, typename T>
constexpr auto holds_alternative(basic_variant<GetFunction, Ts...> const & variant, detail::types<T> type) BOUNDED_NOEXCEPT_DECLTYPE(
	variant.index() == detail::get_index(type, detail::types<Ts>{}...)
)

namespace detail {

struct equality_visitor {
	template<typename T, auto n>
	constexpr auto operator()(visitor_parameter<T, n> const lhs, visitor_parameter<T, n> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		lhs.value == rhs.value
	)
	template<typename LHS, auto lhs_n, typename RHS, auto rhs_n>
	constexpr auto operator()(visitor_parameter<LHS, lhs_n>, visitor_parameter<RHS, rhs_n>) noexcept {
		return false;
	}
};

} // namespace detail

template<typename GetFunction, typename... Ts>
constexpr auto operator==(basic_variant<GetFunction, Ts...> const & lhs, basic_variant<GetFunction, Ts...> const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	visit_with_index(lhs, rhs, detail::equality_visitor{})
)


namespace detail {

template<std::size_t size>
struct variant_selector {
	template<auto n>
	constexpr explicit variant_selector(constant_t<n> index_) noexcept:
		index(index_)
	{
	}
	template<typename Union>
	constexpr auto operator()(Union const &) const noexcept {
		return index;
	}
private:
	integer<0, size - 1> index;
};

template<>
struct variant_selector<0> {
};

}	// namespace detail

template<typename... Ts>
using variant = basic_variant<detail::variant_selector<sizeof...(Ts)>, Ts...>;


}	// namespace bounded
