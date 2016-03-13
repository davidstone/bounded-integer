// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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

struct default_dereference {
	template<typename Iterator>
	constexpr auto operator()(Iterator && it) const BOUNDED_NOEXCEPT_DECLTYPE(
		*std::forward<Iterator>(it)
	)
};

struct default_traverse {
	template<typename Iterator, typename Offset>
	constexpr auto operator()(Iterator && it, Offset && offset) const BOUNDED_NOEXCEPT_DECLTYPE(
		std::forward<Iterator>(it) + std::forward<Offset>(offset)
	)
};

}	// namespace detail

// There are two functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Traversing: it + 5_bi
// This allows you to customize those. Your function is passed an iterator.

template<typename Iterator, typename DereferenceFunction, typename TraversalFunction>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference, TraversalFunction traverse) BOUNDED_NOEXCEPT(
	detail::iterator_adapter_t<Iterator, DereferenceFunction, TraversalFunction>(std::move(it), std::move(dereference), std::move(traverse))
)

template<typename Iterator, typename DereferenceFunction>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference) BOUNDED_NOEXCEPT(
	iterator_adapter(it, std::move(dereference), detail::default_traverse{})
)

}	// namespace containers
