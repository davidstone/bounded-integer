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




// The exact type of value_type should be considered implementation defined.
// map_value_type<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks.

template<typename Container, typename Compare, bool allow_duplicates>
class flat_map_base {
public:
	using value_type = typename Container::value_type;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;
	using allocator_type = typename Container::allocator_type;
	using size_type = typename Container::size_type;

	using const_iterator = typename Container::const_iterator;
	using iterator = typename Container::iterator;
	
	using key_compare = Compare;
	class value_compare {
	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;
		constexpr bool operator()(value_type const & lhs, value_type const & rhs) const {
			return m_compare(lhs.key(), rhs.key());
		}
	protected:
		friend class flat_map_base;
		value_compare(key_compare c):
			m_compare(std::move(c)) {
		}
		key_compare m_compare;
	};
	key_compare key_comp() const {
		return m_data.compare();
	}
	value_compare value_comp() const {
		return value_compare(key_comp());
	}
	
	flat_map_base() = default;
	explicit flat_map_base(key_compare compare, allocator_type allocator = allocator_type{}):
		m_data(Container(std::move(allocator)), std::move(compare)) {
	}
	explicit flat_map_base(allocator_type allocator):
		flat_map_base(key_compare{}, std::move(allocator)) {
	}
	template<typename InputIterator>
	flat_map_base(InputIterator first, InputIterator last, key_compare compare = key_compare{}, allocator_type allocator = allocator_type{}):
		m_data(Container(first, last, std::move(allocator)), std::move(compare))
	{
		auto const less = indirect_compare(value_comp());
		std::sort(begin(m_data.container()), end(m_data.container()), less);
		// At some point this should be unique_sort
		auto const equal = ::containers::negate(less);
		::containers::erase(
			m_data.container(),
			::containers::unique(begin(m_data.container()), end(m_data.container()), equal),
			end(m_data.container())
		);
	}
	template<typename InputIterator>
	flat_map_base(InputIterator first, InputIterator last, allocator_type allocator):
		flat_map_base(first, last, key_compare{}, std::move(allocator)) {
	}
	flat_map_base(flat_map_base const & other, allocator_type allocator):
		m_data(Container(other, std::move(allocator)), key_compare{}) {
	}
	flat_map_base(flat_map_base && other, allocator_type allocator):
		m_data(Container(std::move(other), std::move(allocator)), key_compare{}) {
	}
	flat_map_base(std::initializer_list<value_type> init, key_compare compare = key_compare{}, allocator_type allocator = allocator_type{}):
		flat_map_base(begin(init), end(init), std::move(compare), std::move(allocator)) {
	}
	flat_map_base(std::initializer_list<value_type> init, allocator_type allocator):
		flat_map_base(init, key_compare{}, std::move(allocator)) {
	}
	
	flat_map_base & operator=(std::initializer_list<value_type> init) {
		return *this = flat_map_base(init);
	}
	
	friend constexpr auto begin(flat_map_base const & container) noexcept {
		static_assert(noexcept(begin(container.m_data.container())), "This function assumes begin is noexcept");
		return begin(container.m_data.container());
	}
	friend constexpr auto begin(flat_map_base & container) noexcept {
		static_assert(noexcept(begin(container.m_data.container())), "This function assumes begin is noexcept");
		return begin(container.m_data.container());
	}
	friend constexpr auto end(flat_map_base const & container) noexcept {
		static_assert(noexcept(end(container.m_data.container())), "This function assumes end is noexcept");
		return end(container.m_data.container());
	}
	friend constexpr auto end(flat_map_base & container) noexcept {
		static_assert(noexcept(end(container.m_data.container())), "This function assumes end is noexcept");
		return end(container.m_data.container());
	}
	
	// Extra functions on top of the regular map interface
	size_type capacity() const noexcept {
		return m_data.container().capacity();
	}
	void reserve(size_type const new_capacity) noexcept {
		return m_data.container().reserve(new_capacity);
	}
	void shrink_to_fit() {
		m_data.container().shrink_to_fit();
	}
	
	const_iterator lower_bound(key_type const & key) const {
		return std::lower_bound(begin(*this), end(*this), key, key_value_compare{key_comp()});
	}
	iterator lower_bound(key_type const & key) {
		return std::lower_bound(begin(*this), end(*this), key, key_value_compare{key_comp()});
	}
	const_iterator upper_bound(key_type const & key) const {
		return std::upper_bound(begin(*this), end(*this), key, key_value_compare{key_comp()});
	}
	iterator upper_bound(key_type const & key) {
		return std::upper_bound(begin(*this), end(*this), key, key_value_compare{key_comp()});
	}
	const_iterator find(key_type const & key) const {
		auto const it = lower_bound(key);
		return (it == end(*this) or key_comp()(key, it->key)) ? end(*this) : it;
	}
	iterator find(key_type const & key) {
		auto const it = lower_bound(key);
		return (it == end(*this) or key_comp()(key, it->key())) ? end(*this) : it;
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
	auto emplace(Args && ... args) {
		auto data = separate_key_from_mapped(std::forward<Args>(args)...);
		auto const position = upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
	}
	template<typename... Args>
	iterator emplace_hint(const_iterator hint, Args && ... args) {
		auto data = separate_key_from_mapped(std::forward<Args>(args)...);
		auto const correct_greater = (hint == end(*this)) or key_comp(data.key, *hint);
		auto const correct_less = (hint == begin(*this)) or key_comp(*::containers::prev(hint), data.key);
		auto const correct_hint = correct_greater and correct_less;
		auto const position = correct_hint ? hint : upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
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
		auto const midpoint = m_data.container().insert(end(m_data.container()), first, last);
		std::sort(midpoint, end(m_data.container()), indirect_compare{value_comp()});
		if (allow_duplicates) {
			std::inplace_merge(
				legacy_iterator(begin(m_data.container())),
				legacy_iterator(midpoint),
				legacy_iterator(end(m_data.container())),
				indirect_compare(value_comp())
			);
		}
		else {
			auto const position = ::containers::unique_inplace_merge(
				begin(m_data.container()),
				midpoint,
				end(m_data.container()),
				indirect_compare(value_comp())
			);
			using containers::erase;
			erase(m_data.container(), position, end(m_data.container()));
		}
	}
	void insert(std::initializer_list<value_type> init) {
		insert(begin(init), end(init));
	}
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	iterator erase(const_iterator const it) {
		return m_data.container().erase(it);
	}
	// Need mutable iterator overload to avoid ambiguity if key_type can be
	// constructed from iterator (for instance, an unconstrained constructor
	// template).
	iterator erase(iterator const it) {
		return m_data.container().erase(it);
	}
	iterator erase(const_iterator const first, const_iterator const last) {
		return m_data.container().erase(first, last);
	}
	
protected:
	class key_value_compare {
	public:
		constexpr key_value_compare(key_compare const & compare):
			m_compare(compare) {
		}
		constexpr bool operator()(key_type const & key, value_type const & value) const {
			return m_compare(key, value.key());
		}
		constexpr bool operator()(value_type const & value, key_type const & key) const {
			return m_compare(value.key(), key);
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


	template<typename...>
	struct type_list{};

	template<typename Key>
	static constexpr auto use_reference_to_key(type_list<Key>) {
		return std::is_same<std::decay_t<Key>, key_type>::value;
	}
	template<typename... KeyArgs>
	static constexpr auto use_reference_to_key(type_list<KeyArgs...>) {
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
	auto emplace_at(const_iterator position, Key && key, Mapped && mapped) {
		return m_data.container().emplace(
			position,
			std::piecewise_construct,
			::containers::forward_as_tuple(std::forward<Key>(key)),
			std::forward<Mapped>(mapped)
		);
	}

	template<typename Key, typename Mapped, BOUNDED_REQUIRES(!dependent_allow_duplicates<Key>)>
	auto emplace_at(const_iterator position, Key && key, Mapped && mapped) {
		// Do not decrement an iterator if it might be begin(*this)
		bool const there_is_element_before = position != begin(*this);
		auto const that_element_is_equal = [&](){ return !key_comp()(::containers::prev(position)->key(), key); };
		bool const already_exists = there_is_element_before and that_element_is_equal();
		if (already_exists) {
			return inserted_t(mutable_iterator(*this, ::containers::prev(position)), false);
		}

		auto const it = m_data.container().emplace(
			position,
			std::piecewise_construct,
			::containers::forward_as_tuple(std::forward<Key>(key)),
			std::forward<Mapped>(mapped)
		);
		return inserted_t(it, true);
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

	struct Data : private tuple<Container, key_compare> {
		using tuple<Container, key_compare>::tuple;
	
		constexpr auto && container() const & noexcept {
			return (*this)[0_bi];
		}
		constexpr auto && container() & noexcept {
			return (*this)[0_bi];
		}
		constexpr auto && container() && noexcept {
			return std::move(*this)[0_bi];
		}
		constexpr auto && compare() const & noexcept {
			return (*this)[1_bi];
		}
		constexpr auto && compare() & noexcept {
			return (*this)[1_bi];
		}
		constexpr auto && compare() && noexcept {
			return std::move(*this)[1_bi];
		}
	};
	Data m_data;
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

	using typename base::key_compare;
	using typename base::value_compare;
	
	using base::key_comp;
	using base::value_comp;

	using base::base;
	using base::operator=;
	
	friend constexpr auto begin(flat_map const & container) noexcept {
		static_assert(noexcept(begin(static_cast<base const &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(flat_map & container) noexcept {
		static_assert(noexcept(begin(static_cast<base &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(flat_map const & container) noexcept {
		static_assert(noexcept(end(static_cast<base const &>(container))), "This function assumes end is noexcept");
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(flat_map & container) noexcept {
		static_assert(noexcept(end(static_cast<base &>(container))), "This function assumes end is noexcept");
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

	mapped_type const & at(key_type const & key) const {
		auto const it = find(key);
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped;
	}
	mapped_type & at(key_type const & key) {
		auto const it = this->find(key);
		if (it == end(*this)) {
			throw std::out_of_range{"Key not found"};
		}
		return it->mapped();
	}
	mapped_type & operator[](key_type const & key) {
		return this->emplace(std::piecewise_construct, ::containers::forward_as_tuple(key), ::containers::forward_as_tuple()).first->mapped;
	}
	mapped_type & operator[](key_type && key) {
		return this->emplace(std::piecewise_construct, ::containers::forward_as_tuple(std::move(key)), ::containers::forward_as_tuple()).first->mapped;
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const & key) const {
		auto const it = find(key);
		bool const found = it != end(*this);
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}
	std::pair<iterator, iterator> equal_range(key_type const & key) {
		auto const it = find(key);
		bool const found = it != end(*this);
		return std::make_pair(it, found ? ::containers::next(it) : it);
	}

	size_type count(key_type const & key) const {
		bool const found = this->find(key) != end(*this);
		return found ? 1 : 0;
	}

	size_type erase(key_type const & key) {
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

	using typename base::key_compare;
	using typename base::value_compare;
	
	using base::key_comp;
	using base::value_comp;

	using base::base;
	using base::operator=;
	
	friend constexpr auto begin(flat_multimap const & container) noexcept {
		static_assert(noexcept(begin(static_cast<base const &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(flat_multimap & container) noexcept {
		static_assert(noexcept(begin(static_cast<base &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(flat_multimap const & container) noexcept {
		static_assert(noexcept(end(static_cast<base const &>(container))), "This function assumes end is noexcept");
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(flat_multimap & container) noexcept {
		static_assert(noexcept(end(static_cast<base &>(container))), "This function assumes end is noexcept");
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

	std::pair<const_iterator, const_iterator> equal_range(key_type const & key) const {
		return std::equal_range(begin(*this), end(*this), key, key_value_compare{this->key_comp()});
	}
	std::pair<iterator, iterator> equal_range(key_type const & key) {
		return std::equal_range(begin(*this), end(*this), key, key_value_compare{this->key_comp()});
	}

	size_type count(key_type const & key) const {
		auto const range = this->equal_range(key);
		return static_cast<size_type>(std::distance(range.first, range.second));
	}
	
	size_type erase(key_type const & key) {
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
