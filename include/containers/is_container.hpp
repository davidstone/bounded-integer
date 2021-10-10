// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/c_array.hpp>

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace containers {
namespace detail {

template<typename T>
inline constexpr auto is_c_array = false;

template<typename T, std::size_t size>
inline constexpr auto is_c_array<containers::c_array<T, size>> = true;

template<typename T>
inline constexpr auto is_std_array = false;

template<typename T, std::size_t size>
inline constexpr auto is_std_array<std::array<T, size>> = true;

template<typename T, template<typename...> typename Container>
inline constexpr auto matches_all_type_template_parameters = false;

template<typename... Ts, template<typename...> typename Container>
inline constexpr auto matches_all_type_template_parameters<Container<Ts...>, Container> = true;

} // namespace detail

// `is_container` means that if you own a value of type `T`, you can safely move
// from any subset of the elements in the range.
template<typename T>
inline constexpr auto is_container =
	detail::is_c_array<T> or
	detail::is_std_array<T> or
	detail::matches_all_type_template_parameters<T, std::deque> or
	detail::matches_all_type_template_parameters<T, std::forward_list> or
	detail::matches_all_type_template_parameters<T, std::list> or
	detail::matches_all_type_template_parameters<T, std::vector> or
	detail::matches_all_type_template_parameters<T, std::basic_string> or
	detail::matches_all_type_template_parameters<T, std::set> or
	detail::matches_all_type_template_parameters<T, std::multiset> or
	detail::matches_all_type_template_parameters<T, std::unordered_set> or
	detail::matches_all_type_template_parameters<T, std::unordered_multiset> or
	detail::matches_all_type_template_parameters<T, std::map> or
	detail::matches_all_type_template_parameters<T, std::multimap> or
	detail::matches_all_type_template_parameters<T, std::unordered_map> or
	detail::matches_all_type_template_parameters<T, std::unordered_multimap>;

} // namespace containers
