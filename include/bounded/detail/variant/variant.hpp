// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/copy_cv_ref.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/special_member_functions.hpp>
#include <bounded/detail/variant/visit.hpp>

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

	template<typename T, typename std::enable_if<!std::is_same_v<std::decay_t<T>, basic_variant>, int>::type = 0>
	constexpr auto & operator=(T && value) & {
		this->assignment(BOUNDED_FORWARD(value));
		return *this;
	}
	
	using base::index;
	using base::operator[];
	using base::emplace;
};

template<typename GetFunction, typename... Ts, typename T>
constexpr auto holds_alternative(basic_variant<GetFunction, Ts...> const & variant, detail::types<T> type) {
	return variant.index() == detail::get_index(type, detail::types<Ts>{}...);
}

namespace detail {

struct equality_visitor {
	template<typename T, auto n>
	constexpr auto operator()(visitor_parameter<T, n> const lhs, visitor_parameter<T, n> const rhs) const {
		return lhs.value == rhs.value;
	}
	template<typename LHS, auto lhs_n, typename RHS, auto rhs_n> requires(lhs_n != rhs_n)
	constexpr auto operator()(visitor_parameter<LHS, lhs_n>, visitor_parameter<RHS, rhs_n>) const {
		return false;
	}
};

} // namespace detail

template<typename GetFunction, equality_comparable... Ts>
constexpr auto operator==(basic_variant<GetFunction, Ts...> const & lhs, basic_variant<GetFunction, Ts...> const & rhs) {
	return visit_with_index(lhs, rhs, detail::equality_visitor{});
}


namespace detail {

template<std::size_t size>
struct variant_selector {
	template<auto n>
	constexpr explicit variant_selector(constant_t<n> index_):
		index(index_)
	{
	}
	template<typename Union>
	constexpr auto operator()(Union const &) const {
		return index;
	}
private:
	integer<0, normalize<size - 1U>> index;
};

template<>
struct variant_selector<0> {
	variant_selector() = delete;
};

}	// namespace detail

template<typename... Ts>
using variant = basic_variant<detail::variant_selector<sizeof...(Ts)>, Ts...>;


}	// namespace bounded
