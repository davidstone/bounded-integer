// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/is_range.hpp>
#include <containers/operator_arrow.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/integer.hpp>

namespace containers {

template<typename Sentinel>
struct concatenate_view_sentinel {
	constexpr explicit concatenate_view_sentinel(Sentinel last):
		m_last(std::move(last))
	{
	}
	
	constexpr auto base() const {
		return m_last;
	}
private:
	Sentinel m_last;
};


template<typename Iterator1, typename Sentinel1, typename Iterator2>
struct concatenate_view_iterator : detail::operator_arrow<concatenate_view_iterator<Iterator1, Sentinel1, Iterator2>> {
private:
	using traits1 = std::iterator_traits<Iterator1>;
	using traits2 = std::iterator_traits<Iterator2>;
	template<typename category>
	static constexpr auto either_is_category() noexcept {
		return
			std::is_same_v<typename traits1::iterator_category, category> or
			std::is_same_v<typename traits2::iterator_category, category>;
	}
	static_assert(!either_is_category<std::output_iterator_tag>() or !either_is_category<std::input_iterator_tag>());

	Iterator1 m_first1;
	Sentinel1 m_last1;
	Iterator2 m_first2;

	constexpr auto in_first_range() const BOUNDED_NOEXCEPT_VALUE(
		m_first1 != m_last1
	)
	
	constexpr auto tie() const noexcept {
		return bounded::tie(m_first1, m_last1, m_first2);
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
		std::conditional_t<either_is_category<std::forward_iterator_tag>(), std::forward_iterator_tag,
		std::conditional_t<either_is_category<std::bidirectional_iterator_tag>(), std::bidirectional_iterator_tag,
		std::random_access_iterator_tag
	>>>>;
	
	using pointer = std::remove_reference_t<value_type> *;

	using reference = bounded::detail::common_type_and_value_category_t<
		typename traits1::reference,
		typename traits2::reference
	>;

	constexpr concatenate_view_iterator(Iterator1 first1, Sentinel1 last1, Iterator2 first2):
		m_first1(first1),
		m_last1(last1),
		m_first2(first2)
	{
	}


	constexpr auto operator*() const BOUNDED_NOEXCEPT_DECLTYPE(
		in_first_range() ? *m_first1 : *m_first2
	)

	template<typename Offset, BOUNDED_REQUIRES(
		std::is_convertible_v<Offset, difference_type> and
		(
			!bounded::is_bounded_integer<difference_type> or
			std::is_same_v<iterator_category, std::random_access_iterator_tag> or
			std::numeric_limits<Offset>::max() == bounded::constant<1>
		)
	)>
	friend constexpr auto operator+(concatenate_view_iterator const lhs, Offset const offset) {
		if constexpr (std::is_convertible_v<iterator_category, std::random_access_iterator_tag>) {
			auto const first_remaining = lhs.m_last1 - lhs.m_first1;
			return first_remaining >= offset ?
				concatenate_view_iterator(
					lhs.m_first1 + static_cast<typename std::iterator_traits<Iterator1>::difference_type>(offset),
					lhs.m_last1,
					lhs.m_first2
				) :
				concatenate_view_iterator(
					lhs.m_first1 + first_remaining,
					lhs.m_last1,
					lhs.m_first2 + static_cast<typename std::iterator_traits<Iterator2>::difference_type>((offset - first_remaining))
				);
		} else {
			return lhs.in_first_range() ?
				concatenate_view_iterator(std::next(lhs.m_first1), lhs.m_last1, lhs.m_first2) :
				concatenate_view_iterator(lhs.m_first1, lhs.m_last1, std::next(lhs.m_first2));
		}
	}

	// TODO: What if they have different m_last1?
	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto operator-(
		concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		(lhs.m_first1 - rhs.m_first1) + (lhs.m_first2 - rhs.m_first2)
	)


	template<
		typename LHSSentinel,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto operator-(
		concatenate_view_sentinel<LHSSentinel> const lhs,
		concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		(rhs.m_last1 - rhs.m_first1) + (lhs.base() - rhs.m_first2)
	)


	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto compare(
		concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		compare(lhs.tie(), rhs.tie())
	)

	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSSentinel
	>
	friend constexpr auto compare(
		concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		concatenate_view_sentinel<RHSSentinel> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		compare(bounded::tie(lhs.m_first1, lhs.m_first2), bounded::tie(lhs.m_last1, rhs.base()))
	)


	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
	>
	friend constexpr auto operator==(
		concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		lhs.tie() == rhs.tie()
	)

	template<
		typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
		typename RHSSentinel
	>
	friend constexpr auto operator==(
		concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
		concatenate_view_sentinel<RHSSentinel> const rhs
	) BOUNDED_NOEXCEPT_DECLTYPE(
		bounded::tie(lhs.m_first1, lhs.m_first2) == bounded::tie(lhs.m_last1, rhs.base())
	)


	template<typename Index>
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<concatenate_view_iterator>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}
};


template<
	typename LHSIterator1, typename LHSSentinel1, typename LHSIterator2,
	typename RHSSentinel
>
constexpr auto operator-(
	concatenate_view_iterator<LHSIterator1, LHSSentinel1, LHSIterator2> const lhs,
	concatenate_view_sentinel<RHSSentinel> const rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	-(rhs - lhs)
)


template<
	typename LHSSentinel,
	typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
>
constexpr auto compare(
	concatenate_view_sentinel<LHSSentinel> const lhs,
	concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(0, compare(rhs, lhs))
)


template<
	typename LHSSentinel,
	typename RHSIterator1, typename RHSSentinel1, typename RHSIterator2
>
constexpr auto operator==(
	concatenate_view_sentinel<LHSSentinel> const lhs,
	concatenate_view_iterator<RHSIterator1, RHSSentinel1, RHSIterator2> const rhs
) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs == lhs
)

namespace detail {

template<typename LHS, typename RHS>
struct concatenate_view_t {
	using const_iterator = concatenate_view_iterator<
		decltype(begin(std::declval<LHS const &>())),
		decltype(end(std::declval<LHS const &>())),
		decltype(begin(std::declval<RHS const &>()))
	>;
	using iterator = concatenate_view_iterator<
		decltype(begin(std::declval<LHS &>())),
		decltype(end(std::declval<LHS &>())),
		decltype(begin(std::declval<RHS &>()))
	>;
	using value_type = typename iterator::value_type;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<std::numeric_limits<typename iterator::difference_type>::max().value()>
	>;
	
	constexpr concatenate_view_t(LHS && lhs_, RHS && rhs_) noexcept(std::is_nothrow_constructible_v<LHS, LHS &&> and std::is_nothrow_constructible_v<RHS, RHS &&>):
		lhs(BOUNDED_FORWARD(lhs_)),
		rhs(BOUNDED_FORWARD(rhs_))
	{
	}
	
	friend constexpr auto begin(concatenate_view_t & range) {
		return iterator(begin(range.lhs), end(range.lhs), begin(range.rhs));
	}
	friend constexpr auto begin(concatenate_view_t const & range) {
		return const_iterator(begin(range.lhs), end(range.lhs), begin(range.rhs));
	}
	friend constexpr auto end(concatenate_view_t & range) {
		return concatenate_view_sentinel(end(range.rhs));
	}
	friend constexpr auto end(concatenate_view_t const & range) {
		return concatenate_view_sentinel(end(range.rhs));
	}

private:
	LHS lhs;
	RHS rhs;
};


#if 0
// TODO: Figure out why this crashes clang
template<typename LHS, typename RHS, BOUNDED_REQUIRES(
	!std::is_reference_v<LHS> and
	!std::is_reference_v<RHS> and
	std::is_reference_v<decltype(*begin(std::declval<concatenate_view_t<LHS, RHS>>()))>
)>
constexpr auto begin(concatenate_view_t<LHS, RHS> && range) {
	return ::containers::move_iterator(begin(range));
}
#endif

} // namespace detail

template<typename LHS, typename RHS>
constexpr auto concatenate_view(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT_VALUE(
	detail::concatenate_view_t<LHS, RHS>(BOUNDED_FORWARD(lhs), BOUNDED_FORWARD(rhs))
)

template<typename LHS, typename RHS, typename... Remainder, BOUNDED_REQUIRES(is_range<LHS> and is_range<RHS> and (... and is_range<Remainder>))>
constexpr auto concatenate_view(LHS && lhs, RHS && rhs, Remainder && ... remainder) {
	return ::containers::concatenate_view(
		::containers::concatenate_view(BOUNDED_FORWARD(lhs), BOUNDED_FORWARD(rhs)),
		BOUNDED_FORWARD(remainder)...
	);
}

}	// namespace containers
