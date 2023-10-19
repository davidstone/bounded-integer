// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.flat_map;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.ska_sort;
import containers.algorithms.sort.to_radix_sort_key;

import containers.algorithms.advance;
import containers.algorithms.binary_search;
import containers.algorithms.erase;
import containers.algorithms.keyed_binary_search;
import containers.algorithms.unique;
import containers.append;
import containers.associative_container;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.data;
import containers.dereference;
import containers.extract_key_to_less;
import containers.initializer_range;
import containers.insert;
import containers.iterator_t;
import containers.legacy_iterator;
import containers.lookup;
import containers.map_tags;
import containers.map_value_type;
import containers.maximum_array_size;
import containers.mutable_iterator;
import containers.push_back;
import containers.range;
import containers.range_value_t;
import containers.range_view;
import containers.size;
import containers.static_vector;
import containers.vector;
export import containers.common_iterator_functions;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

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
	auto const compare = ::containers::extract_key_to_less(extract_key);
	ska_sort(range_view(midpoint, last), extract_key);
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
		containers::erase_to_end(container, position);
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
		return ::containers::extract_key_to_less(extract_key());
	}
	
	flat_map_base() = default;
	constexpr explicit flat_map_base(ExtractKey extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}

	constexpr flat_map_base(constructor_initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		if constexpr (allow_duplicates) {
			ska_sort(m_container, extract_key());
		} else {
			unique_ska_sort(m_container, extract_key());
		}
	}
	constexpr explicit flat_map_base(constructor_initializer_range<flat_map_base> auto && source):
		flat_map_base(OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_map_base(assume_sorted_unique_t, constructor_initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}
	constexpr flat_map_base(assume_sorted_unique_t, constructor_initializer_range<flat_map_base> auto && source):
		flat_map_base(assume_sorted_unique, OPERATORS_FORWARD(source), ExtractKey())
	{
	}

	constexpr flat_map_base(assume_unique_t, constructor_initializer_range<flat_map_base> auto && source, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(source)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}
	constexpr flat_map_base(assume_unique_t, constructor_initializer_range<flat_map_base> auto && source):
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
	constexpr auto reserve(range_size_t<Container> const new_capacity) {
		return m_container.reserve(new_capacity);
	}
	
	// Unlike in std::map, insert can only provide a time complexity that
	// matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	template<typename Key = key_type>
	constexpr auto lazy_insert(Key && key, bounded::construct_function_for<mapped_type> auto && mapped) {
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

	constexpr auto insert(range auto && init) -> void {
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
	// TODO: Use [[no_unique_address]] after resolution of
	// https://github.com/llvm/llvm-project/issues/53059
	Container m_container;
	[[no_unique_address]] ExtractKey m_extract_key;
};


export template<typename Container, extract_key_function<typename range_value_t<Container>::key_type> ExtractKey = to_radix_sort_key_t>
class basic_flat_map : private flat_map_base<Container, ExtractKey, false> {
private:
	using base = flat_map_base<Container, ExtractKey, false>;
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
	using base::reserve;
	
	using base::lazy_insert;
	using base::insert;
	
	using base::erase;
	using base::erase_if;

	constexpr auto find(auto const & key) const {
		auto const it = containers::keyed_lower_bound(*this, key);
		return (it == ::containers::end(*this) or compare()(key, get_key(*it))) ? ::containers::end(*this) : it;
	}
	constexpr auto find(auto const & key) {
		auto const it = containers::keyed_lower_bound(*this, key);
		return (it == ::containers::end(*this) or compare()(key, get_key(*it))) ? ::containers::end(*this) : it;
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
class basic_flat_multimap : private flat_map_base<Container, ExtractKey, true> {
private:
	using base = flat_map_base<Container, ExtractKey, true>;
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
