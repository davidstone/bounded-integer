// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace containers {

template<typename>
inline constexpr auto is_container = false;

template<typename T>
inline constexpr auto is_container<T[]> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<T[size]> = true;


template<typename T, std::size_t size>
inline constexpr auto is_container<std::array<T, size>> = true;

template<typename T, typename Allocator>
inline constexpr auto is_container<std::deque<T, Allocator>> = true;

template<typename T, typename Allocator>
inline constexpr auto is_container<std::forward_list<T, Allocator>> = true;

template<typename T, typename Allocator>
inline constexpr auto is_container<std::list<T, Allocator>> = true;

template<typename T, typename Allocator>
inline constexpr auto is_container<std::vector<T, Allocator>> = true;


template<typename Key, typename T, typename Compare, typename Allocator>
inline constexpr auto is_container<std::map<Key, T, Compare, Allocator>> = true;

template<typename Key, typename Compare, typename Allocator>
inline constexpr auto is_container<std::set<Key, Compare, Allocator>> = true;

template<typename Key, typename T, typename Compare, typename Allocator>
inline constexpr auto is_container<std::multimap<Key, T, Compare, Allocator>> = true;

template<typename Key, typename Compare, typename Allocator>
inline constexpr auto is_container<std::multiset<Key, Compare, Allocator>> = true;


template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline constexpr auto is_container<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>> = true;

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
inline constexpr auto is_container<std::unordered_set<Key, Hash, KeyEqual, Allocator>> = true;

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline constexpr auto is_container<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>> = true;

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
inline constexpr auto is_container<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>> = true;


template<typename CharT, typename Traits, typename Allocator>
inline constexpr auto is_container<std::basic_string<CharT, Traits, Allocator>> = true;


}	// namespace containers
