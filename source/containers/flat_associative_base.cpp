// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.flat_associative_base;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.ska_sort;

import containers.algorithms.advance;
import containers.algorithms.erase;
import containers.algorithms.keyed_binary_search;
import containers.algorithms.unique;
import containers.append;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_range;
import containers.data;
import containers.extract_key_to_less;
import containers.initializer_range;
import containers.insert;
import containers.iterator_t;
import containers.legacy_iterator;
import containers.map_tags;
import containers.map_value_type;
import containers.mutable_iterator;
import containers.range;
import containers.range_size_t;
import containers.range_value_t;
import containers.size;
import containers.subrange;
export import containers.common_iterator_functions;

import bounded;
import std_module;

namespace containers {

template<typename T, typename Key, typename ExtractKey>
struct extract_key_t {
	constexpr explicit extract_key_t(ExtractKey extract_key):
		m_extract(std::move(extract_key))
	{
	}
	constexpr auto operator()(Key const & value) const -> decltype(auto) {
		return m_extract(value);
	}
	constexpr auto operator()(T const & value) const -> decltype(auto)
		requires(!std::same_as<Key, T>)
	{
		return m_extract(get_key(value));
	}
private:
	[[no_unique_address]] ExtractKey m_extract;
};

export template<bool allow_duplicates, typename Container>
constexpr auto merge_sorted_and_unsorted(Container & container, iterator_t<Container> midpoint, auto const extract_key) {
	auto const first = ::containers::begin(container);
	auto const last = ::containers::end(container);
	auto const compare = ::containers::extract_key_to_less(extract_key);
	ska_sort(subrange(midpoint, last), extract_key);
	if constexpr (allow_duplicates) {
		std::inplace_merge(
			maybe_legacy_iterator(first),
			maybe_legacy_iterator(midpoint),
			maybe_legacy_iterator(last),
			compare
		);
	} else {
		auto const position = ::containers::unique_inplace_merge(
			first,
			midpoint,
			last,
			compare
		);
		containers::erase_to_end(container, position);
	}
}

export template<typename Container, typename KeyType, typename ExtractKey, bool allow_duplicates>
struct flat_associative_base : private lexicographical_comparison::base {
	using value_type = range_value_t<Container>;
	using key_type = KeyType;

	using const_iterator = iterator_t<Container const &>;
	
	constexpr auto extract_key() const {
		return extract_key_t<value_type, key_type, ExtractKey>(m_extract_key);
	}
	constexpr auto key_comp() const {
		return ::containers::extract_key_to_less(extract_key());
	}
	
	flat_associative_base() = default;
	template<bounded::convertible_to<ExtractKey> EK>
	constexpr explicit flat_associative_base(EK extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}

	constexpr flat_associative_base(constructor_initializer_range<flat_associative_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		if constexpr (allow_duplicates) {
			ska_sort(m_container, extract_key());
		} else {
			unique_ska_sort(m_container, extract_key());
		}
	}
	constexpr explicit flat_associative_base(constructor_initializer_range<flat_associative_base> auto && source):
		flat_associative_base(OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_associative_base(assume_sorted_unique_t, constructor_initializer_range<flat_associative_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, key_comp()));
	}
	constexpr flat_associative_base(assume_sorted_unique_t, constructor_initializer_range<flat_associative_base> auto && source):
		flat_associative_base(assume_sorted_unique, OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_associative_base(assume_unique_t, constructor_initializer_range<flat_associative_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}
	constexpr flat_associative_base(assume_unique_t, constructor_initializer_range<flat_associative_base> auto && source):
		flat_associative_base(assume_unique, OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	template<std::size_t init_size>
	constexpr flat_associative_base(c_array<value_type, init_size> && source, ExtractKey extract_key_):
		m_container(std::move(source)),
		m_extract_key(std::move(extract_key_))
	{
		if constexpr (allow_duplicates) {
			ska_sort(m_container, extract_key());
		} else {
			unique_ska_sort(m_container, extract_key());
		}
	}
	template<std::size_t init_size>
	constexpr flat_associative_base(c_array<value_type, init_size> && source):
		flat_associative_base(std::move(source), ExtractKey())
	{
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr flat_associative_base(Source) {
	}

	template<std::size_t init_size>
	constexpr flat_associative_base(assume_sorted_unique_t, c_array<value_type, init_size> && source, ExtractKey extract_key_):
		m_container(std::move(source)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, key_comp()));
	}
	template<std::size_t init_size>
	constexpr flat_associative_base(assume_sorted_unique_t, c_array<value_type, init_size> && source):
		flat_associative_base(assume_sorted_unique, std::move(source), ExtractKey())
	{
	}
	constexpr flat_associative_base(assume_sorted_unique_t, empty_c_array_parameter, ExtractKey extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}
	constexpr flat_associative_base(assume_sorted_unique_t, empty_c_array_parameter) {
	}

	template<std::size_t init_size>
	constexpr flat_associative_base(assume_unique_t, c_array<value_type, init_size> && source, ExtractKey extract_key_):
		m_container(std::move(source)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}
	template<std::size_t init_size>
	constexpr flat_associative_base(assume_unique_t, c_array<value_type, init_size> && source):
		flat_associative_base(assume_unique, std::move(source), ExtractKey())
	{
	}
	constexpr flat_associative_base(assume_unique_t, empty_c_array_parameter, ExtractKey extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}
	constexpr flat_associative_base(assume_unique_t, empty_c_array_parameter) {
	}

	constexpr auto data() const requires contiguous_range<Container> {
		return ::containers::data(m_container);
	}
	constexpr auto data() requires contiguous_range<Container> {
		return ::containers::data(m_container);
	}
	constexpr auto begin() const {
		return ::containers::begin(m_container);
	}
	constexpr auto begin() {
		return ::containers::begin(m_container);
	}
	constexpr auto size() const {
		return ::containers::size(m_container);
	}
	
	constexpr auto capacity() const {
		return m_container.capacity();
	}
	constexpr auto replace_empty_allocation(range_size_t<Container> const requested_capacity) {
		return m_container.replace_empty_allocation(requested_capacity);
	}
	constexpr auto reserve(range_size_t<Container> const new_capacity) {
		return m_container.reserve(new_capacity);
	}
	
	// Unlike node containers, insert can only provide a time complexity that
	// matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	constexpr auto insert(auto && key, auto const make_value) {
		auto const position = containers::keyed_upper_bound(*this, key);
		auto add_element = [&] {
			return ::containers::lazy_insert(
				m_container,
				position,
				[&] { return make_value(OPERATORS_FORWARD(key)); }
			);
		};
		if constexpr (allow_duplicates) {
			return add_element();
		} else {
			bool const there_is_element_before = position != begin();
			if (!there_is_element_before) {
				return inserted_t{add_element(), true};
			}
			bool const that_element_is_equal = !key_comp()(*containers::prev(position), key);
			if (!that_element_is_equal) {
				return inserted_t{add_element(), true};
			}
			return inserted_t{mutable_iterator(*this, containers::prev(position)), false};
		}
	}

	constexpr auto insert_range(range auto && init) -> void {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all.
		auto const original_size = containers::size(m_container);
		::containers::append(m_container, OPERATORS_FORWARD(init));
		auto const midpoint = begin() + original_size;
		::containers::merge_sorted_and_unsorted<allow_duplicates>(m_container, midpoint, extract_key());
	}
	
	constexpr auto erase(const_iterator const it) {
		return containers::erase(m_container, it);
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) {
		return containers::erase(m_container, first, last);
	}
	constexpr auto erase_if(auto const predicate) {
		return containers::erase_if(m_container, predicate);
	}

protected:
	[[no_unique_address]] Container m_container;
	[[no_unique_address]] ExtractKey m_extract_key;
};

} // namespace containers
