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
#include <containers/iterator_adapter.hpp>
#include <containers/type_list.hpp>

#include <bounded/integer.hpp>
#include <bounded/integer_range.hpp>

#include <iterator>

namespace containers {
namespace detail {

template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator first, Sentinel const last, std::random_access_iterator_tag) BOUNDED_NOEXCEPT(
	last - first
)

template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator first, Sentinel const last, std::input_iterator_tag) {
	auto difference = std::iterator_traits<Iterator>::distance_type(bounded::constant<0>);
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
		if (offset >= bounded::constant<0>) {
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
	return bounded::constant<1>;
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

template<typename Iterator>
struct move_function_object {
private:
	using base_ref = decltype(*std::declval<Iterator>());
	using result = std::conditional_t<std::is_reference<base_ref>::value, std::remove_reference_t<base_ref> &&, base_ref>;
public:
	template<typename T>
	constexpr auto operator()(T && value) const BOUNDED_NOEXCEPT_DECLTYPE(
		static_cast<result>(std::forward<T>(value))
	)
};

}	// namespace detail

template<typename Iterator>
constexpr auto make_move_iterator(Iterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::move_function_object<Iterator>{})
)

template<typename Iterator>
using move_iterator = decltype(make_move_iterator(std::declval<Iterator>()));



namespace detail {

struct reverse_dereference {
	template<typename BidirectionalIterator>
	constexpr auto operator()(BidirectionalIterator it) const BOUNDED_NOEXCEPT_DECLTYPE(
		*::containers::prev(it)
	)
};

// TODO: Support BidirectionalIterator
struct reverse_add {
	template<typename RandomAccessIterator>
	constexpr auto operator()(RandomAccessIterator it, typename std::iterator_traits<RandomAccessIterator>::difference_type const difference) const BOUNDED_NOEXCEPT_DECLTYPE(
		it - difference
	)
};

}	// namespace detail

template<typename BidirectionalIterator>
constexpr auto make_reverse_iterator(BidirectionalIterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::reverse_dereference{}, detail::reverse_add{})
)

template<typename BidirectionalIterator>
using reverse_iterator = decltype(make_reverse_iterator(std::declval<BidirectionalIterator>()));

}	// namespace containers
