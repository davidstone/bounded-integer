// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.flat_map;

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

export template<typename Container, extract_key_function<typename range_value_t<Container>::key_type> ExtractKey = to_radix_sort_key_t>
struct basic_flat_map : private flat_associative_base<Container, ExtractKey, false> {
private:
	using base = flat_associative_base<Container, ExtractKey, false>;
public:
	using typename base::key_type;
	using typename base::mapped_type;

	using base::compare;
	using base::extract_key;

	using base::base;

	using base::data;
	using base::begin;
	using base::size;

	using base::capacity;
	using base::replace_empty_allocation;
	using base::reserve;
	
	using base::lazy_insert;
	using base::insert;
	
	using base::erase;
	using base::erase_if;

	constexpr auto find(this auto && self, auto const & key) {
		auto const it = containers::keyed_lower_bound(self, key);
		return (it == ::containers::end(self) or self.compare()(key, get_key(*it))) ?
			::containers::end(self) :
			it;
	}

	// `Other` is required to be a unique range of elements
	template<range Other>
	constexpr auto upsert(Other && other, auto && update) -> void {
		// TODO: Should this reserve?
		auto const original_size = size();
		auto const last = containers::end(OPERATORS_FORWARD(other));
		for (auto it = containers::begin(OPERATORS_FORWARD(other)); it != last; ++it) {
			auto && value = dereference<Other>(it);
			auto const ptr = ::containers::lookup(*this, get_key(value));
			if (ptr) {
				update(*ptr, ::containers::get_mapped(OPERATORS_FORWARD(value)));
			} else {
				containers::push_back(this->m_container, OPERATORS_FORWARD(value));
			}
		}
		auto const midpoint = begin() + original_size;
		::containers::merge_sorted_and_unsorted<false>(this->m_container, midpoint, extract_key());
	}
};

template<typename Range>
basic_flat_map(Range &&) -> basic_flat_map<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_map(Range &&, ExtractKey) -> basic_flat_map<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_map(assume_unique_t, Range &&) -> basic_flat_map<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_map(assume_unique_t, Range &&, ExtractKey) -> basic_flat_map<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_map(assume_sorted_unique_t, Range &&) -> basic_flat_map<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_map(assume_sorted_unique_t, Range &&, ExtractKey) -> basic_flat_map<std::remove_const_t<Range>, ExtractKey>;


export template<typename Container, extract_key_function<typename range_value_t<Container>::key_type> ExtractKey = to_radix_sort_key_t>
struct basic_flat_multimap : private flat_associative_base<Container, ExtractKey, true> {
private:
	using base = flat_associative_base<Container, ExtractKey, true>;
public:
	using typename base::key_type;
	using typename base::mapped_type;

	using base::compare;
	using base::extract_key;

	using base::base;
	
	using base::data;
	using base::begin;
	using base::size;

	using base::capacity;
	using base::replace_empty_allocation;
	using base::reserve;
	
	using base::lazy_insert;
	using base::insert;
	
	using base::erase;
	using base::erase_if;
};

template<typename Range>
basic_flat_multimap(Range &&) -> basic_flat_multimap<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multimap(Range &&, ExtractKey) -> basic_flat_multimap<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_multimap(assume_unique_t, Range &&) -> basic_flat_multimap<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multimap(assume_unique_t, Range &&, ExtractKey) -> basic_flat_multimap<std::remove_const_t<Range>, ExtractKey>;

template<typename Range>
basic_flat_multimap(assume_sorted_unique_t, Range &&) -> basic_flat_multimap<std::remove_const_t<Range>>;
template<typename Range, typename ExtractKey>
basic_flat_multimap(assume_sorted_unique_t, Range &&, ExtractKey) -> basic_flat_multimap<std::remove_const_t<Range>, ExtractKey>;

template<typename Key, typename Mapped>
constexpr auto maximum_map_size = numeric_traits::max_value<array_size_type<map_value_type<Key, Mapped>>>;

template<bounded::isomorphic_to_integral Key, typename Mapped>
constexpr auto maximum_map_size<Key, Mapped> = bounded::min(
	bounded::number_of<Key>,
	numeric_traits::max_value<array_size_type<map_value_type<Key, Mapped>>>
);

export template<typename Key, typename Mapped, typename... MaybeExtractKey>
using flat_map = basic_flat_map<vector<map_value_type<Key, Mapped>, maximum_map_size<Key, Mapped>>, MaybeExtractKey...>;

export template<typename Key, typename Mapped, array_size_type<map_value_type<Key, Mapped>> capacity, typename... MaybeExtractKey>
using static_flat_map = basic_flat_map<static_vector<map_value_type<Key, Mapped>, capacity>, MaybeExtractKey...>;

export template<typename Key, typename Mapped, typename... MaybeExtractKey>
using flat_multimap = basic_flat_multimap<vector<map_value_type<Key, Mapped>>, MaybeExtractKey...>;

export template<typename Key, typename Mapped, array_size_type<map_value_type<Key, Mapped>> capacity, typename... MaybeExtractKey>
using static_flat_multimap = basic_flat_multimap<static_vector<map_value_type<Key, Mapped>, capacity>, MaybeExtractKey...>;

} // namespace containers
