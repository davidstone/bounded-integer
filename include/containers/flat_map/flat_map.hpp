// A map-like class that has better locality of reference
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/apply_tuple.hpp>
#include <containers/algorithms/iterator.hpp>
#include <containers/algorithms/unique_inplace_merge.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/moving_vector/moving_vector.hpp>
#include <containers/vector/vector.hpp>

#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace containers {
namespace detail {
// The exact type of value_type should be considered implementation defined.
// std::pair<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks. However, we can leave them in place in the case that we
// are working with a moving_vector.
//
// The allocator should never change the value_type of a container, so we just
// use the default to prevent infinite recursion in our flat_map template
// parameter.
template<typename Key, typename T, template<typename, typename> class Container>
constexpr bool supports_const_key() {
	using pair_type = std::pair<Key const, T>;
	using container_type = Container<pair_type, std::allocator<pair_type>>;
	using value_type = std::remove_reference_t<decltype(*moving_begin(std::declval<container_type>()))>;
	return std::is_copy_assignable<typename value_type::first_type>::value;
}

template<typename Key, typename T, template<typename, typename> class Container>
using value_type_t = std::pair<
	typename std::conditional_t<supports_const_key<Key, T, Container>(), Key const, Key>,
	T
>;

template<typename Key, typename T, typename Compare, template<typename, typename> class Container, typename Allocator, bool allow_duplicates>
class flat_map_base {
public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = value_type_t<key_type, mapped_type, Container>;
	using allocator_type = Allocator;
private:
	using container_type = Container<value_type, allocator_type>;
public:
	using size_type = typename container_type::size_type;

	using const_iterator = typename container_type::const_iterator;
	using iterator = typename container_type::iterator;
	
	using key_compare = Compare;
	class value_compare {
	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;
		constexpr bool operator()(value_type const & lhs, value_type const & rhs) const {
			return m_compare(lhs.first, rhs.first);
		}
	protected:
		friend class flat_map_base;
		value_compare(key_compare c):
			m_compare(std::move(c)) {
		}
		key_compare m_compare;
	};
	key_compare key_comp() const {
		return std::get<1>(m_container);
	}
	value_compare value_comp() const {
		return value_compare(key_comp());
	}
	
	explicit flat_map_base(key_compare const & compare = key_compare{}, allocator_type allocator = allocator_type{}):
		m_container(container_type(std::move(allocator)), compare) {
	}
	explicit flat_map_base(allocator_type allocator):
		flat_map_base(key_compare{}, std::move(allocator)) {
	}
	template<typename InputIterator>
	flat_map_base(InputIterator first, InputIterator last, key_compare const & compare = key_compare{}, allocator_type allocator = allocator_type{}):
		m_container(container_type(first, last, std::move(allocator)), compare)
	{
		auto const less = indirect_compare(value_comp());
		std::sort(moving_begin(container()), moving_end(container()), less);
		// At some point this should be unique_sort
		auto const equal = [&](auto const & lhs, auto const & rhs) {
			return !less(lhs, rhs) and !less(rhs, lhs);
		};
		::containers::erase(
			container(),
			moving_to_standard_iterator(::containers::detail::unique(moving_begin(container()), moving_end(container()), equal)),
			container().end()
		);
	}
	template<typename InputIterator>
	flat_map_base(InputIterator first, InputIterator last, allocator_type allocator):
		flat_map_base(first, last, key_compare{}, std::move(allocator)) {
	}
	flat_map_base(flat_map_base const & other, allocator_type allocator):
		m_container(container_type(other, std::move(allocator)), key_compare{}) {
	}
	flat_map_base(flat_map_base && other, allocator_type allocator):
		m_container(container_type(std::move(other), std::move(allocator)), key_compare{}) {
	}
	flat_map_base(std::initializer_list<value_type> init, key_compare const & compare = key_compare{}, allocator_type allocator = allocator_type{}):
		flat_map_base(std::begin(init), std::end(init), compare, std::move(allocator)) {
	}
	flat_map_base(std::initializer_list<value_type> init, allocator_type allocator):
		flat_map_base(init, key_compare{}, std::move(allocator)) {
	}
	
	flat_map_base & operator=(std::initializer_list<value_type> init) {
		return *this = flat_map_base(init);
	}
	
	const_iterator begin() const noexcept {
		return container().begin();
	}
	iterator begin() noexcept {
		return container().begin();
	}
	
	const_iterator end() const noexcept {
		return container().end();
	}
	iterator end() noexcept {
		return container().end();
	}
	
	// Extra functions on top of the regular map interface
	size_type capacity() const noexcept {
		return container().capacity();
	}
	void reserve(size_type const new_capacity) noexcept {
		return container().reserve(new_capacity);
	}
	void shink_to_fit() {
		container().shrink_to_fit();
	}
	
	const_iterator lower_bound(key_type const & key) const {
		return std::lower_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	iterator lower_bound(key_type const & key) {
		return std::lower_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	const_iterator upper_bound(key_type const & key) const {
		return std::upper_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	iterator upper_bound(key_type const & key) {
		return std::upper_bound(begin(), end(), key, key_value_compare{key_comp()});
	}
	const_iterator find(key_type const & key) const {
		auto const it = lower_bound(key);
		return (it == end() or key_comp()(key, it->first)) ? end() : it;
	}
	iterator find(key_type const & key) {
		auto const it = lower_bound(key);
		return (it == end() or key_comp()(key, it->first)) ? end() : it;
	}
	
	// Unlike in std::map, insert / emplace can only provide a time complexity
	// that matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	//
	// Moreover, emplace cannot in general provide the guarantee of no copying
	// or moving. It can only provide the weaker guarantee of no copying or
	// moving of the mapped_type. If the underlying container is a
	// moving_vector, however, then we can perform the operation with no moving
	// of the value_type. In many cases, however, we will have to copy / move
	// the key_type, because we have to construct the key to determine whether
	// we should insert it.
	template<typename... Args>
	std::pair<iterator, bool> emplace(Args && ... args) {
		auto const search_strategy = [&](key_type const & key) {
			return this->upper_bound(key);
		};
		return emplace_search(search_strategy, std::forward<Args>(args)...);
	}
	template<typename... Args>
	iterator emplace_hint(const_iterator hint, Args && ... args) {
		auto const search_strategy = [&](key_type const & key) {
			bool const correct_greater = this->key_comp(key, *hint);
			bool const correct_less = this->key_comp(*::containers::prev(hint), key);
			bool const correct_hint = correct_greater and correct_less;
			return correct_hint ? hint : this->upper_bound(key);
		};
		return emplace_search(search_strategy, std::forward<Args>(args)...).first;
	}

	decltype(auto) insert(value_type const & value) {
		return emplace(value);
	}
	decltype(auto) insert(value_type && value) {
		return emplace(std::move(value));
	}
	decltype(auto) insert(const_iterator const hint, value_type const & value) {
		return emplace_hint(hint, value);
	}
	decltype(auto) insert(const_iterator const hint, value_type && value) {
		return emplace_hint(hint, std::move(value));
	}
	template<typename InputIterator>
	void insert(InputIterator first, InputIterator const last) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const const_midpoint = container().insert(container().end(), first, last);
		auto const midpoint = detail::make_moving_iterator(container(), const_midpoint);
		std::sort(midpoint, moving_end(container()), indirect_compare{value_comp()});
		if (allow_duplicates) {
			std::inplace_merge(
				legacy_iterator(moving_begin(container())),
				legacy_iterator(midpoint),
				legacy_iterator(moving_end(container())),
				indirect_compare(value_comp())
			);
		}
		else {
			auto const position = detail::unique_inplace_merge(
				moving_begin(container()),
				midpoint,
				moving_end(container()),
				indirect_compare(value_comp())
			);
			using containers::erase;
			erase(container(), position, moving_end(container()));
		}
	}
	void insert(std::initializer_list<value_type> init) {
		insert(std::begin(init), std::end(init));
	}
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	iterator erase(const_iterator const it) {
		return container().erase(it);
	}
	// Need mutable iterator overload to avoid ambiguity if key_type can be
	// constructed from iterator (for instance, an unconstrained constructor
	// template).
	iterator erase(iterator const it) {
		return container().erase(it);
	}
	iterator erase(const_iterator const first, const_iterator const last) {
		return container().erase(first, last);
	}
	
protected:
	class key_value_compare {
	public:
		constexpr key_value_compare(key_compare const & compare):
			m_compare(compare) {
		}
		constexpr bool operator()(key_type const & key, value_type const & value) const {
			return m_compare(key, value.first);
		}
		constexpr bool operator()(value_type const & value, key_type const & key) const {
			return m_compare(value.first, key);
		}
	private:
		key_compare const & m_compare;
	};
private:
	// It is safe to bind the reference to the object that is being moved in any
	// of these calls to emplace_key because the call to std::move does not
	// actually move anything, it just converts it to an rvalue reference. The
	// object isn't actually moved from until later on in the function.
	//
	// Search represents a function that finds where to insert
	template<typename Search>
	std::pair<iterator, bool> emplace_search(Search const search) {
		// key_type must be default initialized in case it's something like int
		key_type key{};
		return emplace_key(search, key, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple());
	}
	template<typename Search, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, key_type const & key, ConvertToMapped && mapped) {
		return emplace_key(search, key, key, std::forward<ConvertToMapped>(mapped));
	}
	template<typename Search, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, key_type && key, ConvertToMapped && mapped) {
		return emplace_key(search, key, std::move(key), std::forward<ConvertToMapped>(mapped));
	}
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, ConvertToKey && convert_to_key, ConvertToMapped && mapped) {
		key_type key(convert_to_key);
		return emplace_key(search, key, std::move(key), std::forward<ConvertToMapped>(mapped));
	}
	// These pair constructors could possibly be better written to not construct
	// key twice if ConvertToKey is not the same type as key_type.
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, std::pair<ConvertToKey, ConvertToMapped> && value) {
		return emplace_key(search, value.first, std::move(value));
	}
	template<typename Search, typename ConvertToKey, typename ConvertToMapped>
	std::pair<iterator, bool> emplace_search(Search const search, std::pair<ConvertToKey, ConvertToMapped> const & value) {
		return emplace_key(search, value.first, value);
	}
	class make_key {
	public:
		template<typename... Args>
		constexpr key_type operator()(Args && ... args) const {
			return key_type(std::forward<Args>(args)...);
		}
	};
	template<typename Search, typename KeyTuple, typename MappedTuple>
	std::pair<iterator, bool> emplace_search(Search const search, std::piecewise_construct_t, KeyTuple && key_tuple, MappedTuple && mapped_tuple) {
		key_type key(apply(make_key{}, std::forward<KeyTuple>(key_tuple)));
		return emplace_key(search, key, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward<MappedTuple>(mapped_tuple));
	}

	// args contains everything needed to construct value_type, including the
	// key. It is provided here as the first argument just to make things easier
	
	template<typename Search, typename... Args>
	auto checked_emplace_key(std::true_type, Search const search, key_type const & key, Args && ... args) {
		return container().emplace(search(key), std::forward<Args>(args)...);
	}

	template<typename Search, typename... Args>
	auto checked_emplace_key(std::false_type, Search const upper_bound, key_type const & key, Args && ... args) {
		auto const position = upper_bound(key);
		// Do not decrement an iterator if it might be begin()
		bool const there_is_element_before = position != begin();
		auto const previous = ::containers::prev(position);
		auto const that_element_is_equal = [&](){ return !key_comp()(previous->first, key); };
		bool const already_exists = there_is_element_before and that_element_is_equal();
		return already_exists ?
			std::make_pair(previous, false) :
			std::make_pair(container().emplace(position, std::forward<Args>(args)...), true);
	}

	template<typename Search, typename... Args>
	auto emplace_key(Search const search, key_type const & key, Args && ... args) {
		return checked_emplace_key(std::integral_constant<bool, allow_duplicates>{}, search, key, std::forward<Args>(args)...);
	}
	
	class indirect_compare {
	public:
		constexpr indirect_compare(value_compare const & compare):
			m_compare(compare) {
		}
		template<typename IndirectionType>
		constexpr bool operator()(IndirectionType const & lhs, IndirectionType const & rhs) const {
			return m_compare(*lhs, *rhs);
		}
		constexpr bool operator()(value_type const & lhs, value_type const & rhs) const {
			return m_compare(lhs, rhs);
		}	
	private:
		value_compare const & m_compare;
	};

	container_type const & container() const {
		return std::get<0>(m_container);
	}
	container_type & container() {
		return std::get<0>(m_container);
	}
	
	std::tuple<container_type, key_compare> m_container;
};

template<typename Key, typename T, typename Compare, template<typename, typename> class Container, typename Allocator>
class flat_map : public flat_map_base<Key, T, Compare, Container, Allocator, false> {
private:
	using base = flat_map_base<Key, T, Compare, Container, Allocator, false>;
public:
	using typename base::mapped_type;
	using typename base::key_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using base::base;

	mapped_type const & at(key_type const & key) const {
		auto const it = find(key);
		if (it == this->end()) {
			throw std::out_of_range{"Key not found"};
		}
		return it->second;
	}
	mapped_type & at(key_type const & key) {
		auto const it = this->find(key);
		if (it == this->end()) {
			throw std::out_of_range{"Key not found"};
		}
		return it->second;
	}
	mapped_type & operator[](key_type const & key) {
		return this->emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple()).first->second;
	}
	mapped_type & operator[](key_type && key) {
		return this->emplace(std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple()).first->second;
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const & key) const {
		auto const it = find(key);
		bool const found = it != this->end();
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}
	std::pair<iterator, iterator> equal_range(key_type const & key) {
		auto const it = find(key);
		bool const found = it != this->end();
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}

	size_type count(key_type const & key) const {
		bool const found = this->find(key) != this->end();
		return found ? 1 : 0;
	}

	size_type erase(key_type const & key) {
		auto const it = this->find(key);
		if (it == this->end()) {
			return 0;
		}
		this->erase(it);
		return 1;
	}
};

template<typename Key, typename T, typename Compare, template<typename, typename> class Container, typename Allocator>
class flat_multimap : public flat_map_base<Key, T, Compare, Container, Allocator, true> {
private:
	using base = flat_map_base<Key, T, Compare, Container, Allocator, true>;
	using typename base::key_value_compare;
public:
	using typename base::mapped_type;
	using typename base::key_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using base::base;

	// These implementations work for map or multimap, but I don't expect the
	// compiler to be able to optimize based on the fact that values in flat_map
	// are unique, so I have slightly different versions in flat_map.

	std::pair<const_iterator, const_iterator> equal_range(key_type const & key) const {
		return std::equal_range(this->begin(), this->end(), key, key_value_compare{this->key_comp()});
	}
	std::pair<iterator, iterator> equal_range(key_type const & key) {
		return std::equal_range(this->begin(), this->end(), key, key_value_compare{this->key_comp()});
	}

	size_type count(key_type const & key) const {
		auto const range = this->equal_range(key);
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	size_type erase(key_type const & key) {
		auto const range = this->equal_range(key);
		if (range.first == this->end()) {
			return 0;
		}
		auto const distance = std::distance(range.first, range.second);
		this->erase(range.first, range.second);
		return distance;
	}
};

}	// namespace detail

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<detail::value_type_t<Key, T, vector>>>
using unstable_flat_map = detail::flat_map<Key, T, Compare, vector, Allocator>;

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<detail::value_type_t<Key, T, moving_vector>>>
using stable_flat_map = detail::flat_map<Key, T, Compare, moving_vector, Allocator>;


template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<detail::value_type_t<Key, T, vector>>>
using unstable_flat_multimap = detail::flat_multimap<Key, T, Compare, vector, Allocator>;

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = std::allocator<detail::value_type_t<Key, T, moving_vector>>>
using stable_flat_multimap = detail::flat_multimap<Key, T, Compare, moving_vector, Allocator>;


}	// namespace containers
