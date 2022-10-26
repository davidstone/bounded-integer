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

template<typename T, typename value_type = std::remove_cvref_t<decltype(*std::declval<T>())>>
concept std_random_access_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std::same_as<T, std::string::iterator> or
		std::same_as<T, std::string::const_iterator> or
		std::same_as<T, typename std::deque<value_type>::iterator> or
		std::same_as<T, typename std::deque<value_type>::const_iterator> or
		std::same_as<T, typename std::vector<value_type>::iterator> or
		std::same_as<T, typename std::vector<value_type>::const_iterator>
	);

template<
	typename T,
	typename value_type = std::remove_cvref_t<decltype(*std::declval<T>())>,
	typename key_type = typename value_type::first_type,
	typename mapped_type = typename value_type::second_type
>
concept std_map_bidirectional_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and
	!std::is_reference_v<key_type> and !std::is_const_v<key_type> and
	!std::is_reference_v<mapped_type> and !std::is_const_v<mapped_type> and
	(
		std::same_as<T, typename std::map<key_type, mapped_type>::iterator> or
		std::same_as<T, typename std::map<key_type, mapped_type>::const_iterator> or
		std::same_as<T, typename std::multimap<key_type, mapped_type>::iterator> or
		std::same_as<T, typename std::multimap<key_type, mapped_type>::const_iterator> or
		std::same_as<T, typename std::unordered_map<key_type, mapped_type>::iterator> or
		std::same_as<T, typename std::unordered_map<key_type, mapped_type>::const_iterator> or
		std::same_as<T, typename std::unordered_multimap<key_type, mapped_type>::iterator> or
		std::same_as<T, typename std::unordered_multimap<key_type, mapped_type>::const_iterator>
	);

template<typename T, typename value_type = std::remove_cvref_t<decltype(*std::declval<T>())>>
concept std_bidirectional_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_random_access_iterator<T> or
		std_map_bidirectional_iterator<T> or
		std::same_as<T, typename std::list<value_type>::iterator> or
		std::same_as<T, typename std::list<value_type>::const_iterator> or
		std::same_as<T, typename std::set<value_type>::iterator> or
		std::same_as<T, typename std::set<value_type>::const_iterator> or
		std::same_as<T, typename std::multiset<value_type>::iterator> or
		std::same_as<T, typename std::multiset<value_type>::const_iterator> or
		std::same_as<T, typename std::unordered_set<value_type>::iterator> or
		std::same_as<T, typename std::unordered_set<value_type>::const_iterator> or
		std::same_as<T, typename std::unordered_multiset<value_type>::iterator> or
		std::same_as<T, typename std::unordered_multiset<value_type>::const_iterator>
	);

template<typename T, typename value_type = std::remove_cvref_t<decltype(*std::declval<T>())>>
concept std_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_bidirectional_iterator<T> or
		std::same_as<T, typename std::forward_list<value_type>::iterator> or
		std::same_as<T, typename std::forward_list<value_type>::const_iterator>
	);

} // namespace detail

template<detail::std_random_access_iterator Iterator>
constexpr auto operator+(Iterator const it, bounded::bounded_integer auto const offset) {
	return it + static_cast<typename Iterator::difference_type>(offset);
}

constexpr auto operator+(bounded::bounded_integer auto const offset, detail::std_random_access_iterator auto const it) {
	return offset + it;
}

constexpr auto operator+(detail::std_iterator auto it, bounded::constant_t<1>) {
	return ++it;
}

constexpr auto operator+(bounded::constant_t<1>, detail::std_iterator auto it) {
	return ++it;
}

constexpr auto operator-(detail::std_bidirectional_iterator auto it, bounded::constant_t<1>) {
	return --it;
}

} // namespace bounded
