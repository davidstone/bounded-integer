// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

// TODO: implement as range_adapter so that iterators remain cheap to copy
namespace containers {
namespace detail {

template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
struct iterator_adapter_t {
	static_assert(
		std::is_empty<SubtractFunction>::value,
		"SubtractFunction must be a stateless comparison function. Would we use the state from the left- or right-hand side?"
	);
	static_assert(
		std::is_empty<LessFunction>::value,
		"LessFunction must be a stateless comparison function. Would we use the state from the left- or right-hand side?"
	);
	using value_type = decltype(std::declval<DereferenceFunction>()(std::declval<Iterator>()));
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

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
	
	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT(
		dereference()(base())
	)
	// operator-> intentionally missing

	// TODO: Support ForwardIterator
	friend constexpr auto operator+(iterator_adapter_t lhs, difference_type const rhs) BOUNDED_NOEXCEPT(
		// Use {} initialization to ensure evaluation order
		iterator_adapter_t{
			lhs.add()(lhs.base(), rhs),
			std::move(lhs).dereference(),
			std::move(lhs).add(),
			std::move(lhs).subtract(),
			std::move(lhs).less()
		}
	)
	friend constexpr decltype(auto) operator-(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.subtract()(lhs.base(), rhs.base())
	)

	constexpr decltype(auto) operator[](index_type<iterator_adapter_t> const index) const BOUNDED_NOEXCEPT(
		*(*this + index)
	)

	constexpr auto base() const noexcept {
		return m_data[0_bi];
	}

	friend constexpr decltype(auto) operator<(iterator_adapter_t const & lhs, iterator_adapter_t const & rhs) BOUNDED_NOEXCEPT(
		lhs.less()(lhs.base(), rhs.base())
	)
private:
	constexpr auto && dereference() const & noexcept {
		return m_data[1_bi];
	}
	constexpr auto && dereference() && noexcept {
		return std::move(m_data)[1_bi];
	}
	constexpr auto && add() const & noexcept {
		return m_data[2_bi];
	}
	constexpr auto && add() && noexcept {
		return std::move(m_data)[2_bi];
	}
	constexpr auto && subtract() const & noexcept {
		return m_data[3_bi];
	}
	constexpr auto && subtract() && noexcept {
		return std::move(m_data)[3_bi];
	}
	constexpr auto && less() const & noexcept {
		return m_data[4_bi];
	}
	constexpr auto && less() && noexcept {
		return std::move(m_data)[4_bi];
	}
	tuple<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> m_data;
};

template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator==(
	iterator_adapter_t<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	iterator_adapter_t<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.base() == rhs.base()
)
template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction, typename RHS>
constexpr auto operator==(
	iterator_adapter_t<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & lhs,
	RHS const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.base() == rhs
)
template<typename LHS, typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto operator==(
	LHS const & lhs,
	iterator_adapter_t<Iterator, DereferenceFunction, AddFunction, SubtractFunction, LessFunction> const & rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs == rhs.base()
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

}	// namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Adding: it + 5_bi
// 3) Subtracting: it1 - it2
// 4) Comparing: it1 < it2
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these four behaviors. Adding,
// subtracting, and comparing are all linked to the same algorithm, but
// unfortunately, there does not appear to be a way to provide a function that
// unifies these operations.

template<typename Iterator, typename DereferenceFunction, typename AddFunction, typename SubtractFunction, typename LessFunction>
constexpr auto iterator_adapter(Iterator && it, DereferenceFunction && dereference, AddFunction && add, SubtractFunction && subtract, LessFunction && less) BOUNDED_NOEXCEPT(
	detail::iterator_adapter_t<
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

template<typename Iterator, typename DereferenceFunction>
constexpr auto iterator_adapter(Iterator it, DereferenceFunction dereference) BOUNDED_NOEXCEPT(
	iterator_adapter(it, std::move(dereference), detail::default_add{}, detail::default_subtract{}, detail::default_less{})
)

}	// namespace containers
