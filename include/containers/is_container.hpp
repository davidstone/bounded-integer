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
struct is_container_c : std::false_type {};

template<typename T>
constexpr auto is_container = is_container_c<std::remove_cv_t<std::remove_reference_t<T>>>::value;


template<typename T>
struct is_container_c<T[]> : std::true_type {};

template<typename T, std::size_t size>
struct is_container_c<T[size]> : std::true_type {};


template<typename T, std::size_t size>
struct is_container_c<std::array<T, size>> : std::true_type {};

template<typename T, typename Allocator>
struct is_container_c<std::deque<T, Allocator>> : std::true_type {};

template<typename T, typename Allocator>
struct is_container_c<std::forward_list<T, Allocator>> : std::true_type {};

template<typename T, typename Allocator>
struct is_container_c<std::list<T, Allocator>> : std::true_type {};

template<typename T, typename Allocator>
struct is_container_c<std::vector<T, Allocator>> : std::true_type {};


template<typename Key, typename T, typename Compare, typename Allocator>
struct is_container_c<std::map<Key, T, Compare, Allocator>> : std::true_type {};

template<typename Key, typename Compare, typename Allocator>
struct is_container_c<std::set<Key, Compare, Allocator>> : std::true_type {};

template<typename Key, typename T, typename Compare, typename Allocator>
struct is_container_c<std::multimap<Key, T, Compare, Allocator>> : std::true_type {};

template<typename Key, typename Compare, typename Allocator>
struct is_container_c<std::multiset<Key, Compare, Allocator>> : std::true_type {};


template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
struct is_container_c<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>> : std::true_type {};

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct is_container_c<std::unordered_set<Key, Hash, KeyEqual, Allocator>> : std::true_type {};

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
struct is_container_c<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>> : std::true_type {};

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
struct is_container_c<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>> : std::true_type {};


template<typename CharT, typename Traits, typename Allocator>
struct is_container_c<std::basic_string<CharT, Traits, Allocator>> : std::true_type {};


}	// namespace containers
