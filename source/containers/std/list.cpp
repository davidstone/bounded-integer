// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.std.list;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.distance;
import containers.algorithms.erase;
import containers.algorithms.splice;
import containers.std.std_allocator;
import containers.assign;
import containers.assign_to_empty;
import containers.back;
import containers.begin_end;
import containers.bidirectional_linked_list;
import containers.clear;
import containers.emplace_back;
import containers.emplace_front;
import containers.front;
import containers.index_type;
import containers.initializer_range;
import containers.insert;
import containers.is_empty;
import containers.iter_value_t;
import containers.iterator;
import containers.iterator_t;
export import containers.legacy_iterator;
import containers.maximum_array_size;
import containers.pop_back;
import containers.pop_front;
import containers.push_back;
import containers.push_front;
import containers.repeat_n;
import containers.resize;
import containers.subrange;

import bounded;
import bounded.test_int;
import std_module;

namespace std_containers {

template<typename T>
struct sized_list {
	using size_type = std::size_t;
	using const_iterator = containers::legacy_iterator<typename containers::bidirectional_linked_list<T>::const_iterator>;
	using iterator = containers::legacy_iterator<typename containers::bidirectional_linked_list<T>::iterator>;

	constexpr sized_list() = default;
	sized_list(sized_list &&) = default;
	sized_list(sized_list const &) = default;

	constexpr explicit sized_list(containers::constructor_initializer_range<sized_list> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}

	auto operator=(sized_list &&) -> sized_list & = default;
	auto operator=(sized_list const &) -> sized_list & = default;

	friend constexpr auto swap(sized_list & lhs, sized_list & rhs) noexcept -> void {
		std::ranges::swap(lhs.m_impl, rhs.m_impl);
		std::ranges::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto begin() const noexcept -> const_iterator {
		return const_iterator(containers::begin(m_impl));
	}
	constexpr auto begin() noexcept -> iterator {
		return iterator(containers::begin(m_impl));
	}

	constexpr auto end() const noexcept -> const_iterator  {
		return const_iterator(containers::end(m_impl));
	}
	constexpr auto end() noexcept -> iterator  {
		return iterator(containers::end(m_impl));
	}

	constexpr auto size() const noexcept -> size_type {
		return static_cast<size_type>(m_size);
	}

	constexpr auto mutable_iterator(const_iterator const it) & -> iterator {
		return iterator(m_impl.mutable_iterator(it.base()));
	}

	constexpr auto lazy_push_back(bounded::construct_function_for<T> auto && constructor) -> T & {
		auto & value = m_impl.lazy_push_back(OPERATORS_FORWARD(constructor));
		++m_size;
		return value;
	}

	constexpr auto pop_back() -> void {
		m_impl.pop_back();
		--m_size;
	}

	constexpr auto splice(const_iterator const position, sized_list & other, const_iterator const first, const_iterator const last) -> void {
		if (std::addressof(other) != this) {
			auto const count = static_cast<size_type>(containers::distance(first, last));
			other.m_size -= count;
			m_size += count;
		}
		m_impl.splice(position.base(), other.m_impl, first.base(), last.base());
	}
	constexpr auto splice(const_iterator const position, sized_list & other, const_iterator const it) -> void {
		m_impl.splice(position.base(), other.m_impl, it.base(), containers::next(it.base()));
		--other.m_size;
		++m_size;
	}
	constexpr auto splice(const_iterator const position, sized_list & other) -> void {
		m_impl.splice(position.base(), other.m_impl, containers::begin(other.m_impl), containers::end(other.m_impl));
		m_size += std::exchange(other.m_size, 0);
	}

	friend constexpr auto operator<=>(sized_list const &, sized_list const &) = default;

private:
	containers::bidirectional_linked_list<T> m_impl;
	size_type m_size = 0;
};

// This has the same interface as `std::list`, with the following exceptions:
//
// Allocators are not supported (`std::allocator` is used for all allocations).
//
// `operator<=>` is required for relational operators to exist.
//
// Behavior if `allocate` throws is untested, but should give at least the weak
// exception guarantee in all circumstances (usually the strong guarantee).
//
// `max_size` is static. I believe this is a conforming extension.
//
// `constexpr` is supported.
//
// This is possibly not a complete list. It is expected that this list will
// shrink over time, as the goal is to fix most of these inconsistencies.

export template<typename T, typename Allocator = std::allocator<T>>
struct list {
	static_assert(std_allocator<Allocator>);
	using value_type = T;
	using allocator_type = std::allocator<T>;
	using size_type = typename sized_list<T>::size_type;
	using difference_type = std::ptrdiff_t;
	using const_reference = T const &;
	using reference = T &;
	using const_pointer = T const *;
	using pointer = T *;
	using const_iterator = typename sized_list<T>::const_iterator;
	using iterator = typename sized_list<T>::iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	constexpr list() = default;

	constexpr list(size_type const count, T const & value) {
		containers::assign_to_empty(m_impl, containers::repeat_n(count, value));
	}
	constexpr explicit list(size_type const count) {
		containers::assign_to_empty(m_impl, containers::repeat_default_n<T>(count));
	}

	template<containers::iterator InputIterator>
	constexpr list(InputIterator first, InputIterator last) {
		containers::assign_to_empty(m_impl, containers::subrange(std::move(first), std::move(last)));
	}

	constexpr list(std::initializer_list<T> init) {
		containers::assign_to_empty(m_impl, init);
	}

	list(list &&) = default;
	list(list const &) = default;

	auto operator=(list &&) -> list & = default;
	auto operator=(list const &) -> list & = default;

	constexpr auto operator=(std::initializer_list<T> init) -> list & {
		containers::assign(*this, init);
		return *this;
	}

	friend constexpr auto swap(list & lhs, list & rhs) noexcept -> void {
		std::ranges::swap(lhs.m_impl, rhs.m_impl);
	}
	constexpr auto swap(list & other) noexcept -> void {
		std::ranges::swap(m_impl, other.m_impl);
	}

	constexpr auto assign(size_type const count, T const & value) -> void {
		containers::assign(m_impl, containers::repeat_n(count, value));
	}

	template<containers::iterator InputIterator>
	constexpr auto assign(InputIterator first, InputIterator last) -> void {
		containers::assign(m_impl, containers::subrange(std::move(first), std::move(last)));
	}

	constexpr auto assign(std::initializer_list<T> init) -> void {
		containers::assign(m_impl, init);
	}

	constexpr auto get_allocator() const noexcept {
		return allocator_type();
	}

	constexpr auto front() const -> const_reference {
		return containers::front(m_impl);
	}
	constexpr auto front() -> reference {
		return containers::front(m_impl);
	}

	constexpr auto back() const -> const_reference {
		return containers::back(m_impl);
	}
	constexpr auto back() -> reference {
		return containers::back(m_impl);
	}

	constexpr auto begin() const noexcept -> const_iterator {
		return m_impl.begin();
	}
	constexpr auto begin() noexcept -> iterator {
		return m_impl.begin();
	}
	constexpr auto cbegin() const noexcept -> const_iterator {
		return begin();
	}

	constexpr auto end() const noexcept -> const_iterator  {
		return m_impl.end();
	}
	constexpr auto end() noexcept -> iterator  {
		return m_impl.end();
	}
	constexpr auto cend() const noexcept -> const_iterator  {
		return end();
	}

	constexpr auto rbegin() const noexcept -> const_reverse_iterator {
		return std::make_reverse_iterator(begin());
	}
	constexpr auto rbegin() noexcept -> reverse_iterator {
		return std::make_reverse_iterator(begin());
	}
	constexpr auto crbegin() const noexcept -> const_reverse_iterator {
		return rbegin();
	}

	constexpr auto rend() const noexcept -> const_reverse_iterator {
		return std::make_reverse_iterator(end());
	}
	constexpr auto rend() noexcept -> reverse_iterator {
		return std::make_reverse_iterator(end());
	}
	constexpr auto crend() const noexcept -> const_reverse_iterator {
		return rend();
	}

	[[nodiscard]] constexpr auto empty() const noexcept -> bool {
		return containers::is_empty(m_impl);
	}

	constexpr auto size() const noexcept -> size_type {
		return m_impl.size();
	}

	static constexpr auto max_size() noexcept -> size_type {
		return containers::maximum_array_size<T>;
	}

	constexpr auto clear() noexcept -> void {
		containers::clear(m_impl);
	}

	constexpr auto insert(const_iterator const position, T && value) -> iterator {
		return containers::legacy_iterator(containers::insert(
			m_impl,
			position.base(),
			std::move(value)
		));
	}
	constexpr auto insert(const_iterator const position, T const & value) -> iterator {
		return containers::legacy_iterator(containers::insert(
			m_impl,
			position.base(),
			value
		));
	}
	constexpr auto insert(const_iterator const position, size_type const count, T const & value) -> iterator {
		return containers::legacy_iterator(containers::insert(
			m_impl,
			position.base(),
			containers::repeat_n(count, value)
		));
	}
	template<containers::iterator InputIterator>
	constexpr auto insert(const_iterator const position, InputIterator first, InputIterator last) -> iterator {
		return containers::legacy_iterator(containers::insert(
			m_impl,
			position.base(),
			containers::subrange(std::move(first), std::move(last))
		));
	}
	constexpr auto insert(const_iterator const position, std::initializer_list<T> init) -> iterator {
		return containers::legacy_iterator(containers::insert(
			m_impl,
			position.base(),
			init
		));
	}

	constexpr auto emplace(const_iterator const position, auto && ... args) -> iterator {
		return containers::legacy_iterator(containers::emplace(m_impl, position.base(), OPERATORS_FORWARD(args)...));
	}

	constexpr auto erase(const_iterator const position) -> iterator {
		return containers::legacy_iterator(containers::erase(m_impl, position.base()));
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) -> iterator {
		return containers::legacy_iterator(containers::erase(m_impl, first.base(), last.base()));
	}

	constexpr auto push_back(T const & value) -> void {
		containers::push_back(m_impl, value);
	}
	constexpr auto push_back(T && value) -> void {
		containers::push_back(m_impl, std::move(value));
	}

	constexpr auto emplace_back(auto && ... args) -> reference {
		return containers::emplace_back(m_impl, OPERATORS_FORWARD(args)...);
	}

	constexpr auto pop_back() -> void {
		containers::pop_back(m_impl);
	}

	constexpr auto push_front(T const & value) -> void {
		containers::push_front(m_impl, value);
	}
	constexpr auto push_front(T && value) -> void {
		containers::push_front(m_impl, std::move(value));
	}

	constexpr auto emplace_front(auto && ... args) -> reference {
		return containers::emplace_front(m_impl, OPERATORS_FORWARD(args)...);
	}
	constexpr auto pop_front() -> void {
		containers::pop_front(m_impl);
	}

	constexpr auto resize(size_type const count) -> void {
		containers::resize(m_impl, count);
	}
	constexpr auto resize(size_type const count, T const & value) -> void {
		containers::resize(m_impl, count, value);
	}

	constexpr auto merge(list &, auto const) -> void {
		// TODO: Implement
	}
	constexpr auto merge(list && other, auto const compare) -> void {
		merge(other, compare);
	}
	constexpr auto merge(list & other) -> void {
		merge(other, std::less());
	}
	constexpr auto merge(list && other) -> void {
		merge(other);
	}

	constexpr auto splice(const_iterator const position, list & other, const_iterator const first, const_iterator const last) -> void {
		m_impl.splice(position, other, first, last);
	}
	constexpr auto splice(const_iterator const position, list && other, const_iterator const first, const_iterator const last) -> void {
		m_impl.splice(position, other, first, last);
	}
	constexpr auto splice(const_iterator const position, list & other, const_iterator const it) -> void {
		::containers::splice(m_impl, position, other, it);
	}
	constexpr auto splice(const_iterator const position, list && other, const_iterator const it) -> void {
		::containers::splice(m_impl, position, other, it);
	}
	constexpr auto splice(const_iterator const position, list & other) -> void {
		m_impl.splice(position, other);
	}
	constexpr auto splice(const_iterator const position, list && other) -> void {
		m_impl.splice(position, other);
	}

	constexpr auto remove_if(auto const predicate) -> size_type {
		return static_cast<size_type>(containers::erase_if(m_impl, predicate));
	}
	constexpr auto remove(T const & value) -> size_type {
		return remove_if(bounded::equal_to(value));
	}

	constexpr auto reverse() noexcept -> void {
		// TODO: Implement
	}

	constexpr auto unique(auto const) -> size_type {
		// TODO: Implement
		return 0;
	}
	constexpr auto unique(T const & value) -> size_type {
		return unique(bounded::equal_to(value));
	}

	constexpr auto sort(auto const) -> void {
		// TODO: Implement
		// stable sort
	}
	constexpr auto sort() -> void {
		sort(std::less());
	}

	friend constexpr auto operator<=>(list const &, list const &) = default;

private:
	sized_list<T> m_impl;
};

template<containers::iterator InputIterator>
list(InputIterator, InputIterator) -> list<containers::iter_value_t<InputIterator>>;

} // namespace std_containers

using namespace bounded::literal;

static_assert(containers::equal(std_containers::list<bounded_test::integer>(), containers::bidirectional_linked_list<bounded_test::integer>()));
static_assert(containers::equal(std_containers::list<bounded_test::integer>{}, containers::bidirectional_linked_list<bounded_test::integer>()));
static_assert(containers::equal(std_containers::list<bounded_test::integer>{1}, containers::bidirectional_linked_list<bounded_test::integer>({1})));
static_assert(containers::equal(std_containers::list<bounded_test::integer>{1, 4}, containers::bidirectional_linked_list<bounded_test::integer>({1, 4})));

static_assert(std_containers::list<bounded_test::integer>() == std_containers::list<bounded_test::integer>());
static_assert(std_containers::list<bounded_test::integer>() == std_containers::list<bounded_test::integer>{});
static_assert(std_containers::list<bounded_test::integer>() != std_containers::list<bounded_test::integer>{1});
static_assert(std_containers::list<bounded_test::integer>{1} == std_containers::list<bounded_test::integer>{1});

static_assert([]{
	auto v = std_containers::list<bounded_test::integer>();
	v.assign(4, 1);
	return v == std_containers::list<bounded_test::integer>{1, 1, 1, 1};
}());

static_assert([]{
	auto v = std_containers::list<bounded_test::integer>();
	constexpr int input[] = {0, 3, 2, 1};
	v.assign(std::begin(input), std::end(input));
	return v == std_containers::list<bounded_test::integer>{0, 3, 2, 1};
}());

static_assert([]{
	auto v = std_containers::list<bounded_test::integer>();
	v.assign({1, 3, 6, 3});
	return v == std_containers::list<bounded_test::integer>{1, 3, 6, 3};
}());

static_assert([]{
	auto v = std_containers::list<bounded_test::integer>();
	auto const init = std_containers::list<bounded_test::integer>();
	v.insert(v.end(), init.begin(), init.end());
	return true;
}());

static_assert([]{
	using list = std_containers::list<bounded_test::integer>;
	auto v = list({1, 2});
	v.insert(v.begin(), *containers::next(v.begin()));
	return v == list({2, 1, 2});
}());
