// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/binary_search.hpp>
#include <containers/algorithms/erase.hpp>
#include <containers/algorithms/is_sorted.hpp>
#include <containers/algorithms/keyed_binary_search.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/insert.hpp>
#include <containers/iterator_t.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/lookup.hpp>
#include <containers/map_value_type.hpp>
#include <containers/range_value_t.hpp>
#include <containers/size.hpp>
#include <containers/static_vector.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/integer.hpp>
#include <bounded/concepts.hpp>

#include <operators/forward.hpp>

#include <algorithm>

namespace containers {

constexpr inline struct assume_sorted_unique_t {} assume_sorted_unique;
constexpr inline struct assume_unique_t {} assume_unique;

namespace detail {

template<typename Iterator>
struct inserted_t {
	Iterator iterator;
	bool inserted;
};

template<typename Iterator>
inserted_t(Iterator, bool) -> inserted_t<Iterator>;

template<typename T, typename ExtractKey>
struct extract_map_key {
	constexpr explicit extract_map_key(ExtractKey extract_key):
		m_extract(std::move(extract_key))
	{
	}
	constexpr decltype(auto) operator()(T const & value) const {
		return m_extract(get_key(value));
	}
	constexpr decltype(auto) operator()(typename T::key_type const & key) const {
		return m_extract(key);
	}
private:
	ExtractKey m_extract;
};


template<typename ExtractKey, typename T>
concept extract_key_function = requires(ExtractKey const & extract_key, T const & value) {
	extract_key(value);
};

template<bool allow_duplicates, typename Container>
constexpr auto merge_sorted_and_unsorted(Container & container, iterator_t<Container> midpoint, auto const extract_key) {
	auto const first = ::containers::begin(container);
	auto const last = ::containers::end(container);
	auto const compare = ::containers::detail::extract_key_to_less(extract_key);
	ska_sort(containers::range_view(midpoint, last), extract_key);
	if constexpr (allow_duplicates) {
		std::inplace_merge(
			make_legacy_iterator(first),
			make_legacy_iterator(midpoint),
			make_legacy_iterator(last),
			compare
		);
	} else {
		auto const position = ::containers::unique_inplace_merge(
			first,
			midpoint,
			last,
			compare
		);
		containers::erase_after(container, position);
	}
}

// The exact type of value_type should be considered implementation defined.
// map_value_type<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks.

template<typename Container, extract_key_function<typename range_value_t<Container>::key_type> ExtractKey, bool allow_duplicates>
struct flat_map_base : private lexicographical_comparison::base {
	using value_type = range_value_t<Container>;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;

	using const_iterator = iterator_t<Container const &>;
	
	constexpr auto extract_key() const {
		return extract_map_key<value_type, ExtractKey>(m_extract_key);
	}
	constexpr auto compare() const {
		return ::containers::detail::extract_key_to_less(extract_key());
	}
	
	flat_map_base() = default;
	constexpr explicit flat_map_base(ExtractKey extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}

	constexpr flat_map_base(initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		if constexpr (allow_duplicates) {
			ska_sort(m_container, extract_key());
		} else {
			unique_ska_sort(m_container, extract_key());
		}
	}
	constexpr explicit flat_map_base(initializer_range<flat_map_base> auto && source):
		flat_map_base(OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_map_base(assume_sorted_unique_t, initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}
	constexpr flat_map_base(assume_sorted_unique_t, initializer_range<flat_map_base> auto && source):
		flat_map_base(assume_sorted_unique, OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_map_base(assume_unique_t, initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}
	constexpr flat_map_base(assume_unique_t, initializer_range<flat_map_base> auto && source):
		flat_map_base(assume_unique, OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	template<std::size_t init_size>
	constexpr flat_map_base(c_array<value_type, init_size> && source, ExtractKey extract_key_):
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
	constexpr flat_map_base(c_array<value_type, init_size> && source):
		flat_map_base(std::move(source), ExtractKey())
	{
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_sorted_unique_t, c_array<value_type, init_size> && source, ExtractKey extract_key_):
		m_container(std::move(source)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}
	template<std::size_t init_size>
	constexpr flat_map_base(assume_sorted_unique_t, c_array<value_type, init_size> && source):
		flat_map_base(assume_sorted_unique, std::move(source), ExtractKey())
	{
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_unique_t, c_array<value_type, init_size> && source, ExtractKey extract_key_):
		m_container(std::move(source)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}
	template<std::size_t init_size>
	constexpr flat_map_base(assume_unique_t, c_array<value_type, init_size> && source):
		flat_map_base(assume_unique, std::move(source), ExtractKey())
	{
	}

	constexpr auto begin() const {
		return ::containers::begin(m_container);
	}
	constexpr auto begin() {
		return ::containers::begin(m_container);
	}
	constexpr auto end() const {
		return ::containers::end(m_container);
	}
	constexpr auto end() {
		return ::containers::end(m_container);
	}
	
	constexpr auto capacity() const {
		return m_container.capacity();
	}
	constexpr auto reserve(range_size_t<Container> const new_capacity) {
		return m_container.reserve(new_capacity);
	}
	
	// Unlike in std::map, insert can only provide a time complexity that
	// matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	constexpr auto lazy_insert(auto && key, bounded::construct_function_for<mapped_type> auto && mapped) {
		auto const position = containers::keyed_upper_bound(*this, key);
		auto add_element = [&] {
			return ::containers::lazy_insert(
				m_container,
				position,
				[&] { return value_type{OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)()}; }
			);
		};
		if constexpr (allow_duplicates) {
			return add_element();
		} else {
			bool const there_is_element_before = position != begin();
			if (!there_is_element_before) {
				return inserted_t{add_element(), true};
			}
			bool const that_element_is_equal = !compare()(get_key(*containers::prev(position)), key);
			if (!that_element_is_equal) {
				return inserted_t{add_element(), true};
			}
			return inserted_t{mutable_iterator(*this, containers::prev(position)), false};
		}
	}

	template<range Range>
	constexpr auto insert(Range && init) -> void {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all.
		auto const original_size = containers::size(m_container);
		::containers::append(m_container, OPERATORS_FORWARD(init));
		auto const midpoint = begin() + original_size;
		::containers::detail::merge_sorted_and_unsorted<allow_duplicates>(m_container, midpoint, extract_key());
	}
	
	constexpr auto erase(const_iterator const it) {
		return containers::erase(m_container, it);
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) {
		return containers::erase(m_container, first, last);
	}

private:
	Container m_container;
	[[no_unique_address]] ExtractKey m_extract_key;
};

}	// namespace detail


template<typename Container, detail::extract_key_function<typename range_value_t<Container>::key_type> ExtractKey = to_radix_sort_key_t>
class basic_flat_map : private detail::flat_map_base<Container, ExtractKey, false> {
private:
	using base = detail::flat_map_base<Container, ExtractKey, false>;
public:
	using typename base::key_type;
	using typename base::mapped_type;

	using base::compare;
	using base::extract_key;

	using base::base;

	using base::begin;
	using base::end;

	using base::capacity;
	using base::reserve;
	
	using base::lazy_insert;
	using base::insert;
	
	using base::erase;

	constexpr auto find(auto const & key) const {
		auto const it = containers::keyed_lower_bound(*this, key);
		return (it == end() or compare()(key, get_key(*it))) ? end() : it;
	}
	constexpr auto find(auto const & key) {
		auto const it = containers::keyed_lower_bound(*this, key);
		return (it == end() or compare()(key, get_key(*it))) ? end() : it;
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


template<typename Container, detail::extract_key_function<typename range_value_t<Container>::key_type> ExtractKey = to_radix_sort_key_t>
class basic_flat_multimap : private detail::flat_map_base<Container, ExtractKey, true> {
private:
	using base = detail::flat_map_base<Container, ExtractKey, true>;
public:
	using typename base::key_type;
	using typename base::mapped_type;

	using base::compare;
	using base::extract_key;

	using base::base;
	
	using base::begin;
	using base::end;

	using base::capacity;
	using base::reserve;
	
	using base::lazy_insert;
	using base::insert;
	
	using base::erase;
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


template<typename Key, typename T, typename... MaybeExtractKey>
using flat_map = basic_flat_map<vector<map_value_type<Key, T>>, MaybeExtractKey...>;

template<typename Key, typename T, std::size_t capacity, typename... MaybeExtractKey>
using static_flat_map = basic_flat_map<static_vector<map_value_type<Key, T>, capacity>, MaybeExtractKey...>;

template<typename Key, typename T, typename... MaybeExtractKey>
using flat_multimap = basic_flat_multimap<vector<map_value_type<Key, T>>, MaybeExtractKey...>;

template<typename Key, typename T, std::size_t capacity, typename... MaybeExtractKey>
using static_flat_multimap = basic_flat_multimap<static_vector<map_value_type<Key, T>, capacity>, MaybeExtractKey...>;


}	// namespace containers
