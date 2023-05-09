// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.linear_map;

import containers.algorithms.advance;
import containers.algorithms.all_any_none;
import containers.algorithms.erase;
import containers.algorithms.find;
import containers.append;
import containers.begin_end;
import containers.c_array;
import containers.compare_container;
import containers.flat_map;
import containers.initializer_range;
import containers.is_range;
import containers.iterator_t;
import containers.lazy_push_back;
import containers.map_tags;
import containers.map_value_type;
import containers.maximum_array_size;
import containers.range_value_t;
import containers.range_view;
import containers.size;
import containers.static_vector;
import containers.test_associative_container;
import containers.test_reserve_and_capacity;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

export template<typename Container, typename Equal = std::equal_to<>>
struct basic_linear_map : private lexicographical_comparison::base {
	using value_type = range_value_t<Container>;
	using key_type = typename value_type::key_type;
	using mapped_type = typename value_type::mapped_type;

	using const_iterator = iterator_t<Container const &>;
	using iterator = iterator_t<Container &>;
	
	constexpr auto equal() const {
		return m_equal;
	}
	
	basic_linear_map() = default;
	template<std::same_as<Equal> Equal_ = Equal>
	constexpr explicit basic_linear_map(Equal_ equal_):
		m_equal(std::move(equal_))
	{
	}

	constexpr basic_linear_map(constructor_initializer_range<basic_linear_map> auto && source, Equal equal_):
		m_container(OPERATORS_FORWARD(source)),
		m_equal(std::move(equal_))
	{
		remove_duplicates();
	}
	constexpr explicit basic_linear_map(constructor_initializer_range<basic_linear_map> auto && source):
		basic_linear_map(OPERATORS_FORWARD(source), Equal())
	{
	}

	constexpr basic_linear_map(assume_unique_t, constructor_initializer_range<basic_linear_map> auto && source, Equal equal_):
		m_container(OPERATORS_FORWARD(source)),
		m_equal(std::move(equal_))
	{
	}
	constexpr basic_linear_map(assume_unique_t, constructor_initializer_range<basic_linear_map> auto && source):
		m_container(OPERATORS_FORWARD(source))
	{
	}


	template<std::size_t init_size>
	constexpr basic_linear_map(c_array<value_type, init_size> && source, Equal equal_):
		m_container(std::move(source)),
		m_equal(std::move(equal_))
	{
		remove_duplicates();
	}
	template<std::size_t init_size>
	constexpr basic_linear_map(c_array<value_type, init_size> && source):
		basic_linear_map(std::move(source), Equal())
	{
	}

	template<std::size_t init_size>
	constexpr basic_linear_map(assume_unique_t, c_array<value_type, init_size> && source, Equal equal_):
		m_container(std::move(source)),
		m_equal(std::move(equal_))
	{
	}
	template<std::size_t init_size>
	constexpr basic_linear_map(assume_unique_t, c_array<value_type, init_size> && source):
		m_container(std::move(source))
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
	
	// O(n) time
	constexpr auto find(auto const & key) const -> const_iterator {
		return containers::find_if(m_container, [&](auto const & value) { return equal()(key, value.key); });
	}
	// O(n) time
	constexpr auto find(auto const & key) -> iterator {
		return containers::find_if(m_container, [&](auto const & value) { return equal()(key, value.key); });
	}

	// O(n) time
	constexpr auto lazy_insert(auto && key, bounded::construct_function_for<mapped_type> auto && mapped) {
		auto const it = find(key);
		if (it != end()) {
			return inserted_t{it, false};
		}
		::containers::lazy_push_back(
			m_container,
			[&] { return value_type{OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)()}; }
		);
		return inserted_t{containers::prev(end()), true};
	}

	// O(n * m) time
	template<range Range>
	constexpr auto insert(Range && init) -> void {
		// TODO: This can selectively copy the right elements over instead of
		// copying everything and then removing things that shouldn't be there
		auto const original_count = containers::size(m_container);
		::containers::append(m_container, OPERATORS_FORWARD(init));
		remove_duplicates(begin() + original_count);
	}
	constexpr auto erase(const_iterator const it) {
		return containers::erase(m_container, it);
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) {
		return containers::erase(m_container, first, last);
	}

private:
	constexpr auto remove_duplicates(iterator starting_from) -> void {
		auto const first = containers::begin(m_container);
		auto to_remove = containers::end(m_container);
		while (starting_from != to_remove) {
			auto const unique_range = containers::range_view(first, starting_from);
			auto matches_this_value = [&](value_type const & verified) {
				return equal()(starting_from->key, verified.key);
			};
			if (::containers::any(unique_range, matches_this_value)) {
				--to_remove;
				if (starting_from == to_remove) {
					break;
				}
				std::ranges::swap(*starting_from, *to_remove);
			} else {
				++starting_from;
			}
		}
		::containers::erase_to_end(m_container, to_remove);
	}
	constexpr auto remove_duplicates() -> void {
		remove_duplicates(containers::begin(m_container));
	}

	// TODO: Use [[no_unique_address]] after resolution of
	// https://github.com/llvm/llvm-project/issues/53059
	Container m_container;
	[[no_unique_address]] Equal m_equal;
};

template<typename Range>
basic_linear_map(Range &&) -> basic_linear_map<std::remove_const_t<Range>>;
template<typename Range, typename Equal>
basic_linear_map(Range &&, Equal) -> basic_linear_map<std::remove_const_t<Range>, Equal>;

template<typename Range>
basic_linear_map(assume_unique_t, Range &&) -> basic_linear_map<std::remove_const_t<Range>>;
template<typename Range, typename Equal>
basic_linear_map(assume_unique_t, Range &&, Equal) -> basic_linear_map<std::remove_const_t<Range>, Equal>;

template<typename Range>
basic_linear_map(assume_sorted_unique_t, Range &&) -> basic_linear_map<std::remove_const_t<Range>>;
template<typename Range, typename Equal>
basic_linear_map(assume_sorted_unique_t, Range &&, Equal) -> basic_linear_map<std::remove_const_t<Range>, Equal>;


export template<typename Key, typename T, typename... MaybeExtractKey>
using linear_map = basic_linear_map<vector<map_value_type<Key, T>>, MaybeExtractKey...>;

export template<typename Key, typename T, array_size_type<map_value_type<Key, T>> capacity, typename... MaybeExtractKey>
using static_linear_map = basic_linear_map<static_vector<map_value_type<Key, T>, capacity>, MaybeExtractKey...>;

} // namespace containers

using non_copyable_map = containers::linear_map<bounded_test::non_copyable_integer, bounded_test::non_copyable_integer>;

static_assert(containers_test::test_reserve_and_capacity<non_copyable_map>());
static_assert(containers_test::test_associative_container<non_copyable_map>());
