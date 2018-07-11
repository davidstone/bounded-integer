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
	using reference = decltype(*std::declval<iterator_adapter const &>()) &&;
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

template<typename Iterator, typename SubtractFunction,
	typename = decltype(std::declval<SubtractFunction>()(std::declval<Iterator>(), std::declval<Iterator>()))
>
constexpr auto is_random_access_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_random_access_iterator(Args...) {
	return false;
}

template<typename Iterator, typename AddFunction,
	typename = decltype(std::declval<AddFunction>()(std::declval<Iterator>(), bounded::constant<1>)),
	typename = decltype(std::declval<AddFunction>()(std::declval<Iterator>(), bounded::constant<-1>))
>
constexpr auto is_bidirectional_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_bidirectional_iterator(Args...) {
	return false;
}

template<typename Iterator, typename AddFunction,
	typename = decltype(std::declval<AddFunction>()(std::declval<Iterator>(), 1_bi))
>
constexpr auto is_forward_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_forward_iterator(Args...) {
	return false;
}

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

template<typename Iterator, typename DereferenceFunction, typename AddFunction = detail::default_add, typename SubtractFunction = detail::default_subtract, typename CompareFunction = detail::default_compare>
struct iterator_adapter : detail::operator_arrow<iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>> {
private:
	using tuple_t = tuple<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>;
	using base_iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
public:
	static_assert(
		std::is_empty<SubtractFunction>::value,
		"SubtractFunction must be a stateless function. Would we use the state from the left- or right-hand side?"
	);
	static_assert(
		std::is_empty<CompareFunction>::value,
		"CompareFunction must be a stateless comparison function. Would we use the state from the left- or right-hand side?"
	);
	using value_type = std::decay_t<decltype(std::declval<DereferenceFunction>()(std::declval<Iterator>()))>;
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category =
		std::conditional_t<std::is_same<base_iterator_category, std::output_iterator_tag>{}, std::output_iterator_tag,
		std::conditional_t<std::is_same<base_iterator_category, std::input_iterator_tag>{}, std::input_iterator_tag,
		std::conditional_t<detail::is_random_access_iterator<Iterator, SubtractFunction>(), std::random_access_iterator_tag,
		std::conditional_t<detail::is_bidirectional_iterator<Iterator, AddFunction>(), std::bidirectional_iterator_tag,
		std::conditional_t<detail::is_forward_iterator<Iterator, AddFunction>(), std::forward_iterator_tag,
		void
	>>>>>;
	static_assert(not std::is_void<iterator_category>{});

	// Not sure what these actually mean...
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = std::conditional_t<std::is_reference<value_type>::value, value_type, value_type &>;

	// Even though subtract and compare must be stateless, we must still keep
	// them around. This is because there is no guarantee that they are
	// default-constructible: for instance, lambdas.
	constexpr iterator_adapter(Iterator it, DereferenceFunction dereference, AddFunction add, SubtractFunction subtract, CompareFunction compare) noexcept(std::is_nothrow_constructible<tuple_t, Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>::value):
		m_data(std::move(it), std::move(dereference), std::move(add), std::move(subtract), std::move(compare))
	{
	}
	
	constexpr iterator_adapter(Iterator it, DereferenceFunction dereference) BOUNDED_NOEXCEPT_INITIALIZATION(
		iterator_adapter(
			std::move(it),
			std::move(dereference),
			detail::default_add{},
			detail::default_subtract{},
			detail::default_compare{}
		)
	) {
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
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<iterator_adapter>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}

private:
	tuple_t m_data;
};


template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto operator*(iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & it) BOUNDED_NOEXCEPT_DECLTYPE(
	it.dereference()(it.base())
)

template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction, typename Offset>
constexpr auto operator+(
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> lhs,
	Offset const rhs
) BOUNDED_NOEXCEPT_VALUE(
	// Use {} initialization to ensure evaluation order
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction>{
		lhs.add()(lhs.base(), rhs),
		std::move(lhs).dereference(),
		std::move(lhs).add(),
		lhs.subtract(),
		lhs.compare()
	}
)


template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto operator-(
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.subtract()(lhs.base(), rhs.base())
)

using bounded::compare;
template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto compare(
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.compare()(lhs.base(), rhs.base())
)
template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction, typename RHS>
constexpr auto compare(
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & lhs,
	RHS const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.compare()(lhs.base(), rhs)
)
template<typename LHS, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename CompareFunction>
constexpr auto compare(
	LHS const & lhs,
	iterator_adapter<Iterator, DereferenceFunction, AddFunction, SubtractFunction, CompareFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs.compare()(lhs, rhs.base())
)

}	// namespace containers
