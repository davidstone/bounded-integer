// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/binary_search.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/erase.hpp>
#include <containers/insert.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/lookup.hpp>
#include <containers/size.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/integer.hpp>
#include <bounded/concepts.hpp>

#include <operators/forward.hpp>

#include <algorithm>

namespace containers {

template<typename Key, typename Mapped>
struct map_value_type {
	using key_type = Key;
	using mapped_type = Mapped;

	constexpr map_value_type(Key key_, Mapped mapped_):
		m_key(std::move(key_)),
		m_mapped(std::move(mapped_))
	{
	}
	constexpr map_value_type(
		bounded::lazy_init_t,
		bounded::construct_function_for<Key> auto && key_,
		bounded::construct_function_for<Mapped> auto && mapped_
	):
		m_key(OPERATORS_FORWARD(key_)()),
		m_mapped(OPERATORS_FORWARD(mapped_)())
	{
	}
	
	constexpr auto && key() const & {
		return m_key;
	}
	constexpr auto && key() & {
		return m_key;
	}
	constexpr auto && key() && {
		return std::move(m_key);
	}
	constexpr auto && mapped() const & {
		return m_mapped;
	}
	constexpr auto && mapped() & {
		return m_mapped;
	}
	constexpr auto && mapped() && {
		return std::move(m_mapped);
	}

	friend auto operator<=>(map_value_type const & lhs, map_value_type const & rhs) = default;

private:
	[[no_unique_address]] Key m_key;
	[[no_unique_address]] Mapped m_mapped;
};

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
		m_extract(extract_key)
	{
	}
	constexpr decltype(auto) operator()(T const & value) const {
		return m_extract(value.key());
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


// The exact type of value_type should be considered implementation defined.
// map_value_type<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks.

template<typename Container, extract_key_function<typename Container::value_type::key_type> ExtractKey, bool allow_duplicates>
struct flat_map_base : private lexicographical_comparison::base {
private:
	Container m_container;
	[[no_unique_address]] ExtractKey m_extract_key;
public:
	using value_type = typename Container::value_type;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;
	using size_type = typename Container::size_type;

	using const_iterator = typename Container::const_iterator;
	using iterator = typename Container::iterator;
	
	constexpr auto extract_key() const {
		return extract_map_key<value_type, ExtractKey>(m_extract_key);
	}
	constexpr auto compare() const {
		return ::containers::detail::extract_key_to_compare(extract_key());
	}
	
	flat_map_base() = default;
	constexpr explicit flat_map_base(ExtractKey extract_key_):
		m_extract_key(std::move(extract_key_))
	{
	}

	template<range InputRange>
	constexpr explicit flat_map_base(InputRange && init):
		m_container(OPERATORS_FORWARD(init))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(InputRange && init, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(init)),
		m_extract_key(std::move(extract_key_))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(assume_sorted_unique_t, InputRange && init):
		m_container(OPERATORS_FORWARD(init))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<range InputRange>
	constexpr flat_map_base(assume_sorted_unique_t, InputRange && init, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(init)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<range InputRange>
	constexpr flat_map_base(assume_unique_t, InputRange && init):
		m_container(OPERATORS_FORWARD(init))
	{
		ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(assume_unique_t, InputRange && init, ExtractKey extract_key_):
		m_container(OPERATORS_FORWARD(init)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}


	template<std::size_t init_size>
	constexpr flat_map_base(c_array<value_type, init_size> && init):
		m_container(std::move(init))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<std::size_t init_size>
	constexpr flat_map_base(c_array<value_type, init_size> && init, ExtractKey extract_key_):
		m_container(std::move(init)),
		m_extract_key(std::move(extract_key_))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_sorted_unique_t, c_array<value_type, init_size> && init):
		m_container(std::move(init))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_sorted_unique_t, c_array<value_type, init_size> && init, ExtractKey extract_key_):
		m_container(std::move(init)),
		m_extract_key(std::move(extract_key_))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_unique_t, c_array<value_type, init_size> && init):
		m_container(std::move(init))
	{
		ska_sort(m_container, extract_key());
	}

	template<std::size_t init_size>
	constexpr flat_map_base(assume_unique_t, c_array<value_type, init_size> && init, ExtractKey extract_key_):
		m_container(std::move(init)),
		m_extract_key(std::move(extract_key_))
	{
		ska_sort(m_container, extract_key());
	}

	constexpr auto begin() const & {
		return ::containers::begin(m_container);
	}
	constexpr auto begin() & {
		return ::containers::begin(m_container);
	}
	constexpr auto begin() && {
		return ::containers::begin(std::move(m_container));
	}
	constexpr auto end() const & {
		return ::containers::end(m_container);
	}
	constexpr auto end() & {
		return ::containers::end(m_container);
	}
	constexpr auto end() && {
		return ::containers::end(std::move(m_container));
	}
	
	// Extra functions on top of the regular map interface
	constexpr auto capacity() const {
		return m_container.capacity();
	}
	constexpr auto reserve(size_type const new_capacity) {
		return m_container.reserve(new_capacity);
	}
	
	constexpr auto lower_bound(auto && key) const {
		return containers::lower_bound(
			*this,
			OPERATORS_FORWARD(key),
			compare()
		);
	}
	constexpr auto lower_bound(auto && key) {
		return containers::lower_bound(
			*this,
			OPERATORS_FORWARD(key),
			compare()
		);
	}
	constexpr auto upper_bound(auto && key) const {
		return containers::upper_bound(
			*this,
			OPERATORS_FORWARD(key),
			compare()
		);
	}
	constexpr auto upper_bound(auto && key) {
		return containers::upper_bound(
			*this,
			OPERATORS_FORWARD(key),
			compare()
		);
	}

	// Unlike in std::map, insert / try_emplace can only provide a time
	// complexity that matches an insert into the underlying container, which is
	// to say, linear. An insertion implies shifting all of the elements.
	constexpr auto try_emplace(auto && key, auto && ... mapped_args) {
		auto const position = upper_bound(key);
		return try_emplace_at(position, OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped_args)...);
	}

	constexpr auto insert(value_type const & value) {
		return try_emplace(value.key(), value.mapped());
	}
	constexpr auto insert(value_type && value) {
		return try_emplace(std::move(value).key(), std::move(value).mapped());
	}
	template<range Range>
	constexpr auto insert(Range && init) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const original_size = containers::size(m_container);
		::containers::append(m_container, OPERATORS_FORWARD(init));
		auto const midpoint = begin() + original_size;

		ska_sort(midpoint, end(), extract_key());
		if constexpr (allow_duplicates) {
			std::inplace_merge(
				make_legacy_iterator(begin()),
				make_legacy_iterator(midpoint),
				make_legacy_iterator(end()),
				compare()
			);
		} else {
			auto const position = ::containers::unique_inplace_merge(
				begin(),
				midpoint,
				end(),
				compare()
			);
			containers::erase(m_container, position, end());
		}
	}
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	constexpr auto erase(const_iterator const it) {
		return m_container.erase(it);
	}
	// Need mutable iterator overload to avoid ambiguity if key_type can be
	// constructed from iterator (for instance, an unconstrained constructor
	// template).
	constexpr auto erase(iterator const it) {
		return m_container.erase(it);
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) {
		return m_container.erase(first, last);
	}
	
private:
	constexpr auto try_emplace_at(const_iterator position, auto && key, auto && ... mapped_args) {
		auto add_element = [&] {
			return ::containers::emplace(
				m_container,
				position,
				bounded::lazy_init,
				[&]{ return key_type(OPERATORS_FORWARD(key)); },
				[&]{ return mapped_type(OPERATORS_FORWARD(mapped_args)...); }
			);
		};
		if constexpr (allow_duplicates) {
			return add_element();
		} else {
			bool const there_is_element_before = position != begin();
			if (!there_is_element_before) {
				return inserted_t{add_element(), true};
			}
			bool const that_element_is_equal = !compare()(containers::prev(position)->key(), key);
			if (!that_element_is_equal) {
				return inserted_t{add_element(), true};
			}
			return inserted_t{mutable_iterator(*this, containers::prev(position)), false};
		}
	}
};

}	// namespace detail


template<typename Container, typename ExtractKey = bounded::identity_t>
class basic_flat_map : private detail::flat_map_base<Container, ExtractKey, false> {
private:
	using base = detail::flat_map_base<Container, ExtractKey, false>;
public:
	using typename base::key_type;
	using typename base::mapped_type;
	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using base::compare;

	using base::base;
	using base::operator=;

	using base::begin;
	using base::end;

	using base::capacity;
	using base::reserve;
	
	using base::lower_bound;
	using base::upper_bound;
	
	using base::try_emplace;
	using base::insert;
	
	using base::erase;

	constexpr auto find(auto const & key) const {
		auto const it = lower_bound(key);
		return (it == end() or compare()(key, it->key())) ? end() : it;
	}
	constexpr auto find(auto const & key) {
		auto const it = lower_bound(key);
		return (it == end() or compare()(key, it->key())) ? end() : it;
	}
	
	constexpr auto equal_range(auto && key) const {
		auto const it = find(OPERATORS_FORWARD(key));
		bool const found = it != end();
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}

	constexpr size_type count(auto && key) const {
		bool const found = this->find(OPERATORS_FORWARD(key)) != end();
		return found ? 1 : 0;
	}

	constexpr size_type erase(auto && key) {
		auto const it = this->find(OPERATORS_FORWARD(key));
		if (it == end()) {
			return 0;
		}
		this->erase(it);
		return 1;
	}
};

template<typename Container, typename ExtractKey = bounded::identity_t>
class basic_flat_multimap : private detail::flat_map_base<Container, ExtractKey, true> {
private:
	using base = detail::flat_map_base<Container, ExtractKey, true>;
public:
	using typename base::key_type;
	using typename base::mapped_type;
	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using base::compare;

	using base::base;
	using base::operator=;
	
	using base::begin;
	using base::end;

	using base::capacity;
	using base::reserve;
	
	using base::lower_bound;
	using base::upper_bound;
	
	using base::try_emplace;
	using base::insert;
	
	using base::erase;

	// These implementations work for map or multimap, but I don't expect the
	// compiler to be able to optimize based on the fact that values in flat_map
	// are unique, so I have slightly different versions in flat_map.

	constexpr auto equal_range(auto && key) const {
		return std::equal_range(begin(), end(), OPERATORS_FORWARD(key), compare());
	}
	constexpr auto equal_range(auto && key) {
		return std::equal_range(begin(), end(), OPERATORS_FORWARD(key), compare());
	}

	constexpr auto count(auto && key) const {
		auto const range = this->equal_range(OPERATORS_FORWARD(key));
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	constexpr size_type erase(auto && key) {
		auto const range = this->equal_range(OPERATORS_FORWARD(key));
		if (range.first == end()) {
			return 0;
		}
		auto const distance = std::distance(range.first, range.second);
		this->erase(range.first, range.second);
		return distance;
	}
};


template<typename Key, typename T, typename... MaybeExtractKey>
using flat_map = basic_flat_map<vector<map_value_type<Key, T>>, MaybeExtractKey...>;

template<typename Key, typename T, typename... MaybeExtractKey>
using flat_multimap = basic_flat_multimap<vector<map_value_type<Key, T>>, MaybeExtractKey...>;


}	// namespace containers
