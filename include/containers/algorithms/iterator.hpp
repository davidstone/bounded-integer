// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/type_list.hpp>

#include <bounded/integer.hpp>
#include <bounded/integer_range.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator first, Sentinel const last, std::random_access_iterator_tag) BOUNDED_NOEXCEPT(
	last - first
)

template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator first, Sentinel const last, std::input_iterator_tag) {
	auto difference = std::iterator_traits<Iterator>::distance_type(0_bi);
	for (; first != last; ++first) {
		++difference;
	}
	return difference;
}

}	// namespace detail

template<typename InputIterator, typename Sentinel>
constexpr auto distance(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::detail::distance(first, last, typename std::iterator_traits<InputIterator>::iterator_category{})
)

namespace detail {

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::random_access_iterator_tag) BOUNDED_NOEXCEPT(
	static_cast<void>(it += offset)
)
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::bidirectional_iterator_tag) {
	for (auto const n : bounded::integer_range(bounded::abs(offset))) {
		static_cast<void>(n);
		if (offset >= 0_bi) {
			++it;
		} else {
			--it;
		}
	}
}
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::input_iterator_tag) {
	for (auto const n : bounded::integer_range(offset)) {
		static_cast<void>(n);
		++it;
	}
}


}	// namespace detail

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset) BOUNDED_NOEXCEPT(
	::containers::detail::advance(it, offset, typename std::iterator_traits<Iterator>::iterator_category{})
)

namespace detail {

template<
	typename Iterator,
	BOUNDED_REQUIRES(bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>)
>
constexpr auto iterator_one() noexcept {
	return 1_bi;
}

template<
	typename Iterator,
	BOUNDED_REQUIRES(!bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>)
>
constexpr auto iterator_one() noexcept {
	return 1;
}

}	// namespace detail

template<
	typename Iterator,
	typename Offset = decltype(::containers::detail::iterator_one<Iterator>()),
	BOUNDED_REQUIRES(is_iterator<Iterator>)
>
constexpr auto next(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, offset);
	return it;
}


template<
	typename Iterator,
	typename Offset = decltype(::containers::detail::iterator_one<Iterator>()),
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
constexpr auto prev(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, -offset);
	return it;
}


namespace detail {

struct move_dereference {
private:
	template<typename Iterator, typename base_result = decltype(*std::declval<Iterator>())>
	using result = std::conditional_t<
		std::is_reference<base_result>::value,
		std::remove_reference_t<base_result> &&,
		base_result
	>;
public:
	template<typename Iterator>
	constexpr auto operator()(Iterator && it) const BOUNDED_NOEXCEPT_DECLTYPE(
		static_cast<result<Iterator>>(*std::forward<Iterator>(it))
	)
};

}	// namespace detail

template<typename Iterator>
constexpr auto move_iterator(Iterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::move_dereference{})
)

template<typename Iterator>
using move_iterator_t = decltype(move_iterator(std::declval<Iterator>()));



namespace detail {

struct reverse_dereference {
	template<typename BidirectionalIterator>
	constexpr auto operator()(BidirectionalIterator it) const BOUNDED_NOEXCEPT_DECLTYPE(
		*::containers::prev(it)
	)
};

struct reverse_add {
	template<typename RandomAccessIterator, typename Offset>
	constexpr auto operator()(RandomAccessIterator it, Offset const offset) const BOUNDED_NOEXCEPT_DECLTYPE(
		it - offset
	)
};

struct reverse_subtract {
	template<typename RandomAccessIterator>
	constexpr auto operator()(RandomAccessIterator const lhs, RandomAccessIterator const rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		rhs - lhs
	)
};

struct reverse_less {
	template<typename RandomAccessIterator>
	constexpr auto operator()(RandomAccessIterator const lhs, RandomAccessIterator const rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		rhs < lhs
	)
};

}	// namespace detail

template<typename BidirectionalIterator>
constexpr auto reverse_iterator(BidirectionalIterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::reverse_dereference{}, detail::reverse_add{}, detail::reverse_subtract{}, detail::reverse_less{})
)

template<typename BidirectionalIterator>
using reverse_iterator_t = decltype(reverse_iterator(std::declval<BidirectionalIterator>()));

}	// namespace containers
