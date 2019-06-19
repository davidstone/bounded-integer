// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/sort.hpp>
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
struct map_value_type : private bounded::tuple<Key, Mapped> {
	using key_type = Key;
	using mapped_type = Mapped;
	
	using bounded::tuple<Key, Mapped>::tuple;
	using bounded::tuple<Key, Mapped>::as_tuple;
	
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
constexpr auto compare(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	compare(lhs.as_tuple(), rhs.as_tuple())
)

template<typename Key, typename Mapped>
constexpr auto operator==(map_value_type<Key, Mapped> const & lhs, map_value_type<Key, Mapped> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	lhs.as_tuple() == rhs.as_tuple()
)

namespace detail {

template<typename Iterator>
struct inserted_t {
	Iterator iterator;
	bool inserted;
};

template<typename Iterator>
inserted_t(Iterator, bool) -> inserted_t<Iterator>;

// The exact type of value_type should be considered implementation defined.
// map_value_type<key_type const, mapped_type> does not work if the underlying
// container is vector because insertion into the middle / sorting requires
// moving the value_type. key_type const does not have a copy or move assignment
// operator. To get the code to work with a vector, we have to sacrifice some
// compile-time checks.

template<typename Container, typename Compare, bool allow_duplicates>
struct flat_map_base {
private:
	Container m_container;
	[[no_unique_address]] Compare m_compare;
public:
	using value_type = typename Container::value_type;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;
	using size_type = typename Container::size_type;

	using const_iterator = typename Container::const_iterator;
	using iterator = typename Container::iterator;
	
	using key_compare_type = Compare;
	class value_compare_type {
	protected:
		friend struct flat_map_base;
		constexpr value_compare_type(key_compare_type c) noexcept(std::is_nothrow_move_constructible_v<key_compare_type>):
			m_compare(std::move(c)) {
		}
		key_compare_type m_compare;
	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;
		constexpr auto operator()(value_type const & lhs, value_type const & rhs) const BOUNDED_NOEXCEPT(
			m_compare(lhs.key(), rhs.key())
		)
	};
	
	constexpr auto key_compare() const noexcept {
		return m_compare;
	}
	constexpr auto value_compare() const noexcept{
		return value_compare_type(key_compare());
	}
	
	flat_map_base() = default;
	constexpr explicit flat_map_base(key_compare_type compare) noexcept(std::is_nothrow_default_constructible_v<Container> and std::is_nothrow_move_constructible_v<Compare>):
		m_compare(std::move(compare))
	{
	}
	template<typename InputIterator, typename Sentinel>
	constexpr flat_map_base(InputIterator first, Sentinel last, key_compare_type compare = key_compare_type{}):
		m_container(first, last),
		m_compare(std::move(compare))
	{
		sort(m_container, value_compare());
		// At some point this should be unique_sort
		auto const equal = ::containers::negate(value_compare());
		::containers::erase(
			m_container,
			::containers::unique(begin(m_container), end(m_container), equal),
			end(m_container)
		);
	}

	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit flat_map_base(Range && range) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)))
	) {
	}

	constexpr flat_map_base(std::initializer_list<value_type> init, key_compare_type compare = key_compare_type{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		flat_map_base(begin(init), end(init), std::move(compare))
	) {
	}
	
	constexpr auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(flat_map_base(init)) and std::is_nothrow_move_assignable_v<flat_map_base>) {
		return *this = flat_map_base(init);
	}
	
	friend constexpr auto begin(flat_map_base const & c) BOUNDED_NOEXCEPT(
		begin(c.m_container)
	)
	friend constexpr auto begin(flat_map_base & c) BOUNDED_NOEXCEPT(
		begin(c.m_container)
	)
	friend constexpr auto end(flat_map_base const & c) BOUNDED_NOEXCEPT(
		end(c.m_container)
	)
	friend constexpr auto end(flat_map_base & c) BOUNDED_NOEXCEPT(
		end(c.m_container)
	)
	
	// Extra functions on top of the regular map interface
	constexpr auto capacity() const BOUNDED_NOEXCEPT(
		m_container.capacity()
	)
	constexpr auto reserve(size_type const new_capacity) BOUNDED_NOEXCEPT(
		m_container.reserve(new_capacity)
	)
	constexpr auto shrink_to_fit() BOUNDED_NOEXCEPT(
		m_container.shrink_to_fit()
	)
	
	template<typename Key>
	constexpr auto lower_bound(Key && key) const BOUNDED_NOEXCEPT_GCC_BUG(
		std::lower_bound(
			legacy_iterator(begin(*this)),
			legacy_iterator(end(*this)),
			BOUNDED_FORWARD(key),
			key_value_compare{key_compare()}
		).base()
	)
	template<typename Key>
	constexpr auto lower_bound(Key && key) BOUNDED_NOEXCEPT_GCC_BUG(
		std::lower_bound(
			legacy_iterator(begin(*this)),
			legacy_iterator(end(*this)),
			BOUNDED_FORWARD(key),
			key_value_compare{key_compare()}
		).base()
	)
	template<typename Key>
	constexpr auto upper_bound(Key && key) const BOUNDED_NOEXCEPT_GCC_BUG(
		std::upper_bound(
			legacy_iterator(begin(*this)),
			legacy_iterator(end(*this)),
			BOUNDED_FORWARD(key),
			key_value_compare{key_compare()}
		).base()
	)
	template<typename Key>
	constexpr auto upper_bound(Key && key) BOUNDED_NOEXCEPT_GCC_BUG(
		std::upper_bound(
			legacy_iterator(begin(*this)),
			legacy_iterator(end(*this)),
			BOUNDED_FORWARD(key),
			key_value_compare{key_compare()}
		).base()
	)
	// TODO: noexcept
	template<typename Key>
	constexpr auto find(Key const & key) const {
		auto const it = lower_bound(key);
		return (it == end(*this) or key_compare()(key, it->key())) ? end(*this) : it;
	}
	template<typename Key>
	constexpr auto find(Key const & key) {
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
		auto data = separate_key_from_mapped(BOUNDED_FORWARD(args)...);
		auto const position = upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
	}
	template<typename... Args>
	constexpr auto emplace_hint(const_iterator hint, Args && ... args) {
		auto data = separate_key_from_mapped(BOUNDED_FORWARD(args)...);
		auto const correct_greater = (hint == end(*this)) or key_compare(data.key, *hint);
		auto const correct_less = (hint == begin(*this)) or key_compare(*::containers::prev(hint), data.key);
		auto const correct_hint = correct_greater and correct_less;
		auto const position = correct_hint ? hint : upper_bound(data.key);
		return emplace_at(position, std::move(data).key, std::move(data).mapped_args);
	}

	// TODO: noexcept
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
	// TODO: noexcept
	template<typename Range = std::initializer_list<value_type>>
	constexpr auto insert(Range && range) {
		// Because my underlying container is expected to be contiguous storage,
		// it's best to do a batch insert and then just sort it all. However,
		// because I know that the first section of the final range is already
		// sorted, it's probably better to just sort the new elements then do a
		// merge sort on both ranges, rather than calling std::sort on the
		// entire container.
		auto const midpoint = append(m_container, BOUNDED_FORWARD(range));
		sort(midpoint, end(m_container), value_compare());
		if (allow_duplicates) {
			std::inplace_merge(
				legacy_iterator(begin(m_container)),
				legacy_iterator(midpoint),
				legacy_iterator(end(m_container)),
				value_compare()
			);
		} else {
			auto const position = ::containers::unique_inplace_merge(
				begin(m_container),
				midpoint,
				end(m_container),
				value_compare()
			);
			using containers::erase;
			erase(m_container, position, end(m_container));
		}
	}
	
	// These maintain the relative order of all elements in the container, so I
	// don't have to worry about re-sorting
	constexpr auto erase(const_iterator const it) BOUNDED_NOEXCEPT(
		m_container.erase(it)
	)
	// Need mutable iterator overload to avoid ambiguity if key_type can be
	// constructed from iterator (for instance, an unconstrained constructor
	// template).
	constexpr auto erase(iterator const it) BOUNDED_NOEXCEPT(
		m_container.erase(it)
	)
	constexpr auto erase(const_iterator const first, const_iterator const last) BOUNDED_NOEXCEPT(
		m_container.erase(first, last)
	)
	
private:
	class key_value_compare {
	public:
		constexpr key_value_compare(key_compare_type const & compare):
			m_compare(compare) {
		}
		template<typename Key>
		constexpr bool operator()(Key && key, value_type const & value) const {
			return m_compare(BOUNDED_FORWARD(key), value.key());
		}
		template<typename Key>
		constexpr bool operator()(value_type const & value, Key && key) const {
			return m_compare(value.key(), BOUNDED_FORWARD(key));
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
	static constexpr auto use_reference_to_key(bounded::detail::types<Key>) noexcept {
		return std::is_same<std::decay_t<Key>, key_type>::value;
	}
	template<typename... KeyArgs>
	static constexpr auto use_reference_to_key(bounded::detail::types<KeyArgs...>) noexcept {
		return false;
	}

	template<typename MappedTuple, typename... KeyArgs>
	static constexpr auto construct_result(MappedTuple && mapped_tuple, KeyArgs && ... key_args) {
		struct result {
			result(MappedTuple && mapped_, KeyArgs && ... key_):
				key(BOUNDED_FORWARD(key_)...),
				mapped_args(BOUNDED_FORWARD(mapped_)) {
			}

			std::conditional_t<
				use_reference_to_key(bounded::detail::types<KeyArgs...>{}),
				key_type const &,
				key_type
			> key;
			MappedTuple mapped_args;
		};
		return result(BOUNDED_FORWARD(mapped_tuple), BOUNDED_FORWARD(key_args)...);
	}

	static constexpr auto separate_key_from_mapped() {
		return construct_result(bounded::tuple<>{});
	}
	template<typename Pair>
	static constexpr auto separate_key_from_mapped(Pair && pair) {
		return construct_result(
			bounded::tie(BOUNDED_FORWARD(pair).mapped()),
			BOUNDED_FORWARD(pair).key()
		);
	}
	template<typename Key, typename Mapped>
	static constexpr auto separate_key_from_mapped(Key && key, Mapped && mapped) {
		return construct_result(
			bounded::tie(BOUNDED_FORWARD(mapped)),
			BOUNDED_FORWARD(key)
		);
	}
	template<typename KeyTuple, typename MappedTuple>
	static constexpr auto separate_key_from_mapped(std::piecewise_construct_t, KeyTuple && key, MappedTuple && mapped) {
		return bounded::apply(BOUNDED_FORWARD(key), [&](auto && ... args) {
			return construct_result(BOUNDED_FORWARD(mapped), BOUNDED_FORWARD(args)...);
		});
	}
	
	template<typename Key, typename Mapped>
	constexpr auto emplace_at(const_iterator position, Key && key, Mapped && mapped) {
		if constexpr (allow_duplicates) {
			return m_container.emplace(
				position,
				std::piecewise_construct,
				bounded::tie(BOUNDED_FORWARD(key)),
				BOUNDED_FORWARD(mapped)
			);
		} else {
			// Do not decrement an iterator if it might be begin(*this)
			bool const there_is_element_before = position != begin(*this);
			auto const that_element_is_equal = [&](){ return !key_compare()(containers::prev(position)->key(), key); };
			bool const already_exists = there_is_element_before and that_element_is_equal();
			if (already_exists) {
				return inserted_t{mutable_iterator(*this, containers::prev(position)), false};
			}

			auto const it = m_container.emplace(
				position,
				std::piecewise_construct,
				bounded::tie(BOUNDED_FORWARD(key)),
				BOUNDED_FORWARD(mapped)
			);
			return inserted_t{it, true};
		}
	}
};

}	// namespace detail


template<typename Container, typename Compare = std::less<>>
class basic_flat_map : private detail::flat_map_base<Container, Compare, false> {
private:
	using base = detail::flat_map_base<Container, Compare, false>;
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
	
	friend constexpr auto begin(basic_flat_map const & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base const &>(container))
	)
	friend constexpr auto begin(basic_flat_map & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base &>(container))
	)
	friend constexpr auto end(basic_flat_map const & container) BOUNDED_NOEXCEPT(
		end(static_cast<base const &>(container))
	)
	friend constexpr auto end(basic_flat_map & container) BOUNDED_NOEXCEPT(
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

template<typename Container, typename Compare>
constexpr auto is_container<basic_flat_map<Container, Compare>> = is_container<Container>;

template<typename Container, typename Compare = std::less<>>
class basic_flat_multimap : private detail::flat_map_base<Container, Compare, true> {
private:
	using base = detail::flat_map_base<Container, Compare, true>;
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
	
	friend constexpr auto begin(basic_flat_multimap const & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base const &>(container))
	)
	friend constexpr auto begin(basic_flat_multimap & container) BOUNDED_NOEXCEPT(
		begin(static_cast<base &>(container))
	)
	friend constexpr auto end(basic_flat_multimap const & container) BOUNDED_NOEXCEPT(
		end(static_cast<base const &>(container))
	)
	friend constexpr auto end(basic_flat_multimap & container) BOUNDED_NOEXCEPT(
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

	template<typename Key>
	constexpr auto equal_range(Key && key) const BOUNDED_NOEXCEPT_GCC_BUG(
		std::equal_range(begin(*this), end(*this), BOUNDED_FORWARD(key), key_value_compare{this->key_compare()})
	)
	template<typename Key>
	constexpr auto equal_range(Key && key) BOUNDED_NOEXCEPT_GCC_BUG(
		std::equal_range(begin(*this), end(*this), BOUNDED_FORWARD(key), key_value_compare{this->key_compare()})
	)

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

template<typename Container, typename Compare>
constexpr auto is_container<basic_flat_multimap<Container, Compare>> = is_container<Container>;


template<typename Key, typename T, typename... MaybeCompare>
using flat_map = basic_flat_map<vector<map_value_type<Key, T>>, MaybeCompare...>;

template<typename Key, typename T, typename... MaybeCompare>
using flat_multimap = basic_flat_multimap<vector<map_value_type<Key, T>>, MaybeCompare...>;


}	// namespace containers
