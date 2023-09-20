// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


module;

#include <operators/forward.hpp>

export module containers.std.vector;

import containers.algorithms.compare;
import containers.algorithms.erase;
import containers.std.std_allocator;
import containers.assign;
import containers.at;
import containers.begin_end;
import containers.clear;
import containers.data;
import containers.emplace_back;
import containers.front_back;
import containers.index_type;
import containers.insert;
import containers.is_empty;
import containers.is_iterator;
import containers.iter_value_t;
import containers.iterator_t;
import containers.maximum_array_size;
import containers.pop_back;
import containers.push_back;
import containers.range_view;
import containers.repeat_n;
import containers.resize;
import containers.shrink_to_fit;
import containers.size;
import containers.to_address;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

namespace std_containers {

// This has the same interface as `std::vector`, with the following exceptions:
//
// Allocators are not supported (`std::allocator` is used for all allocations).
//
// `operator<=>` is required for relational operators to exist
//
// vector<bool> doesn't have special behavior yet
//
// Assignment is assumed to be equivalent to destroy + construct
//
// All types are required to have a nothrow `relocate`. `relocate` is assumed to
// be equivalent to move + destroy.
//
// Move construction and assignment are assumed to not throw exceptions.
//
// Behavior if `allocate` throws is untested, but should give at least the weak
// exception guarantee in all circumstances (usually the strong guarantee).
//
// `max_size` is static. I believe this is a conforming extension.
//
// This is possibly not a complete list. It is expected that this list will
// shrink over time, as the goal is to fix most of these inconsistencies.

export template<typename T, typename Allocator = std::allocator<T>>
struct vector {
	static_assert(std_allocator<Allocator>);
	using value_type = T;
	using allocator_type = std::allocator<T>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_reference = T const &;
	using reference = T &;
	using const_pointer = T const *;
	using pointer = T *;
	using const_iterator = T const *;
	using iterator = T *;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	constexpr vector() = default;

	constexpr vector(size_type const count, T const & value):
		m_impl(containers::repeat_n(count, value))
	{
	}
	constexpr explicit vector(size_type const count):
		m_impl(containers::repeat_default_n<T>(count))
	{
	}

	template<containers::iterator InputIterator>
	constexpr vector(InputIterator first, InputIterator last):
		m_impl(containers::range_view(std::move(first), std::move(last)))
	{
	}

	constexpr vector(std::initializer_list<T> init):
		m_impl(init)
	{
	}

	vector(vector &&) = default;
	vector(vector const &) = default;

	auto operator=(vector &&) -> vector & = default;
	auto operator=(vector const &) -> vector & = default;

	constexpr auto operator=(std::initializer_list<T> init) -> vector & {
		containers::assign(*this, init);
		return *this;
	}

	friend constexpr auto swap(vector & lhs, vector & rhs) noexcept -> void {
		std::ranges::swap(lhs.m_impl, rhs.m_impl);
	}
	constexpr auto swap(vector & other) noexcept -> void {
		std::ranges::swap(*this, other);
	}

	constexpr auto assign(size_type const count, T const & value) -> void {
		containers::assign(m_impl, containers::repeat_n(count, value));
	}

	template<containers::iterator InputIterator>
	constexpr auto assign(InputIterator first, InputIterator last) -> void {
		containers::assign(m_impl, containers::range_view(std::move(first), std::move(last)));
	}

	constexpr auto assign(std::initializer_list<T> init) -> void {
		containers::assign(m_impl, init);
	}

	constexpr auto get_allocator() const noexcept {
		return allocator_type();
	}

	constexpr auto at(size_type const index) const -> const_reference {
		return containers::at(m_impl, index);
	}
	constexpr auto && at(size_type const index) {
		return containers::at(m_impl, index);
	}

	constexpr auto operator[](size_type const index) const -> const_reference {
		return m_impl[::bounded::assume_in_range<containers::index_type<impl_t>>(index)];
	}
	constexpr auto operator[](size_type const index) -> reference {
		return m_impl[::bounded::assume_in_range<containers::index_type<impl_t>>(index)];
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

	constexpr auto data() const -> const_pointer {
		return containers::data(m_impl);
	}
	constexpr auto data() -> pointer {
		return containers::data(m_impl);
	}

	constexpr auto begin() const noexcept -> const_iterator {
		return containers::to_address(containers::begin(m_impl));
	}
	constexpr auto begin() noexcept -> iterator {
		return containers::to_address(containers::begin(m_impl));
	}
	constexpr auto cbegin() const noexcept -> const_iterator {
		return begin();
	}

	constexpr auto end() const noexcept -> const_iterator  {
		return containers::to_address(containers::end(m_impl));
	}
	constexpr auto end() noexcept -> iterator  {
		return containers::to_address(containers::end(m_impl));
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

	constexpr auto reserve(size_type const requested_capacity) -> void {
		m_impl.reserve(bounded::check_in_range<containers::range_size_t<impl_t>, std::length_error>(bounded::integer(requested_capacity)));
	}

	constexpr auto capacity() const noexcept -> size_type {
		return static_cast<size_type>(m_impl.capacity());
	}

	constexpr auto shrink_to_fit() -> void {
		containers::shrink_to_fit(m_impl);
	}

	[[nodiscard]] constexpr auto empty() const noexcept -> bool {
		return containers::is_empty(m_impl);
	}

	constexpr auto size() const noexcept -> size_type {
		return static_cast<size_type>(containers::size(m_impl));
	}

	static constexpr auto max_size() noexcept -> size_type {
		return containers::maximum_array_size<T>;
	}

	constexpr auto clear() noexcept -> void {
		containers::clear(m_impl);
	}

	constexpr auto insert(const_iterator const position, T && value) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), std::move(value)));
	}
	constexpr auto insert(const_iterator const position, T const & value) -> iterator {
		if (object_is_in_storage(value)) {
			return insert(position, T(value));
		} else {
			return containers::to_address(containers::insert(m_impl, impl_iterator(position), value));
		}
	}
	constexpr auto insert(const_iterator const position, size_type const count, T const & value) -> iterator {
		auto do_insert = [&](auto range) {
			return containers::to_address(containers::insert(m_impl, impl_iterator(position), std::move(range)));
		};
		if (object_is_in_storage(value)) {
			return do_insert(containers::repeat_n(count, T(value)));
		} else {
			return do_insert(containers::repeat_n(count, value));
		}
	}
	template<containers::iterator InputIterator>
	constexpr auto insert(const_iterator const position, InputIterator first, InputIterator last) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), containers::range_view(std::move(first), std::move(last))));
	}
	constexpr auto insert(const_iterator const position, std::initializer_list<T> init) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), init));
	}

	constexpr auto emplace(const_iterator const position, auto && ... args) -> iterator {
		auto it = impl_iterator(position);
		// Handle https://cplusplus.github.io/LWG/issue2164
		auto out = (... and std::is_rvalue_reference_v<decltype(args)>) ?
			containers::emplace(m_impl, it, OPERATORS_FORWARD(args)...) :
			containers::insert(m_impl, it, T(OPERATORS_FORWARD(args)...));
		return containers::to_address(out);
	}

	constexpr auto erase(const_iterator const position) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(position)));
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(first), impl_iterator(last)));
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

	constexpr auto resize(size_type const count) -> void {
		containers::resize(m_impl, count);
	}
	constexpr auto resize(size_type const count, T const & value) -> void {
		containers::resize(m_impl, count, value);
	}

	friend constexpr auto operator<=>(vector const &, vector const &) = default;

	// Extensions
	constexpr auto set_size(auto const new_size) -> void {
		m_impl.set_size(new_size);
	}

private:
	using impl_t = containers::vector<T, bounded::constant<max_size()>>;
	static constexpr auto impl_iterator(const_iterator it) {
		return containers::iterator_t<impl_t const &>(it);
	}
	constexpr auto object_is_in_storage(T const & object) const {
		auto const ptr = std::addressof(object);
		return std::less_equal()(data(), ptr) and std::less()(ptr, data() + size());
	}

	impl_t m_impl;
};

template<containers::iterator InputIterator>
vector(InputIterator, InputIterator) -> vector<containers::iter_value_t<InputIterator>>;

} // namespace std_containers

using namespace bounded::literal;

static_assert(containers::equal(std_containers::vector<bounded_test::integer>(), containers::vector<bounded_test::integer>()));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{}, containers::vector<bounded_test::integer>()));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{1}, containers::vector<bounded_test::integer>({1})));
static_assert(containers::equal(std_containers::vector<bounded_test::integer>{1, 4}, containers::vector<bounded_test::integer>({1, 4})));

static_assert(std_containers::vector<bounded_test::integer>() == std_containers::vector<bounded_test::integer>());
static_assert(std_containers::vector<bounded_test::integer>() == std_containers::vector<bounded_test::integer>{});
static_assert(std_containers::vector<bounded_test::integer>() != std_containers::vector<bounded_test::integer>{1});
static_assert(std_containers::vector<bounded_test::integer>{1} == std_containers::vector<bounded_test::integer>{1});

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	v.assign(4, 1);
	return v == std_containers::vector<bounded_test::integer>{1, 1, 1, 1};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	constexpr int input[] = {0, 3, 2, 1};
	v.assign(std::begin(input), std::end(input));
	return v == std_containers::vector<bounded_test::integer>{0, 3, 2, 1};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	v.assign({1, 3, 6, 3});
	return v == std_containers::vector<bounded_test::integer>{1, 3, 6, 3};
}());

static_assert([]{
	auto v = std_containers::vector<bounded_test::integer>();
	auto const init = std_containers::vector<bounded_test::integer>();
	v.insert(v.end(), init.begin(), init.end());
	return true;
}());

static_assert([]{
	using vector = std_containers::vector<bounded_test::integer>;
	auto v = vector({1, 2});
	v.reserve(4);
	v.insert(v.begin(), v[1]);
	return v == vector({2, 1, 2});
}());
