// Adapter for functions that cannot be made compatible with a bounded::integer
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

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {
namespace detail {
namespace bounded_integer {


template<typename Integer>
struct integer_compatibility {
	integer_compatibility() = default;
	template<typename T, BOUNDED_REQUIRES(std::is_constructible<Integer, T>::value)>
	constexpr integer_compatibility(T && t) noexcept(std::is_nothrow_constructible<Integer, T>::value):
		value(std::forward<T>(t))
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

#define CONTAINERS_DETAIL_INTEGER_COMPATIBILITY_OPERATORS(op) \
	template<typename Integer, typename RHS, BOUNDED_REQUIRES(std::numeric_limits<RHS>::is_integer)> \
	constexpr auto operator op(integer_compatibility<Integer> const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT( \
		static_cast<integer_compatibility<Integer>>(static_cast<Integer const &>(lhs) + rhs) \
	) \
	template<typename LHS, typename Integer, BOUNDED_REQUIRES(std::numeric_limits<LHS>::is_integer)> \
	constexpr auto operator op(LHS const & lhs, integer_compatibility<Integer> const & rhs) BOUNDED_NOEXCEPT( \
		rhs + lhs \
	) \
	template<typename Integer> \
	constexpr auto operator op(integer_compatibility<Integer> const & lhs, integer_compatibility<Integer> const & rhs) BOUNDED_NOEXCEPT( \
		static_cast<Integer const &>(lhs) + rhs \
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


}	// namespace bounded_integer
}	// namespace detail
}	// namespace containers

namespace std {

template<typename Integer>
struct numeric_limits<containers::detail::bounded_integer::integer_compatibility<Integer>> : numeric_limits<Integer> {};

}	// namespace std

namespace containers {

template<typename Iterator>
struct legacy_iterator_t {
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using difference_type = detail::bounded_integer::integer_compatibility<std::ptrdiff_t>;
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	legacy_iterator_t() = default;
	constexpr legacy_iterator_t(Iterator it) noexcept(std::is_nothrow_move_constructible<Iterator>::value):
		m_it(std::move(it)) {
	}
	
	constexpr auto base() const noexcept(std::is_nothrow_copy_constructible<Iterator>::value) {
		return m_it;
	}

	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT(
		base().operator*()
	)
	constexpr decltype(auto) operator->() const BOUNDED_NOEXCEPT(
		base().operator->()
	)
	template<typename Index>
	constexpr decltype(auto) operator[](Index const index) const BOUNDED_NOEXCEPT(
		*(*this + index)
	)

private:
	Iterator m_it;
};

template<typename Iterator, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto operator+(legacy_iterator_t<Iterator> const lhs, Offset const rhs) BOUNDED_NOEXCEPT(
	legacy_iterator_t<Iterator>(lhs.base() + static_cast<typename std::iterator_traits<Iterator>::difference_type>(rhs))
)
template<typename Iterator>
constexpr auto operator-(legacy_iterator_t<Iterator> const lhs, legacy_iterator_t<Iterator> const rhs) BOUNDED_NOEXCEPT(
	static_cast<typename legacy_iterator_t<Iterator>::difference_type>(lhs.base() - rhs.base())
)

template<typename Iterator>
constexpr auto operator==(legacy_iterator_t<Iterator> const lhs, legacy_iterator_t<Iterator> const rhs) BOUNDED_NOEXCEPT(
	lhs.base() == rhs.base()
)
template<typename Iterator>
constexpr auto operator<(legacy_iterator_t<Iterator> const lhs, legacy_iterator_t<Iterator> const rhs) BOUNDED_NOEXCEPT(
	lhs.base() < rhs.base()
)


template<typename Iterator, BOUNDED_REQUIRES(is_iterator<Iterator>)>
constexpr auto legacy_iterator(Iterator it) BOUNDED_NOEXCEPT(
	legacy_iterator_t<Iterator>(std::move(it))
)

}	// namespace containers
