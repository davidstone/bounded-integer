// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {
namespace detail {

template<typename T, typename value_type = typename std::iterator_traits<T>::value_type>
concept std_random_access_iterator =
	!std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std::is_same_v<T, std::string::iterator> or
		std::is_same_v<T, std::string::const_iterator> or
		std::is_same_v<T, typename std::deque<value_type>::iterator> or
		std::is_same_v<T, typename std::deque<value_type>::const_iterator> or
		std::is_same_v<T, typename std::vector<value_type>::iterator> or
		std::is_same_v<T, typename std::vector<value_type>::const_iterator>
	);

template<typename T, typename value_type = typename std::iterator_traits<T>::value_type>
concept std_bidirectional_iterator =
	!std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_random_access_iterator<T> or
		std::is_same_v<T, typename std::list<value_type>::iterator> or
		std::is_same_v<T, typename std::list<value_type>::const_iterator> or
		std::is_same_v<T, typename std::map<typename value_type::first_type, typename value_type::second_type>::iterator> or
		std::is_same_v<T, typename std::map<typename value_type::first_type, typename value_type::second_type>::const_iterator> or
		std::is_same_v<T, typename std::multimap<typename value_type::first_type, typename value_type::second_type>::iterator> or
		std::is_same_v<T, typename std::multimap<typename value_type::first_type, typename value_type::second_type>::const_iterator> or
		std::is_same_v<T, typename std::set<value_type>::iterator> or
		std::is_same_v<T, typename std::set<value_type>::const_iterator> or
		std::is_same_v<T, typename std::multiset<value_type>::iterator> or
		std::is_same_v<T, typename std::multiset<value_type>::const_iterator> or
		std::is_same_v<T, typename std::unordered_map<typename value_type::first_type, typename value_type::second_type>::iterator> or
		std::is_same_v<T, typename std::unordered_map<typename value_type::first_type, typename value_type::second_type>::const_iterator> or
		std::is_same_v<T, typename std::unordered_multimap<typename value_type::first_type, typename value_type::second_type>::iterator> or
		std::is_same_v<T, typename std::unordered_multimap<typename value_type::first_type, typename value_type::second_type>::const_iterator> or
		std::is_same_v<T, typename std::unordered_set<value_type>::iterator> or
		std::is_same_v<T, typename std::unordered_set<value_type>::const_iterator> or
		std::is_same_v<T, typename std::unordered_multiset<value_type>::iterator> or
		std::is_same_v<T, typename std::unordered_multiset<value_type>::const_iterator>
	);

template<typename T, typename value_type = typename std::iterator_traits<T>::value_type>
concept std_iterator =
	!std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_bidirectional_iterator<T> or
		std::is_same_v<T, typename std::forward_list<value_type>::iterator> or
		std::is_same_v<T, typename std::forward_list<value_type>::const_iterator>
	);

} // namespace detail

constexpr auto operator+(detail::std_random_access_iterator auto const it, bounded::bounded_integer auto const offset) {
	return it + offset.value();
}

constexpr auto operator+(bounded::bounded_integer auto const offset, detail::std_random_access_iterator auto const it) {
	return offset.value() + it;
}

constexpr auto operator+(detail::std_iterator auto const it, bounded::constant_t<0>) {
	return ++it;
}

constexpr auto operator+(bounded::constant_t<0>, detail::std_iterator auto const it) {
	return ++it;
}

constexpr auto operator-(detail::std_bidirectional_iterator auto const it, bounded::constant_t<0>) {
	return --it;
}

} // namespace bounded
