// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/tuple.hpp>
#include <containers/vector/vector.hpp>

#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace containers {
namespace detail {

template<typename Key, typename Mapped>
struct map_value_type : private tuple<Key, Mapped> {
	using key_type = Key;
	using mapped_type = Mapped;
	
	using tuple<Key, Mapped>::tuple;
	using tuple<Key, Mapped>::as_tuple;
	
	constexpr auto && key() const & noexcept {
		return (*this)[0_bi];
	}
	constexpr auto && key() & noexcept {
		return (*this)[0_bi];
	}
	constexpr auto && key() && noexcept {
		return std::move(*this)[0_bi];
	}
	constexpr auto && mapped() const & noexcept {
		return (*this)[1_bi];
	}
	constexpr auto && mapped() & noexcept {
		return (*this)[1_bi];
	}
	constexpr auto && mapped() && noexcept {
		return std::move(*this)[1_bi];
	}
};

template<typename Key, typename Mapped>
constexpr auto operator==(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	lhs.as_tuple() == rhs.as_tuple()
)
template<typename Key, typename Mapped>
constexpr auto operator<(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	lhs.as_tuple() < rhs.as_tuple()
)


template<typename Container, typename KeyCompare>
struct flat_map_data : private tuple<Container, KeyCompare> {
	using tuple<Container, KeyCompare>::tuple;

	constexpr auto && container() const & noexcept {
		return (*this)[0_bi];
	}
	constexpr auto && container() & noexcept {
		return (*this)[0_bi];
	}
	constexpr auto && container() && noexcept {
		return std::move(*this)[0_bi];
	}
	constexpr auto key_compare() const BOUNDED_NOEXCEPT_VALUE(
		(*this)[1_bi]
	)
};


// The exact type of value_type should be considered implementation defined.
// map_value_type<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks.

template<typename Container, typename Compare, bool allow_duplicates>
class flat_map_base : private flat_map_data<Container, Compare> {
	using base = flat_map_data<Container, Compare>;
	using base::container;
public:
	using value_type = typename Container::value_type;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;
	using allocator_type = typename Container::allocator_type;
	using size_type = typename Container::size_type;

	using const_iterator = typename Container::const_iterator;
	using iterator = typename Container::iterator;
	
	using key_compare_type = Compare;
	class value_compare_type {
	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;
		constexpr auto operator()(value_type const & lhs, value_type const & rhs) const BOUNDED_NOEXCEPT(
			m_compare(lhs.key(), rhs.key())
		)
	protected:
		friend class flat_map_base;
		constexpr value_compare_type(key_compare_type c) noexcept(std::is_nothrow_move_constructible<key_compare_type>::value):
			m_compare(std::move(c)) {
		}
		key_compare_type m_compare;
	};
	
	using base::key_compare;
	constexpr auto value_compare() const BOUNDED_NOEXCEPT(
		value_compare_type(key_compare())
	)
	
	flat_map_base() = default;
	constexpr explicit flat_map_base(key_compare_type compare, allocator_type allocator = allocator_type{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(Container(std::move(allocator)), std::move(compare))
	) {
	}
	constexpr explicit flat_map_base(allocator_type allocator) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(key_compare_type{}, std::move(allocator))
	) {
	}
	template<typename InputIterator>
	constexpr flat_map_base(InputIterator first, InputIterator last, key_compare_type compare = key_compare_type{}, allocator_type allocator = allocator_type{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(Container(first, last, std::move(allocator)), std::move(compare))
	) {
		std::sort(begin(container()), end(container()), value_compare());
		// At some point this should be unique_sort
		auto const equal = ::containers::negate(value_compare());
		::containers::erase(
			container(),
			::containers::unique(begin(container()), end(container()), equal),
			end(container())
		);
	}
	template<typename InputIterator>
	constexpr flat_map_base(InputIterator first, InputIterator last, allocator_type allocator) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(first, last, key_compare_type{}, std::move(allocator))
	) {
	}
	constexpr flat_map_base(flat_map_base const & other, allocator_type allocator) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(Container(other, std::move(allocator)), key_compare_type{})
	) {
	}
	constexpr flat_map_base(flat_map_base && other, allocator_type allocator) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(Container(std::move(other), std::move(allocator)), key_compare_type{})
	) {
	}
	constexpr flat_map_base(std::initializer_list<value_type> init, key_compare_type compare = key_compare_type{}, allocator_type allocator = allocator_type{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(begin(init), end(init), std::move(compare), std::move(allocator))
	) {
	}
	constexpr flat_map_base(std::initializer_list<value_type> init, allocator_type allocator) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(init, key_compare_type{}, std::move(allocator))
	) {
	}
	
	constexpr auto & operator=(std::initializer_list<value_type> init) & BOUNDED_NOEXCEPT(
		*this = flat_map_base(init)
	)
	
	friend constexpr auto begin(flat_map_base const & c) BOUNDED_NOEXCEPT(
		begin(c.container())
	)
	friend constexpr auto begin(flat_map_base & c) BOUNDED_NOEXCEPT(
		begin(c.container())
	)
	friend constexpr auto end(flat_map_base const & c) BOUNDED_NOEXCEPT(
		end(c.container())
	)
	friend constexpr auto end(flat_map_base & c) BOUNDED_NOEXCEPT(
		end(c.container())
	)
	
	// Extra functions on top of the regular map interface
	constexpr auto capacity() const BOUNDED_NOEXCEPT(
		container().capacity()
	)
	constexpr auto reserve(size_type const new_capacity) BOUNDED_NOEXCEPT(
		container().reserve(new_capacity)
	)
	constexpr auto shrink_to_fit() BOUNDED_NOEXCEPT(
		container().shrink_to_fit()
	)
	
	constexpr auto lower_bound(key_type const & key) const BOUNDED_NOEXCEPT(
		std::lower_bound(begin(*this), end(*this), key, key_value_compare{key_compare()})
	)
	constexpr auto lower_bound(key_type const & key) BOUNDED_NOEXCEPT(
		std::lower_bound(begin(*this), end(*this), key, key_value_compare{key_compare()})
	)
	constexpr auto upper_bound(key_type const & key) const BOUNDED_NOEXCEPT(
		std::upper_bound(begin(*this), end(*this), key, key_value_compare{key_compare()})
	)
	constexpr auto upper_bound(key_type const & key) {
		return std::upper_bound(begin(*this), end(*this), key, key_value_compare{key_compare()});
	}
	// TODO: noexcept
	constexpr auto find(key_type const & key) const {
		auto const it = lower_bound(key);
		return (it == end(*this) or key_compare()(key, it->key)) ? end(*this) : it;
	}
	constexpr auto find(key_type const & key) {
		auto const it = lower_bound(key);
		return (it == end(*this) or key_compare()(key, it->key())) ? end(*this) : it;
	}
	
	// Unlike in std::map, insert / emplace can only provide a time complexity
	// that matches an insert into the underlying container, which is to say,
	// linear. An insertion implies shifting all of the elements.
	//
	// Moreover, emplace cannot in general provide the guarantee of no copying
	// or moving. It can only provide the weaker guarantee of no copying or
	// moving of the mapped_type. We must copy or move key_type because we have
	// to construct the key to determine whether we should insert it.
	//
	// TODO: noexcept
	template<typename... Args>
	constexpr auto emplace(Args && ... args) {
		auto data = separate_key_from_mapped(std::forward<Args>(args)...);
		auto const position = upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
	}
	template<typename... Args>
	constexpr auto emplace_hint(const_iterator hint, Args && ... args) {
		auto data = separate_key_from_mapped(std::forward<Args>(args)...);
		auto const correct_greater = (hint == end(*this)) or key_compare(data.key, *hint);
		auto const correct_less = (hint == begin(*this)) or key_compare(*::containers::prev(hint), data.key);
		auto const correct_hint = correct_greater and correct_less;
		auto const position = correct_hint ? hint : upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
	}

	constexpr auto insert(value_type const & value) BOUNDED_NOEXCEPT(
		emplace(value)
	)
	constexpr auto insert(value_type && value) BOUNDED_NOEXCEPT(
		emplace(std::move(value))
	)
	constexpr auto insert(const_iterator const hint, value_type const & value) BOUNDED_NOEXCEPT(
		emplace_hint(hint, value)
	)
	constexpr auto insert(const_iterator const hint, value_type && value) BOUNDED_NOEXCEPT(
		emplace_hint(hint, std::move(value))
	)
	// TODO: noexcept
	template<typename InputIterator>
	constexpr auto insert(InputIterator first, InputIterator const last) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const midpoint = container().insert(end(container()), first, last);
		std::sort(midpoint, end(container()), value_compare());
		if (allow_duplicates) {
			std::inplace_merge(
				legacy_iterator(begin(container())),
				legacy_iterator(midpoint),
				legacy_iterator(end(container())),
				value_compare()
			);
		}
		else {
			auto const position = ::containers::unique_inplace_merge(
				begin(container()),
				midpoint,
				end(container()),
				value_compare()
			);
			using containers::erase;
			erase(container(), position, end(container()));
		}
	}
	constexpr auto insert(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_DECLTYPE(
		insert(begin(init), end(init))
	)
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	constexpr auto erase(const_iterator const it) BOUNDED_NOEXCEPT(
		container().erase(it)
	)
	// Need mutable iterator overload to avoid ambiguity if key_type can be
	// constructed from iterator (for instance, an unconstrained constructor
	// template).
	constexpr auto erase(iterator const it) BOUNDED_NOEXCEPT(
		container().erase(it)
	)
	constexpr auto erase(const_iterator const first, const_iterator const last) BOUNDED_NOEXCEPT(
		container().erase(first, last)
	)
	
private:
	class key_value_compare {
	public:
		constexpr key_value_compare(key_compare_type const & compare):
			m_compare(compare) {
		}
		constexpr bool operator()(key_type const & key, value_type const & value) const {
			return m_compare(key, value.key());
		}
		constexpr bool operator()(value_type const & value, key_type const & key) const {
			return m_compare(value.key(), key);
		}
	private:
		key_compare_type const & m_compare;
	};

	// It is safe to bind the reference to the object that is being moved in any
	// of these calls to emplace_key because the call to std::move does not
	// actually move anything, it just converts it to an rvalue reference. The
	// object isn't actually moved from until later on in the function.
	//
	// Search represents a function that finds where to insert

	template<typename Key>
	static constexpr auto use_reference_to_key(type_list<Key>) noexcept {
		return std::is_same<std::decay_t<Key>, key_type>::value;
	}
	template<typename... KeyArgs>
	static constexpr auto use_reference_to_key(type_list<KeyArgs...>) noexcept {
		return false;
	}

	template<typename MappedTuple, typename... KeyArgs>
	static constexpr auto construct_result(MappedTuple && mapped_tuple, KeyArgs && ... key_args) {
		struct result {
			result(MappedTuple && mapped_, KeyArgs && ... key_):
				key(std::forward<KeyArgs>(key_)...),
				mapped_args(std::forward<MappedTuple>(mapped_)) {
			}

			std::conditional_t<
				use_reference_to_key(type_list<KeyArgs...>{}),
				key_type const &,
				key_type
			> key;
			MappedTuple mapped_args;
		};
		return result(std::forward<MappedTuple>(mapped_tuple), std::forward<KeyArgs>(key_args)...);
	}

	static constexpr auto separate_key_from_mapped() {
		return construct_result(tuple<>{});
	}
	template<typename Pair>
	static constexpr auto separate_key_from_mapped(Pair && pair) {
		return construct_result(
			::containers::forward_as_tuple(std::forward<Pair>(pair).mapped()),
			std::forward<Pair>(pair).key()
		);
	}
	template<typename Key, typename Mapped>
	static constexpr auto separate_key_from_mapped(Key && key, Mapped && mapped) {
		return construct_result(
			::containers::forward_as_tuple(std::forward<Mapped>(mapped)),
			std::forward<Key>(key)
		);
	}
	template<typename KeyTuple, typename MappedTuple>
	static constexpr auto separate_key_from_mapped(std::piecewise_construct_t, KeyTuple && key, MappedTuple && mapped) {
		auto construct = [&](auto && ... args) {
			return construct_result(
				std::forward<MappedTuple>(mapped),
				std::forward<decltype(args)>(args)...
			);
		};
		return apply(construct, std::forward<KeyTuple>(key));
	}
	
	struct inserted_t : private tuple<iterator, bool> {
		using tuple<iterator, bool>::tuple;
	
		constexpr auto iterator() const noexcept {
			return (*this)[0_bi];
		}
		constexpr auto inserted() const noexcept {
			return (*this)[1_bi];
		}
	};

	template<typename>
	static constexpr auto dependent_allow_duplicates = allow_duplicates;
	
	template<typename Key, typename Mapped, BOUNDED_REQUIRES(dependent_allow_duplicates<Key>)>
	constexpr auto emplace_at(const_iterator position, Key && key, Mapped && mapped) BOUNDED_NOEXCEPT_DECLTYPE(
		container().emplace(
			position,
			std::piecewise_construct,
			::containers::forward_as_tuple(std::forward<Key>(key)),
			std::forward<Mapped>(mapped)
		)
	)

	template<typename Key, typename Mapped, BOUNDED_REQUIRES(!dependent_allow_duplicates<Key>)>
	constexpr auto emplace_at(const_iterator position, Key && key, Mapped && mapped) {
		// Do not decrement an iterator if it might be begin(*this)
		bool const there_is_element_before = position != begin(*this);
		auto const that_element_is_equal = [&](){ return !key_compare()(::containers::prev(position)->key(), key); };
		bool const already_exists = there_is_element_before and that_element_is_equal();
		if (already_exists) {
			return inserted_t(mutable_iterator(*this, ::containers::prev(position)), false);
		}

		auto const it = container().emplace(
			position,
			std::piecewise_construct,
			::containers::forward_as_tuple(std::forward<Key>(key)),
			std::forward<Mapped>(mapped)
		);
		return inserted_t(it, true);
	}
};



template<typename Container, typename Compare>
class flat_map : private flat_map_base<Container, Compare, false> {
private:
	using base = flat_map_base<Container, Compare, false>;
public:
	using typename base::key_type;
	using typename base::mapped_type;
	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using typename base::key_compare_type;
	using typename base::value_compare_type;
	
	using base::key_compare;
	using base::value_compare;

	using base::base;
	using base::operator=;
	
	friend constexpr auto begin(flat_map const & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base const &>(container))
	)
	friend constexpr auto begin(flat_map & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base &>(container))
	)
	friend constexpr auto end(flat_map const & container) BOUNDED_NOEXCEPT(
		end(static_cast<base const &>(container))
	)
	friend constexpr auto end(flat_map & container) BOUNDED_NOEXCEPT(
		end(static_cast<base &>(container))
	)

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

	constexpr auto const & at(key_type const & key) const {
		auto const it = find(key);
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped;
	}
	constexpr auto & at(key_type const & key) {
		auto const it = this->find(key);
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped();
	}
	constexpr auto & operator[](key_type const & key) {
		return this->emplace(std::piecewise_construct, ::containers::forward_as_tuple(key), ::containers::forward_as_tuple()).first->mapped;
	}
	constexpr auto & operator[](key_type && key) {
		return this->emplace(std::piecewise_construct, ::containers::forward_as_tuple(std::move(key)), ::containers::forward_as_tuple()).first->mapped;
	}

	constexpr auto equal_range(key_type const & key) const {
		auto const it = find(key);
		bool const found = it != end(*this);
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}
	constexpr auto equal_range(key_type const & key) {
		auto const it = find(key);
		bool const found = it != end(*this);
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}

	constexpr size_type count(key_type const & key) const {
		bool const found = this->find(key) != end(*this);
		return found ? 1 : 0;
	}

	constexpr size_type erase(key_type const & key) {
		auto const it = this->find(key);
		if (it == end(*this)) {
			return 0;
		}
		this->erase(it);
		return 1;
	}
};


template<typename Container, typename Compare>
class flat_multimap : private flat_map_base<Container, Compare, true> {
private:
	using base = flat_map_base<Container, Compare, true>;
	using typename base::key_value_compare;
public:
	using typename base::key_type;
	using typename base::mapped_type;
	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;

	using typename base::key_compare_type;
	using typename base::value_compare_type;
	
	using base::key_compare;
	using base::value_compare;

	using base::base;
	using base::operator=;
	
	friend constexpr auto begin(flat_multimap const & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base const &>(container))
	)
	friend constexpr auto begin(flat_multimap & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base &>(container))
	)
	friend constexpr auto end(flat_multimap const & container) BOUNDED_NOEXCEPT(
		end(static_cast<base const &>(container))
	)
	friend constexpr auto end(flat_multimap & container) BOUNDED_NOEXCEPT(
		end(static_cast<base &>(container))
	)

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

	constexpr auto equal_range(key_type const & key) const BOUNDED_NOEXCEPT(
		std::equal_range(begin(*this), end(*this), key, key_value_compare{this->key_compare()})
	)
	constexpr auto equal_range(key_type const & key) BOUNDED_NOEXCEPT(
		std::equal_range(begin(*this), end(*this), key, key_value_compare{this->key_compare()})
	)

	constexpr auto count(key_type const & key) const {
		auto const range = this->equal_range(key);
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	constexpr size_type erase(key_type const & key) {
		auto const range = this->equal_range(key);
		if (range.first == end(*this)) {
			return 0;
		}
		auto const distance = std::distance(range.first, range.second);
		this->erase(range.first, range.second);
		return distance;
	}
};

}	// namespace detail


template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = allocator<detail::map_value_type<Key, T>>>
using flat_map = detail::flat_map<vector<detail::map_value_type<Key, T>, Allocator>, Compare>;


template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = allocator<detail::map_value_type<Key, T>>>
using flat_multimap = detail::flat_multimap<vector<detail::map_value_type<Key, T>, Allocator>, Compare>;

}	// namespace containers
