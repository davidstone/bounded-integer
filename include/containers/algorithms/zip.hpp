// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/operator_arrow.hpp>
#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

namespace containers {

using bounded::compare;

// TODO: require that we do not try to zip an input + output iterator
template<typename Iterator1, typename Sentinel1, typename Iterator2>
struct zip_iterator : detail::operator_arrow<zip_iterator<Iterator1, Sentinel1, Iterator2>> {
private:
	using traits1 = std::iterator_traits<Iterator1>;
	using traits2 = std::iterator_traits<Iterator2>;
	template<typename category>
	static constexpr auto either_is_category() noexcept {
		return
			std::is_same_v<typename traits1::iterator_category, category> or
			std::is_same_v<typename traits2::iterator_category, category>;
	}

	Iterator1 m_first1;
	Sentinel1 m_last1;
	Iterator2 m_first2;

	constexpr auto in_first_range() const BOUNDED_NOEXCEPT_VALUE(
		m_first1 != m_last1
	)
	
	constexpr auto tie() const noexcept {
		return ::containers::tie(m_first1, m_last1, m_first2);
	}

public:
	using value_type = bounded::detail::common_type_and_value_category_t<
		typename traits1::value_type,
		typename traits2::value_type
	>;
	using difference_type = decltype(
		std::declval<typename traits1::difference_type>() +
		std::declval<typename traits2::difference_type>()
	);
	
	using iterator_category =
		std::conditional_t<either_is_category<std::output_iterator_tag>(), std::output_iterator_tag,
		std::conditional_t<either_is_category<std::input_iterator_tag>(), std::input_iterator_tag,
		std::forward_iterator_tag
	>>;
	
	using pointer = bounded::detail::common_type_and_value_category_t<
		typename traits1::pointer,
		typename traits2::pointer
	>;

	using reference = bounded::detail::common_type_and_value_category_t<
		typename traits1::reference,
		typename traits2::reference
	>;

	constexpr zip_iterator(Iterator1 first1, Sentinel1 last1, Iterator2 first2):
		m_first1(first1),
		m_last1(last1),
		m_first2(first2)
	{
	}


	constexpr auto operator*() const BOUNDED_NOEXCEPT_DECLTYPE(
		in_first_range() ? *m_first1 : *m_first2
	)

	// TODO: In theory this can support random forward access if the underlying
	// iterators support it
	friend constexpr zip_iterator operator+(zip_iterator const lhs, bounded::constant_t<1>) BOUNDED_NOEXCEPT(
		lhs.in_first_range() ?
			zip_iterator(std::next(lhs.m_first1), lhs.m_last1, lhs.m_first2) :
			zip_iterator(lhs.m_first1, lhs.m_last1, std::next(lhs.m_first2))
	)

	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto operator-(
		zip_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		zip_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		(lhs.m_first1 - rhs.m_first1) + (lhs.m_first2 - rhs.m_first2)
	)

	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto compare(
		zip_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		zip_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		compare(lhs.tie(), rhs.tie())
	)

	template<typename Index>
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<zip_iterator>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}
};

}	// namespace containers
