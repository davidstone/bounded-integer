// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/tuple.hpp>
#include <containers/type.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

// TODO: implement as range_adapter so that iterators remain cheap to copy
namespace containers {
namespace detail {

template<typename Iterator, typename Enable = void>
struct operator_arrow {
};

template<typename Iterator>
struct operator_arrow<Iterator, void_t<decltype(::bounded::addressof(*std::declval<Iterator const &>()))>> {
	constexpr auto operator->() const BOUNDED_NOEXCEPT_VALUE(
		::bounded::addressof(*static_cast<Iterator const &>(*this))
	)
};

#if 0
// Another possible implementation of operator-> is as follows, which works with
// any iterator, even those that return by value

struct temporary_operator_arrow {
	using reference = decltype(*std::declval<iterator_adapter_t const &>()) &&;
	constexpr explicit temporary_operator_arrow(reference value) noexcept:
		m_value(std::forward<reference>(value))
	{
	}
	constexpr auto operator->() && noexcept {
		return ::bounded::addressof(m_value);
	}

private:
	reference m_value;
};

constexpr auto operator->() const BOUNDED_NOEXCEPT(
	temporary_operator_arrow(**this)
)

// However, this has the nasty effect of silently breaking lifetime extension
// with references. In other words, if *it returns a prvalue instead of a
// glvalue (by value instead of by reference), the following code does not work:

auto && ref = it->x;
use(x);

// But the following code does:

use(it->x);

// This is fragile behavior, so for now, disable the operator-> overload if *it
// is a prvalue.
#endif

template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
struct iterator_adapter_t : operator_arrow<iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction>> {
	static_assert(
		std::is_empty<SubtractFunction>::value,
		"SubtractFunction must be a stateless function. Would we use the state from the left- or right-hand side?"
	);
	static_assert(
		std::is_empty<LessFunction>::value,
		"LessFunction must be a stateless comparison function. Would we use the state from the left- or right-hand side?"
	);
	using value_type = decltype(std::declval<DereferenceFunction>()(std::declval<Iterator>()));
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = IteratorCategory;

	// Not sure what these actually mean...
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = std::conditional_t<std::is_reference<value_type>::value, value_type, value_type &>;

	// Even though subtract and less must be stateless, we must still keep them
	// around. This is because there is no guarantee that they are
	// default-constructible: for instance, lambdas.
	constexpr iterator_adapter_t(Iterator it, DereferenceFunction dereference, AddFunction add, SubtractFunction subtract, LessFunction less) noexcept(std::is_nothrow_constructible<decltype(m_data), Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction>::value):
		m_data(std::move(it), std::move(dereference), std::move(add), std::move(subtract), std::move(less))
	{
	}
	
	constexpr auto base() const noexcept {
		return m_data[0_bi];
	}

	constexpr auto && dereference() const & noexcept {
		return m_data[1_bi];
	}
	constexpr auto && dereference() & noexcept {
		return m_data[1_bi];
	}
	constexpr auto && dereference() && noexcept {
		return std::move(m_data)[1_bi];
	}
	constexpr auto && add() const & noexcept {
		return m_data[2_bi];
	}
	constexpr auto && add() & noexcept {
		return m_data[2_bi];
	}
	constexpr auto && add() && noexcept {
		return std::move(m_data)[2_bi];
	}
	constexpr auto && subtract() const & noexcept {
		return m_data[3_bi];
	}
	constexpr auto && subtract() & noexcept {
		return m_data[3_bi];
	}
	constexpr auto && subtract() && noexcept {
		return std::move(m_data)[3_bi];
	}
	constexpr auto && less() const & noexcept {
		return m_data[4_bi];
	}
	constexpr auto && less() & noexcept {
		return m_data[4_bi];
	}
	constexpr auto && less() && noexcept {
		return std::move(m_data)[4_bi];
	}
	
	template<typename Index>
	constexpr auto operator[](Index const index) const BOUNDED_NOEXCEPT_DECLTYPE(
		*(*this + index)
	)

private:
	tuple<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> m_data;
};


template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator*(iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & it) BOUNDED_NOEXCEPT_DECLTYPE(
	it.dereference()(it.base())
)

template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction, typename Offset>
constexpr auto operator+(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> lhs,
	Offset const rhs
) BOUNDED_NOEXCEPT_VALUE(
	// Use {} initialization to ensure evaluation order
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction>{
		lhs.add()(lhs.base(), rhs),
		std::move(lhs).dereference(),
		std::move(lhs).add(),
		std::move(lhs).subtract(),
		std::move(lhs).less()
	}
)


template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator-(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.subtract()(lhs.base(), rhs.base())
)


template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator==(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.base() == rhs.base()
)
template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction, typename RHS>
constexpr auto operator==(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	RHS const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.base() == rhs
)
template<typename LHS, typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator==(
	LHS const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs == rhs.base()
)

template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator<(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.less()(lhs.base(), rhs.base())
)


struct default_dereference {
	template<typename Iterator>
	constexpr auto operator()(Iterator const & it) const BOUNDED_NOEXCEPT_DECLTYPE(
		*it
	)
};

struct default_add {
	template<typename Iterator, typename Offset>
	constexpr auto operator()(Iterator const & it, Offset const & offset) const BOUNDED_NOEXCEPT_DECLTYPE(
		it + offset
	)
};

struct default_subtract {
	template<typename Iterator>
	constexpr auto operator()(Iterator const & lhs, Iterator const & rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		lhs - rhs
	)
};

struct default_less {
	template<typename Iterator>
	constexpr auto operator()(Iterator const & lhs, Iterator const & rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		lhs < rhs
	)
};

struct not_a_function {};

template<typename IteratorCategory, typename Iterator>
using get_iterator_category = std::conditional_t<std::is_void<IteratorCategory>::value, typename std::iterator_traits<std::decay_t<Iterator>>::iterator_category, IteratorCategory>;

}	// namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Add: it + 5_bi
// 3) Subtract: it1 - it2
// 4) Less than: it1 < it2
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these four behaviors. Adding,
// subtracting, and comparing are all linked to the same algorithm, but
// unfortunately, there does not appear to be a way to provide a function that
// unifies these operations.

// See bounded::make to understand the argument defaulting

template<typename IteratorCategory = void, typename Iterator = void, typename DereferenceFunction = void, typename AddFunction = void, typename SubtractFunction = void, typename LessFunction = void>
constexpr auto iterator_adapter(Iterator && it, DereferenceFunction && dereference, AddFunction && add, SubtractFunction && subtract, LessFunction && less) BOUNDED_NOEXCEPT_VALUE(
	detail::iterator_adapter_t<
		detail::get_iterator_category<IteratorCategory, Iterator>,
		std::decay_t<Iterator>,
		std::decay_t<DereferenceFunction>,
		std::decay_t<AddFunction>,
		std::decay_t<SubtractFunction>,
		std::decay_t<LessFunction>
	>(
		std::forward<Iterator>(it),
		std::forward<DereferenceFunction>(dereference),
		std::forward<AddFunction>(add),
		std::forward<SubtractFunction>(subtract),
		std::forward<LessFunction>(less)
	)
)

template<typename IteratorCategory = void, typename Iterator = void, typename DereferenceFunction = void>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction && dereference) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter<IteratorCategory>(
		it,
		std::forward<DereferenceFunction>(dereference),
		detail::default_add{},
		detail::default_subtract{},
		detail::default_less{}
	)
)

}	// namespace containers
