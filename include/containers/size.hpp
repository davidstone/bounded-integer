// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/distance.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_range.hpp>
#include <containers/iterator_t.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<typename Range>
concept sized_range = (range<Range> and detail::has_member_size<Range>) or forward_random_access_range<Range>;

namespace detail {

template<range T>
struct size_type_impl {
	using type = bounded::integer<0, bounded::builtin_max_value<iter_difference_t<iterator_t<T>>>>;
};

template<typename T>
concept has_size_type = requires { typename T::size_type; };

template<range T> requires has_size_type<T> and has_member_size<T>
struct size_type_impl<T> {
	static_assert(std::is_same_v<decltype(std::declval<T const &>().size()), typename T::size_type>);
	using type = typename T::size_type;
};

template<range T> requires has_size_type<T>
struct size_type_impl<T> {
	using type = typename T::size_type;
};

template<range T> requires has_member_size<T>
struct size_type_impl<T> {
	using type = decltype(std::declval<T const &>().size());
};

} // namespace detail

template<typename T>
using range_size_t = typename detail::size_type_impl<std::remove_cvref_t<T>>::type;

template<sized_range Range>
constexpr auto size(Range const & range) {
	if constexpr (detail::has_member_size<Range>) {
		return range.size();
	} else {
		return ::bounded::assume_in_range<range_size_t<Range>>(containers::end(range) - containers::begin(range));
	}
}

template<typename T, std::size_t size_>
constexpr auto size(c_array<T, size_> const &) {
	return bounded::constant<size_>;
}

template<range Range>
constexpr auto linear_size(Range const & r) {
	if constexpr (requires { containers::size(r); }) {
		return containers::size(r);
	} else {
		return ::bounded::assume_in_range<range_size_t<Range>>(containers::distance(containers::begin(r), containers::end(r)));
	}
}

} // namespace containers