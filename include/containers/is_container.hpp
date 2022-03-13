// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/c_array.hpp>
#include <containers/erase_concepts.hpp>

#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace containers {
namespace detail {

template<typename T, template<typename...> typename Container>
inline constexpr auto matches_all_type_template_parameters = false;

template<typename... Ts, template<typename...> typename Container>
inline constexpr auto matches_all_type_template_parameters<Container<Ts...>, Container> = true;

} // namespace detail

// `is_container` means that if you own a value of type `T`, you can safely move
// from any subset of the elements in the range.
template<typename T>
inline constexpr auto is_container =
	(range<T> and (detail::pop_backable<T> or detail::pop_frontable<T>)) or
	detail::matches_all_type_template_parameters<T, std::set> or
	detail::matches_all_type_template_parameters<T, std::multiset> or
	detail::matches_all_type_template_parameters<T, std::unordered_set> or
	detail::matches_all_type_template_parameters<T, std::unordered_multiset> or
	detail::matches_all_type_template_parameters<T, std::map> or
	detail::matches_all_type_template_parameters<T, std::multimap> or
	detail::matches_all_type_template_parameters<T, std::unordered_map> or
	detail::matches_all_type_template_parameters<T, std::unordered_multimap>;

template<typename T, std::size_t size>
inline constexpr auto is_container<containers::c_array<T, size>> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<std::array<T, size>> = true;

} // namespace containers
