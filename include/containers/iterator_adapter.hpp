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
struct operator_arrow<Iterator, void_t<decltype(std::addressof(*std::declval<Iterator const &>()))>> {
	constexpr auto operator->() const noexcept(noexcept(*std::declval<Iterator const &>())) {
		return std::addressof(*static_cast<Iterator const &>(*this));
	}
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
		return std::addressof(m_value);
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

template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
struct iterator_adapter_t : operator_arrow<iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>> {
private:
	using tuple_t = tuple<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>;
public:
	static_assert(
		std::is_empty<SubtractFunction>::value,
		"SubtractFunction must be a stateless function. Would we use the state from the left- or right-hand side?"
	);
	static_assert(
		std::is_empty<CompareFunction>::value,
		"CompareFunction must be a stateless comparison function. Would we use the state from the left- or right-hand side?"
	);
	using value_type = decltype(std::declval<DereferenceFunction>()(std::declval<Iterator>()));
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = IteratorCategory;

	// Not sure what these actually mean...
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = std::conditional_t<std::is_reference<value_type>::value, value_type, value_type &>;

	// Even though subtract and compare must be stateless, we must still keep
	// them around. This is because there is no guarantee that they are
	// default-constructible: for instance, lambdas.
	constexpr iterator_adapter_t(Iterator it, DereferenceFunction dereference, AddFunction add, SubtractFunction subtract, CompareFunction compare) noexcept(std::is_nothrow_constructible<tuple_t, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>::value):
		m_data(std::move(it), std::move(dereference), std::move(add), std::move(subtract), std::move(compare))
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
	constexpr auto subtract() const noexcept {
		return m_data[3_bi];
	}
	constexpr auto compare() const noexcept {
		return m_data[4_bi];
	}
	
	template<typename Index>
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<iterator_adapter_t>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}

private:
	tuple_t m_data;
};


template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto operator*(iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & it) BOUNDED_NOEXCEPT_DECLTYPE(
	it.dereference()(it.base())
)

template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction, typename Offset>
constexpr auto operator+(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> lhs,
	Offset const rhs
) BOUNDED_NOEXCEPT_VALUE(
	// Use {} initialization to ensure evaluation order
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>{
		lhs.add()(lhs.base(), rhs),
		std::move(lhs).dereference(),
		std::move(lhs).add(),
		lhs.subtract(),
		lhs.compare()
	}
)


template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto operator-(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.subtract()(lhs.base(), rhs.base())
)

using bounded::compare;
template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto compare(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.compare()(lhs.base(), rhs.base())
)
template<typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction, typename RHS>
constexpr auto compare(
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	RHS const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.compare()(lhs.base(), rhs)
)
template<typename LHS, typename IteratorCategory, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto compare(
	LHS const & lhs,
	iterator_adapter_t<IteratorCategory, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs.compare()(lhs, rhs.base())
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

namespace compare_adl {

using bounded::compare;

template<typename LHS, typename RHS>
constexpr auto indirect_compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs)
)

}	// namespace compare_adl

struct default_compare {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		compare_adl::indirect_compare(lhs, rhs)
	)
};

struct not_a_function {};

template<typename IteratorCategory, typename Iterator>
using get_iterator_category = std::conditional_t<std::is_void<IteratorCategory>::value, typename std::iterator_traits<Iterator>::iterator_category, IteratorCategory>;

}	// namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Add: it + 5_bi
// 3) Subtract: it1 - it2
// 4) Compare: compare(it1, it2)
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these four behaviors. Adding,
// subtracting, and comparing are all linked to the same algorithm, but
// unfortunately, there does not appear to be a way to provide a function that
// unifies these operations.

// See bounded::make to understand the argument defaulting

template<typename IteratorCategory = void, typename Iterator = void, typename DereferenceFunction = void, typename AddFunction = void, typename SubtractFunction = void, typename CompareFunction = void>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference, AddFunction add, SubtractFunction subtract, CompareFunction compare) BOUNDED_NOEXCEPT_VALUE(
	detail::iterator_adapter_t<
		detail::get_iterator_category<IteratorCategory, Iterator>,
		Iterator,
		DereferenceFunction,
		AddFunction,
		SubtractFunction,
		CompareFunction
	>(
		std::move(it),
		std::move(dereference),
		std::move(add),
		std::move(subtract),
		std::move(compare)
	)
)

template<typename IteratorCategory = void, typename Iterator = void, typename DereferenceFunction = void>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter<IteratorCategory>(
		std::move(it),
		std::move(dereference),
		detail::default_add{},
		detail::default_subtract{},
		detail::default_compare{}
	)
)

}	// namespace containers
