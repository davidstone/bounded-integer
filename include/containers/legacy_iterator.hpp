// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {
namespace detail {


template<typename Integer>
struct integer_compatibility {
	integer_compatibility() = default;
	template<typename T, BOUNDED_REQUIRES(std::is_constructible<Integer, T>::value)>
	constexpr integer_compatibility(T t) noexcept(std::is_nothrow_constructible<Integer, T>::value):
		value(std::move(t))
	{
	}

#if 0
	template<typename T, BOUNDED_REQUIRES(std::is_constructible<T, Integer>::value)>
	constexpr operator T() const BOUNDED_NOEXCEPT(
		static_cast<T>(value)
	)
#endif
	constexpr operator Integer const &() const noexcept(std::is_nothrow_copy_constructible<Integer>::value) {
		return value;
	}
	constexpr operator Integer &() noexcept(std::is_nothrow_copy_constructible<Integer>::value) {
		return value;
	}

private:
	Integer value;
};

template<typename Integer>
integer_compatibility(Integer) -> integer_compatibility<Integer>;

template<typename LHS, typename RHS>
constexpr auto compare(integer_compatibility<LHS> const lhs, integer_compatibility<RHS> const rhs) noexcept {
	return bounded::compare(static_cast<LHS>(lhs), static_cast<RHS>(rhs));
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(bounded::is_bounded_integer<RHS>)>
constexpr auto compare(integer_compatibility<LHS> const lhs, RHS const rhs) noexcept {
	return bounded::compare(static_cast<LHS>(lhs), rhs);
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(bounded::is_bounded_integer<LHS>)>
constexpr auto compare(LHS const lhs, integer_compatibility<RHS> const rhs) noexcept {
	return bounded::compare(lhs, static_cast<RHS>(rhs));
}


template<typename Integer, BOUNDED_REQUIRES(bounded::is_bounded_integer<Integer>)>
constexpr auto to_builtin(Integer x) noexcept {
	return x.value();
}
template<typename Integer, BOUNDED_REQUIRES(!bounded::is_bounded_integer<Integer>)>
constexpr auto to_builtin(Integer x) noexcept {
	return x;
}


#define CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(op) \
	template<typename Integer, typename RHS, BOUNDED_REQUIRES(std::numeric_limits<RHS>::is_integer)> \
	constexpr auto operator op(integer_compatibility<Integer> const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT( \
		integer_compatibility(static_cast<Integer const &>(lhs) op to_builtin(rhs)) \
	) \
	template<typename LHS, typename Integer, BOUNDED_REQUIRES(std::numeric_limits<LHS>::is_integer)> \
	constexpr auto operator op(LHS const & lhs, integer_compatibility<Integer> const & rhs) BOUNDED_NOEXCEPT( \
		integer_compatibility(to_builtin(lhs) op static_cast<Integer const &>(rhs)) \
	) \
	template<typename Integer> \
	constexpr auto operator op(integer_compatibility<Integer> const & lhs, integer_compatibility<Integer> const & rhs) BOUNDED_NOEXCEPT( \
		static_cast<Integer const &>(lhs) op static_cast<Integer const &>(rhs) \
	)

CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(+)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(-)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(*)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(/)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(%)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(<<)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(>>)
CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(&)

#undef CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS


}	// namespace detail
}	// namespace containers

namespace std {

template<typename Integer>
struct numeric_limits<containers::detail::integer_compatibility<Integer>> : numeric_limits<Integer> {};

}	// namespace std

namespace containers {

template<typename Iterator>
struct legacy_iterator {
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using difference_type = detail::integer_compatibility<std::ptrdiff_t>;
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	legacy_iterator() = default;
	constexpr legacy_iterator(Iterator it) noexcept(std::is_nothrow_move_constructible<Iterator>::value):
		m_it(std::move(it)) {
	}
	
	constexpr auto base() const noexcept(std::is_nothrow_copy_constructible<Iterator>::value) {
		return m_it;
	}

	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT_GCC_BUG(
		base().operator*()
	)
	constexpr decltype(auto) operator->() const BOUNDED_NOEXCEPT_GCC_BUG(
		base().operator->()
	)
	template<typename Index>
	constexpr decltype(auto) operator[](Index const index) const BOUNDED_NOEXCEPT_GCC_BUG(
		base()[index]
	)

private:
	Iterator m_it;
};

template<typename Iterator, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto operator+(legacy_iterator<Iterator> const lhs, Offset const rhs) BOUNDED_NOEXCEPT(
	legacy_iterator<Iterator>(lhs.base() + static_cast<typename std::iterator_traits<Iterator>::difference_type>(rhs))
)
template<typename Iterator>
constexpr auto operator-(legacy_iterator<Iterator> const lhs, legacy_iterator<Iterator> const rhs) BOUNDED_NOEXCEPT(
	static_cast<typename legacy_iterator<Iterator>::difference_type>(lhs.base() - rhs.base())
)

template<typename Iterator>
constexpr auto compare(legacy_iterator<Iterator> const lhs, legacy_iterator<Iterator> const rhs) BOUNDED_NOEXCEPT(
	compare(lhs.base(), rhs.base())
)

}	// namespace containers
