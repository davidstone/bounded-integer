// Copyright (C) 2015 David Stone
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

#include <containers/common_iterator_functions.hpp>
#include <containers/index_type.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

// TODO: implement as range_adapter so that iterators remain cheap to copy
namespace containers {
namespace detail {

template<typename Iterator, typename Function>
struct iterator_adapter_t : private std::tuple<Iterator, Function> {
private:
	using base = std::tuple<Iterator, Function>;
public:
	using value_type = decltype(std::declval<Function>()(*std::declval<Iterator>()));
	using difference_type = typename Iterator::difference_type;
	using pointer = typename Iterator::pointer;
	using reference = typename Iterator::reference;
	using iterator_category = typename Iterator::iterator_category;

	using base::base;

	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT(
		function()(*iterator())
	)
	constexpr decltype(auto) operator*() BOUNDED_NOEXCEPT(
		function()(*iterator())
	)
	// operator-> intentionally missing

	friend constexpr auto operator+(iterator_adapter_t lhs, difference_type const rhs) BOUNDED_NOEXCEPT(
		iterator_adapter_t(std::move(lhs.iterator()) + rhs, std::move(lhs.function()))
	)
	friend constexpr decltype(auto) operator-(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.iterator() - rhs.iterator()
	)

	constexpr decltype(auto) operator[](index_type<iterator_adapter_t> const index) const BOUNDED_NOEXCEPT(
		function()(iterator()[index])
	)

	friend constexpr decltype(auto) operator==(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.iterator() == rhs.iterator()
	)
	template<typename RHS>
	friend constexpr decltype(auto) operator==(iterator_adapter_t const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
		lhs.iterator() == rhs
	)
	template<typename LHS>
	friend constexpr decltype(auto) operator==(LHS const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs == rhs.iterator()
	)

	friend constexpr decltype(auto) operator<(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.iterator() < rhs.iterator()
	)
	template<typename RHS>
	friend constexpr decltype(auto) operator<(iterator_adapter_t const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
		lhs.iterator() < rhs
	)
	template<typename LHS>
	friend constexpr decltype(auto) operator<(LHS const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs < rhs.iterator()
	)
private:
	constexpr auto && as_tuple() const noexcept {
		return static_cast<base const &>(*this);
	}
	constexpr auto && as_tuple() noexcept {
		return static_cast<base &>(*this);
	}
	constexpr auto && iterator() const noexcept {
		return std::get<Iterator>(as_tuple());
	}
	constexpr auto && iterator() noexcept {
		return std::get<Iterator>(as_tuple());
	}
	constexpr auto && function() const noexcept {
		return std::get<Function>(as_tuple());
	}
	constexpr auto && function() noexcept {
		return std::get<Function>(as_tuple());
	}
};

}	// namespace detail

template<typename Iterator, typename Function>
constexpr auto iterator_adapter(Iterator it, Function function) BOUNDED_NOEXCEPT(
	detail::iterator_adapter_t<Iterator, Function>(std::move(it), std::move(function))
)

}	// namespace containers
