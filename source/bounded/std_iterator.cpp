// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.std_iterator;

import bounded.bounded_integer;
import bounded.character;
import bounded.concepts;
import bounded.declval;
import bounded.integer;

import std_module;

namespace bounded {

template<typename T, typename value_type = std::remove_cvref_t<decltype(*declval<T>())>>
concept std_random_access_iterator =
	!std::is_pointer_v<T> and
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std::same_as<T, std::string::iterator> or
		std::same_as<T, std::string::const_iterator> or
		std::same_as<T, typename std::deque<value_type>::iterator> or
		std::same_as<T, typename std::deque<value_type>::const_iterator> or
		std::same_as<T, typename std::vector<value_type>::iterator> or
		std::same_as<T, typename std::vector<value_type>::const_iterator> or
		std::same_as<T, typename std::span<value_type>::iterator> or
		std::same_as<T, typename std::span<value_type const>::iterator> or
		std::same_as<T, typename std::span<value_type>::const_iterator> or
		(character<value_type> and (
			std::same_as<T, typename std::basic_string_view<value_type>::iterator> or
			std::same_as<T, typename std::basic_string_view<value_type>::const_iterator>
		))
	);

template<typename T>
concept hashable = default_constructible<std::hash<T>>;

template<typename T, typename Key, typename Mapped>
concept std_ordered_map_iterator =
	std::same_as<T, typename std::map<Key, Mapped>::iterator> or
	std::same_as<T, typename std::map<Key, Mapped>::const_iterator> or
	std::same_as<T, typename std::multimap<Key, Mapped>::iterator> or
	std::same_as<T, typename std::multimap<Key, Mapped>::const_iterator>;

template<typename T, typename Key, typename Mapped>
concept std_unordered_map_iterator =
	hashable<Key> and (
		std::same_as<T, typename std::unordered_map<Key, Mapped>::iterator> or
		std::same_as<T, typename std::unordered_map<Key, Mapped>::const_iterator> or
		std::same_as<T, typename std::unordered_multimap<Key, Mapped>::iterator> or
		std::same_as<T, typename std::unordered_multimap<Key, Mapped>::const_iterator>
	);

template<
	typename T,
	typename value_type,
	typename key_type = typename value_type::first_type,
	typename mapped_type = typename value_type::second_type
>
concept std_map_bidirectional_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and
	!std::is_reference_v<key_type> and !std::is_const_v<key_type> and
	!std::is_reference_v<mapped_type> and !std::is_const_v<mapped_type> and
	(
		std_ordered_map_iterator<T, key_type, mapped_type> or
		std_unordered_map_iterator<T, key_type, mapped_type>
	);

template<typename T, typename value_type>
concept std_unordered_set_iterator =
	hashable<value_type> and (
		std::same_as<T, typename std::unordered_set<value_type>::iterator> or
		std::same_as<T, typename std::unordered_set<value_type>::const_iterator> or
		std::same_as<T, typename std::unordered_multiset<value_type>::iterator> or
		std::same_as<T, typename std::unordered_multiset<value_type>::const_iterator>
	);

template<typename T, typename value_type = std::remove_cvref_t<decltype(*declval<T>())>>
concept std_bidirectional_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_random_access_iterator<T> or
		std_map_bidirectional_iterator<T, value_type> or
		std_unordered_set_iterator<T, value_type> or
		std::same_as<T, typename std::list<value_type>::iterator> or
		std::same_as<T, typename std::list<value_type>::const_iterator> or
		std::same_as<T, typename std::set<value_type>::iterator> or
		std::same_as<T, typename std::set<value_type>::const_iterator> or
		std::same_as<T, typename std::multiset<value_type>::iterator> or
		std::same_as<T, typename std::multiset<value_type>::const_iterator>
	);

template<typename T, typename value_type = std::remove_cvref_t<decltype(*declval<T>())>>
concept std_iterator =
	!std::is_void_v<value_type> and !std::is_reference_v<value_type> and !std::is_const_v<value_type> and (
		std_bidirectional_iterator<T> or
		std::same_as<T, typename std::forward_list<value_type>::iterator> or
		std::same_as<T, typename std::forward_list<value_type>::const_iterator>
	);

export template<std_random_access_iterator Iterator>
constexpr auto operator+(Iterator const it, bounded::bounded_integer auto const offset) -> Iterator {
	return it + static_cast<typename Iterator::difference_type>(offset);
}

export template<std_random_access_iterator Iterator>
constexpr auto operator+(bounded::bounded_integer auto const offset, Iterator const it) -> Iterator {
	return it + offset;
}

export template<std_iterator Iterator>
constexpr auto operator+(Iterator it, bounded::constant_t<1>) -> Iterator {
	++it;
	return it;
}

export template<std_iterator Iterator>
constexpr auto operator+(bounded::constant_t<1>, Iterator it) -> Iterator {
	++it;
	return it;
}

export template<std_bidirectional_iterator Iterator>
constexpr auto operator-(Iterator it, bounded::constant_t<1>) -> Iterator {
	--it;
	return it;
}

} // namespace bounded

namespace {

static_assert(bounded::std_random_access_iterator<std::vector<int>::iterator>);
static_assert(bounded::std_bidirectional_iterator<std::vector<int>::iterator>);
static_assert(bounded::std_iterator<std::vector<int>::iterator>);

static_assert(std::same_as<decltype(std::vector<int>::iterator() + bounded::constant<1>), std::vector<int>::iterator>);

struct iterator {
	struct value_type {
	};
	constexpr auto operator*() const -> value_type {
		return value_type();
	}
	constexpr auto operator++() & -> iterator & {
		return *this;
	}
};
static_assert(!bounded::std_iterator<iterator>);

} // namespace