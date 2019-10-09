// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/binary_search.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/integer.hpp>

#include <algorithm>
#include <stdexcept>

namespace containers {

template<typename Key, typename Mapped>
struct map_value_type {
//private:
	// Treat this as private. Public until we have C++20 `<=>` and `==`.
	bounded::tuple<Key, Mapped> m_data;

public:
	using key_type = Key;
	using mapped_type = Mapped;

	template<typename... Args> requires std::is_constructible_v<bounded::tuple<key_type, mapped_type>, Args...>
	constexpr map_value_type(Args && ... args):
		m_data(BOUNDED_FORWARD(args)...)
	{
	}
	
	constexpr auto && key() const & {
		return m_data[0_bi];
	}
	constexpr auto && key() & {
		return m_data[0_bi];
	}
	constexpr auto && key() && {
		return std::move(m_data)[0_bi];
	}
	constexpr auto && mapped() const & {
		return m_data[1_bi];
	}
	constexpr auto && mapped() & {
		return m_data[1_bi];
	}
	constexpr auto && mapped() && {
		return std::move(m_data)[1_bi];
	}

};

// These functions just become defaulted <=> and == in C++20.
template<typename Key, typename Mapped>
constexpr auto compare(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) {
	return compare(lhs.m_data, rhs.m_data);
}

template<typename Key, typename Mapped>
constexpr auto operator==(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) {
	return lhs.m_data == rhs.m_data;
}

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

// Key could be a reference type
template<typename Key, typename MappedFunction>
struct emplace_helper {
	explicit constexpr emplace_helper(Key key_, MappedFunction mapped_):
		m_key(BOUNDED_FORWARD(key_)),
		m_mapped(std::move(mapped_))
	{
	}

	auto && key() const & {
		return m_key;
	}
	auto && key() && {
		return BOUNDED_FORWARD(m_key);
	}
	auto mapped() && {
		return std::move(m_mapped);
	}

private:
	Key m_key;
	MappedFunction m_mapped;
};

template<typename Key, typename MappedFunction>
constexpr auto make_emplace_helper_key_ref(Key && key, MappedFunction mapped) {
	return emplace_helper<Key &&, MappedFunction>(
		BOUNDED_FORWARD(key),
		std::move(mapped)
	);
}

template<typename Key, typename Mapped>
constexpr auto make_emplace_helper() {
	return emplace_helper(
		Key(),
		bounded::construct_return<Mapped>
	);
}
// TODO: Think about how to handle transparent comparators better than this?
template<typename, typename, typename Pair>
constexpr auto make_emplace_helper(Pair && pair) {
	return make_emplace_helper_key_ref(
		BOUNDED_FORWARD(pair).key(),
		[&]{ return BOUNDED_FORWARD(pair).mapped(); }
	);
}
template<typename, typename, typename Key, typename Mapped>
constexpr auto make_emplace_helper(Key && key, Mapped && mapped) {
	return make_emplace_helper_key_ref(
		BOUNDED_FORWARD(key),
		[&]{ return BOUNDED_FORWARD(mapped); }
	);
}
template<typename, typename, typename KeyFunction, typename MappedFunction>
static constexpr auto make_emplace_helper(bounded::lazy_init_t, KeyFunction && key, MappedFunction && mapped) {
	if constexpr (std::is_reference_v<decltype(BOUNDED_FORWARD(key)())>) {
		return make_emplace_helper_key_ref(
			BOUNDED_FORWARD(key)(),
			BOUNDED_FORWARD(mapped)
		);
	} else {
		return emplace_helper(
			BOUNDED_FORWARD(key)(),
			BOUNDED_FORWARD(mapped)
		);
	}
}


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
struct flat_map_base {
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
	constexpr explicit flat_map_base(ExtractKey extract_key):
		m_extract_key(std::move(extract_key))
	{
	}

	template<range InputRange>
	constexpr explicit flat_map_base(InputRange && range):
		m_container(BOUNDED_FORWARD(range))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(InputRange && range, ExtractKey extract_key):
		m_container(BOUNDED_FORWARD(range)),
		m_extract_key(std::move(extract_key))
	{
		unique_ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(assume_sorted_unique_t, InputRange && range):
		m_container(BOUNDED_FORWARD(range))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<range InputRange>
	constexpr flat_map_base(assume_sorted_unique_t, InputRange && range, ExtractKey extract_key):
		m_container(BOUNDED_FORWARD(range)),
		m_extract_key(std::move(extract_key))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	template<range InputRange>
	constexpr flat_map_base(assume_unique_t, InputRange && range):
		m_container(BOUNDED_FORWARD(range))
	{
		ska_sort(m_container, extract_key());
	}

	template<range InputRange>
	constexpr flat_map_base(assume_unique_t, InputRange && range, ExtractKey extract_key):
		m_container(BOUNDED_FORWARD(range)),
		m_extract_key(std::move(extract_key))
	{
		ska_sort(m_container, extract_key());
	}



	constexpr flat_map_base(std::initializer_list<value_type> range):
		m_container(range)
	{
		unique_ska_sort(m_container, extract_key());
	}

	constexpr flat_map_base(std::initializer_list<value_type> range, ExtractKey extract_key):
		m_container(range),
		m_extract_key(std::move(extract_key))
	{
		unique_ska_sort(m_container, extract_key());
	}

	constexpr flat_map_base(assume_sorted_unique_t, std::initializer_list<value_type> range):
		m_container(range)
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	constexpr flat_map_base(assume_sorted_unique_t, std::initializer_list<value_type> range, ExtractKey extract_key):
		m_container(range),
		m_extract_key(std::move(extract_key))
	{
		BOUNDED_ASSERT(is_sorted(m_container, compare()));
	}

	constexpr flat_map_base(assume_unique_t, std::initializer_list<value_type> range):
		m_container(range)
	{
		ska_sort(m_container, extract_key());
	}

	constexpr flat_map_base(assume_unique_t, std::initializer_list<value_type> range, ExtractKey extract_key):
		m_container(range),
		m_extract_key(std::move(extract_key))
	{
		ska_sort(m_container, extract_key());
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		return *this = flat_map_base(init);
	}
	
	friend constexpr auto begin(flat_map_base const & c) {
		return begin(c.m_container);
	}
	friend constexpr auto begin(flat_map_base & c) {
		return begin(c.m_container);
	}
	friend constexpr auto end(flat_map_base const & c) {
		return end(c.m_container);
	}
	friend constexpr auto end(flat_map_base & c) {
		return end(c.m_container);
	}
	
	// Extra functions on top of the regular map interface
	constexpr auto capacity() const {
		return m_container.capacity();
	}
	constexpr auto reserve(size_type const new_capacity) {
		return m_container.reserve(new_capacity);
	}
	constexpr auto shrink_to_fit() {
		return m_container.shrink_to_fit();
	}
	
	template<typename Key>
	constexpr auto lower_bound(Key && key) const {
		return containers::lower_bound(
			*this,
			BOUNDED_FORWARD(key),
			compare()
		);
	}
	template<typename Key>
	constexpr auto lower_bound(Key && key) {
		return containers::lower_bound(
			*this,
			BOUNDED_FORWARD(key),
			compare()
		);
	}
	template<typename Key>
	constexpr auto upper_bound(Key && key) const {
		return containers::upper_bound(
			*this,
			BOUNDED_FORWARD(key),
			compare()
		);
	}
	template<typename Key>
	constexpr auto upper_bound(Key && key) {
		return containers::upper_bound(
			*this,
			BOUNDED_FORWARD(key),
			compare()
		);
	}

	template<typename Key>
	constexpr auto find(Key const & key) const {
		auto const it = lower_bound(key);
		return (it == end(*this) or compare()(key, it->key())) ? end(*this) : it;
	}
	template<typename Key>
	constexpr auto find(Key const & key) {
		auto const it = lower_bound(key);
		return (it == end(*this) or compare()(key, it->key())) ? end(*this) : it;
	}
	
	// Unlike in std::map, insert / emplace can only provide a time complexity
	// that matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	//
	// Moreover, emplace cannot in general provide the guarantee of no copying
	// or moving. It can only provide the weaker guarantee of no copying or
	// moving of the mapped_type. We must copy or move key_type because we have
	// to construct the key to determine whether we should insert it.
	template<typename... Args>
	constexpr auto emplace(Args && ... args) {
		auto data = ::containers::detail::make_emplace_helper<key_type, mapped_type>(BOUNDED_FORWARD(args)...);
		auto const position = upper_bound(data.key());
		return emplace_at(position, std::move(data));
	}
	template<typename... Args>
	constexpr auto emplace_hint(const_iterator hint, Args && ... args) {
		auto data = ::containers::detail::make_emplace_helper<key_type, mapped_type>(BOUNDED_FORWARD(args)...);
		auto const correct_greater = (hint == end(*this)) or compare()(data.key(), *hint);
		auto const correct_less = (hint == begin(*this)) or compare()(*::containers::prev(hint), data.key());
		auto const correct_hint = correct_greater and correct_less;
		auto const position = correct_hint ? hint : upper_bound(data.key());
		return emplace_at(position, std::move(data));
	}

	constexpr auto insert(value_type const & value) {
		return emplace(value);
	}
	constexpr auto insert(value_type && value) {
		return emplace(std::move(value));
	}
	constexpr auto insert(const_iterator const hint, value_type const & value) {
		return emplace_hint(hint, value);
	}
	constexpr auto insert(const_iterator const hint, value_type && value) {
		return emplace_hint(hint, std::move(value));
	}
	template<typename Range = std::initializer_list<value_type>>
	constexpr auto insert(Range && range) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const original_size = size(m_container);
		append(m_container, BOUNDED_FORWARD(range));
		auto const midpoint = begin(m_container) + original_size;

		ska_sort(midpoint, end(m_container), extract_key());
		if (allow_duplicates) {
			std::inplace_merge(
				make_legacy_iterator(begin(m_container)),
				make_legacy_iterator(midpoint),
				make_legacy_iterator(end(m_container)),
				compare()
			);
		} else {
			auto const position = ::containers::unique_inplace_merge(
				begin(m_container),
				midpoint,
				end(m_container),
				compare()
			);
			using containers::erase;
			erase(m_container, position, end(m_container));
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
	template<typename Emplacer>
	constexpr auto emplace_at(const_iterator position, Emplacer && emplacer) {
		auto add_element = [&] {
			return ::containers::emplace(
				m_container,
				position,
				bounded::lazy_init,
				[&]{ return std::move(emplacer).key(); },
				std::move(emplacer).mapped()
			);
		};
		if constexpr (allow_duplicates) {
			return add_element();
		} else {
			// Do not decrement an iterator if it might be begin(*this)
			bool const there_is_element_before = position != begin(*this);
			auto const that_element_is_equal = [&]{ return !compare()(containers::prev(position)->key(), emplacer.key()); };
			bool const already_exists = there_is_element_before and that_element_is_equal();
			if (already_exists) {
				return inserted_t{mutable_iterator(*this, containers::prev(position)), false};
			}
			return inserted_t{add_element(), true};
		}
	}
};

}	// namespace detail


template<typename Container, typename ExtractKey = detail::identity_t>
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
	
	friend constexpr auto begin(basic_flat_map const & container) {
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(basic_flat_map & container) {
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(basic_flat_map const & container) {
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(basic_flat_map & container) {
		return end(static_cast<base &>(container));
	}

	using base::capacity;
	using base::reserve;
	using base::shrink_to_fit;
	
	using base::lower_bound;
	using base::upper_bound;
	using base::find;
	
	using base::emplace;
	using base::emplace_hint;
	using base::insert;
	
	using base::erase;

	template<typename Key>
	constexpr auto const & at(Key && key) const {
		auto const it = find(BOUNDED_FORWARD(key));
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped();
	}
	template<typename Key>
	constexpr auto & at(Key && key) {
		auto const it = this->find(BOUNDED_FORWARD(key));
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped();
	}
	template<typename Key>
	constexpr auto & operator[](Key && key) {
		return this->emplace(std::piecewise_construct, bounded::tie(BOUNDED_FORWARD(key)), bounded::tie()).first->mapped();
	}

	template<typename Key>
	constexpr auto equal_range(Key && key) const {
		auto const it = find(BOUNDED_FORWARD(key));
		bool const found = it != end(*this);
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}

	template<typename Key>
	constexpr size_type count(Key && key) const {
		bool const found = this->find(BOUNDED_FORWARD(key)) != end(*this);
		return found ? 1 : 0;
	}

	template<typename Key>
	constexpr size_type erase(Key && key) {
		auto const it = this->find(BOUNDED_FORWARD(key));
		if (it == end(*this)) {
			return 0;
		}
		this->erase(it);
		return 1;
	}
};

template<typename Container, typename ExtractKey>
inline constexpr auto is_container<basic_flat_map<Container, ExtractKey>> = is_container<Container>;

template<typename Container, typename ExtractKey = detail::identity_t>
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
	
	friend constexpr auto begin(basic_flat_multimap const & container) {
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(basic_flat_multimap & container) {
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(basic_flat_multimap const & container) {
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(basic_flat_multimap & container) {
		return end(static_cast<base &>(container));
	}

	using base::capacity;
	using base::reserve;
	using base::shrink_to_fit;
	
	using base::lower_bound;
	using base::upper_bound;
	using base::find;
	
	using base::emplace;
	using base::emplace_hint;
	using base::insert;
	
	using base::erase;

	// These implementations work for map or multimap, but I don't expect the
	// compiler to be able to optimize based on the fact that values in flat_map
	// are unique, so I have slightly different versions in flat_map.

	template<typename Key>
	constexpr auto equal_range(Key && key) const {
		return std::equal_range(begin(*this), end(*this), BOUNDED_FORWARD(key), compare());
	}
	template<typename Key>
	constexpr auto equal_range(Key && key) {
		return std::equal_range(begin(*this), end(*this), BOUNDED_FORWARD(key), compare);
	}

	template<typename Key>
	constexpr auto count(Key && key) const {
		auto const range = this->equal_range(BOUNDED_FORWARD(key));
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	template<typename Key>
	constexpr size_type erase(Key && key) {
		auto const range = this->equal_range(BOUNDED_FORWARD(key));
		if (range.first == end(*this)) {
			return 0;
		}
		auto const distance = std::distance(range.first, range.second);
		this->erase(range.first, range.second);
		return distance;
	}
};

template<typename Container, typename ExtractKey>
inline constexpr auto is_container<basic_flat_multimap<Container, ExtractKey>> = is_container<Container>;


template<typename Key, typename T, typename... MaybeExtractKey>
using flat_map = basic_flat_map<vector<map_value_type<Key, T>>, MaybeExtractKey...>;

template<typename Key, typename T, typename... MaybeExtractKey>
using flat_multimap = basic_flat_multimap<vector<map_value_type<Key, T>>, MaybeExtractKey...>;


}	// namespace containers
