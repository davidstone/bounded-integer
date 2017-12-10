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

#include <bounded/integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

// TODO: implement as range_adapter so that iterators remain cheap to copy
namespace containers {
namespace detail {

// TODO: Use empty base optimization
template<typename Iterator, typename DereferenceFunction, typename AddFunction>
struct iterator_adapter_t {
	using value_type = decltype(std::declval<DereferenceFunction>()(std::declval<Iterator>()));
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	// Not sure what these actually mean...
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = std::conditional_t<std::is_reference<value_type>::value, value_type, value_type &>;

	constexpr iterator_adapter_t(Iterator it, DereferenceFunction dereference, AddFunction add):
		m_base(it),
		m_dereference(std::move(dereference)),
		m_add(std::move(add))
	{
	}
	
	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT(
		m_dereference(base())
	)
	// operator-> intentionally missing

	// TODO: Support ForwardIterator
	friend constexpr auto operator+(iterator_adapter_t lhs, difference_type const rhs) BOUNDED_NOEXCEPT(
		// Use {} initialization to ensure evaluation order
		iterator_adapter_t{lhs.m_add(lhs.base(), rhs), std::move(lhs.m_dereference), std::move(lhs.m_add)}
	)
	friend constexpr decltype(auto) operator-(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.base() - rhs.base()
	)

	constexpr decltype(auto) operator[](index_type<iterator_adapter_t> const index) const BOUNDED_NOEXCEPT(
		m_dereference(*(*this + index))
	)

	friend constexpr decltype(auto) operator==(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.base() == rhs.base()
	)
	template<typename RHS>
	friend constexpr decltype(auto) operator==(iterator_adapter_t const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
		lhs.base() == rhs
	)
	template<typename LHS>
	friend constexpr decltype(auto) operator==(LHS const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs == rhs.base()
	)

	friend constexpr decltype(auto) operator<(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.base() < rhs.base()
	)
	template<typename RHS>
	friend constexpr decltype(auto) operator<(iterator_adapter_t const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
		lhs.base() < rhs
	)
	template<typename LHS>
	friend constexpr decltype(auto) operator<(LHS const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs < rhs.base()
	)
	
	constexpr auto base() const noexcept {
		return m_base;
	}
private:
	Iterator m_base;
	DereferenceFunction m_dereference;
	AddFunction m_add;
};

template<typename SimpleFunction>
struct dereferenced_function : private SimpleFunction {
	constexpr dereferenced_function(SimpleFunction && base) BOUNDED_NOEXCEPT_INITIALIZATION(
		SimpleFunction(std::move(base))
	) {
	}
	template<typename Iterator>
	constexpr decltype(auto) operator()(Iterator const it) const {
		return static_cast<SimpleFunction const &>(*this)(*it);
	}
};

struct iterator_adapter_increment {
	template<typename Iterator>
	constexpr auto operator()(Iterator const it, typename std::iterator_traits<Iterator>::difference_type const difference) const BOUNDED_NOEXCEPT_DECLTYPE(
		it + difference
	)
};

}	// namespace detail

template<typename Iterator, typename DereferenceFunction, typename AddFunction>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference, AddFunction add) BOUNDED_NOEXCEPT(
	detail::iterator_adapter_t<Iterator, DereferenceFunction, AddFunction>(std::move(it), std::move(dereference), std::move(add))
)

template<typename Iterator, typename Function>
constexpr auto iterator_adapter(Iterator it, Function function) BOUNDED_NOEXCEPT(
	iterator_adapter(it, detail::dereferenced_function<Function>(std::move(function)), detail::iterator_adapter_increment{})
)

}	// namespace containers
