// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.flat_set;

import containers.algorithms.keyed_binary_search;
import containers.algorithms.sort.to_radix_sort_key;

import containers.append;
import containers.associative_container;
import containers.begin_end;
import containers.common_functions;
import containers.dereference;
import containers.flat_associative_base;
import containers.lookup;
import containers.map_tags;
import containers.map_value_type;
import containers.maximum_array_size;
import containers.push_back;
import containers.range;
import containers.range_value_t;
import containers.static_vector;
import containers.vector;
export import containers.common_iterator_functions;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

template<typename ExtractKey, typename T>
concept extract_key_function = requires(ExtractKey const & extract_key, T const & value) {
	extract_key(value);
};

export template<
	typename Container,
	extract_key_function<range_value_t<Container>> ExtractKey = to_radix_sort_key_t
>
struct basic_flat_set :
	private flat_associative_base<Container, range_value_t<Container>, ExtractKey, false>
{
private:
	using base = flat_associative_base<Container, range_value_t<Container>, ExtractKey, false>;
public:
	using typename base::key_type;

	using base::key_comp;
	using base::extract_key;

	using base::base;

	using base::data;
	using base::begin;
	using base::size;

	using base::capacity;
	using base::replace_empty_allocation;
	using base::reserve;
	
	template<bounded::convertible_to<key_type> Key = key_type>
	constexpr auto insert(Key && key) {
		return base::insert(
			OPERATORS_FORWARD(key),
			[](auto && key_) { return key_; }
		);
	}
	using base::insert;
	
	using base::erase;
	using base::erase_if;

	constexpr auto find(this auto && self, auto const & key) {
		auto const it = containers::keyed_lower_bound(self, key);
		return (it == ::containers::end(self) or self.key_comp()(key, get_key(*it))) ?
			::containers::end(self) :
			it;
	}
};

template<typename Range>
basic_flat_set(Range &&) -> basic_flat_set<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_set(Range &&, ExtractKey) -> basic_flat_set<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_set(assume_unique_t, Range &&) -> basic_flat_set<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_set(assume_unique_t, Range &&, ExtractKey) -> basic_flat_set<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_set(assume_sorted_unique_t, Range &&) -> basic_flat_set<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_set(assume_sorted_unique_t, Range &&, ExtractKey) -> basic_flat_set<std::remove_const_t<Range>, ExtractKey>;


export template<
	typename Container,
	extract_key_function<range_value_t<Container>> ExtractKey = to_radix_sort_key_t
>
struct basic_flat_multiset :
	private flat_associative_base<Container, range_value_t<Container>, ExtractKey, true>
{
private:
	using base = flat_associative_base<Container, range_value_t<Container>, ExtractKey, true>;
public:
	using typename base::key_type;

	using base::key_comp;
	using base::extract_key;

	using base::base;
	
	using base::data;
	using base::begin;
	using base::size;

	using base::capacity;
	using base::replace_empty_allocation;
	using base::reserve;
	
	template<bounded::convertible_to<key_type> Key = key_type>
	constexpr auto insert(Key && key) {
		return base::insert(
			OPERATORS_FORWARD(key),
			[](auto && key_) { return key_; }
		);
	}
	using base::insert;
	
	using base::erase;
	using base::erase_if;
};

template<typename Range>
basic_flat_multiset(Range &&) -> basic_flat_multiset<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multiset(Range &&, ExtractKey) -> basic_flat_multiset<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_multiset(assume_unique_t, Range &&) -> basic_flat_multiset<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multiset(assume_unique_t, Range &&, ExtractKey) -> basic_flat_multiset<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_multiset(assume_sorted_unique_t, Range &&) -> basic_flat_multiset<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multiset(assume_sorted_unique_t, Range &&, ExtractKey) -> basic_flat_multiset<std::remove_const_t<Range>, ExtractKey>;

template<typename Key>
constexpr auto maximum_set_size = numeric_traits::max_value<array_size_type<Key>>;

template<bounded::isomorphic_to_integral Key>
constexpr auto maximum_set_size<Key> = bounded::min(
	bounded::number_of<Key>,
	numeric_traits::max_value<array_size_type<Key>>
);

export template<typename Key, typename... MaybeExtractKey>
using flat_set = basic_flat_set<vector<Key, maximum_set_size<Key>>, MaybeExtractKey...>;

export template<typename Key, array_size_type<Key> capacity, typename... MaybeExtractKey>
using static_flat_set = basic_flat_set<static_vector<Key, capacity>, MaybeExtractKey...>;

export template<typename Key, typename... MaybeExtractKey>
using flat_multiset = basic_flat_multiset<vector<Key>, MaybeExtractKey...>;

export template<typename Key, array_size_type<Key> capacity, typename... MaybeExtractKey>
using static_flat_multiset = basic_flat_multiset<static_vector<Key, capacity>, MaybeExtractKey...>;

} // namespace containers
