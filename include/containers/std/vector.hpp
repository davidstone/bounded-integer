// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/assign.hpp>
#include <containers/at.hpp>
#include <containers/begin_end.hpp>
#include <containers/clear.hpp>
#include <containers/data.hpp>
#include <containers/emplace_back.hpp>
#include <containers/front_back.hpp>
#include <containers/index_type.hpp>
#include <containers/insert.hpp>
#include <containers/is_empty.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/pop_back.hpp>
#include <containers/push_back.hpp>
#include <containers/range_view.hpp>
#include <containers/repeat_n.hpp>
#include <containers/resize.hpp>
#include <containers/shrink_to_fit.hpp>
#include <containers/vector.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <cstddef>
#include <initializer_list>
#include <iterator>

namespace std_containers {
namespace detail {

template<typename T>
inline constexpr auto is_std_allocator = false;

template<typename T>
inline constexpr auto is_std_allocator<std::allocator<T>> = true;

template<typename T>
concept std_allocator = is_std_allocator<T>;

} // namespace detail

// This has the same interface as `std::vector`, with the following exceptions:
//
// Names are not uglified (`__foo`). When this becomes a module, that should not
// matter.
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
// `length_error` is never thrown.
//
// `max_size` is static. I believe this is a conforming extension.
//
// Some operations (such as assign) might not support the source element being a
// reference to an element of the vector
//
// This is possibly not a complete list. It is expected that this list will
// shrink over time, as the goal is to fix most of these inconsistencies.

template<typename T, detail::std_allocator = std::allocator<T>>
struct vector {
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

	friend constexpr void swap(vector & lhs, vector & rhs) noexcept {
		using std::swap;
		swap(lhs.m_impl, rhs.m_impl);
	}

	constexpr void assign(size_type const count, T const & value) {
		containers::assign(m_impl, containers::repeat_n(count, value));
	}

	template<containers::iterator InputIterator>
	constexpr void assign(InputIterator first, InputIterator last) {
		containers::assign(m_impl, containers::range_view(std::move(first), std::move(last)));
	}

	constexpr void assign(std::initializer_list<T> init) {
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
		return m_impl[static_cast<containers::index_type<impl_t>>(index)];
	}
	constexpr auto operator[](size_type const index) -> reference {
		return m_impl[static_cast<containers::index_type<impl_t>>(index)];
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

	constexpr void reserve(size_type const requested_capacity) {
		m_impl.reserve(containers::range_size_t<impl_t>(requested_capacity));
	}

	constexpr auto capacity() const noexcept -> size_type {
		return static_cast<size_type>(m_impl.capacity());
	}

	constexpr void shrink_to_fit() {
		containers::shrink_to_fit(m_impl);
	}

	[[nodiscard]] constexpr auto empty() const noexcept -> bool {
		return containers::is_empty(m_impl);
	}

	constexpr auto size() const noexcept -> size_type {
		return static_cast<size_type>(containers::size(m_impl));
	}

	static constexpr auto max_size() noexcept -> size_type {
		return containers::detail::maximum_array_size<T>;
	}

	constexpr void clear() noexcept {
		containers::clear(m_impl);
	}

	constexpr auto insert(const_iterator const position, T const & value) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), value));
	}
	constexpr auto insert(const_iterator const position, T && value) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), value));
	}
	constexpr auto insert(const_iterator const position, size_type const count, T const & value) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), containers::repeat_n(count, value)));
	}
	template<containers::iterator InputIterator>
	constexpr auto insert(const_iterator const position, InputIterator first, InputIterator last) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), containers::range_view(std::move(first), std::move(last))));
	}
	constexpr auto insert(const_iterator const position, std::initializer_list<T> init) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), init));
	}

	constexpr auto emplace(const_iterator const position, auto && ... args) -> iterator {
		return containers::to_address(containers::emplace(m_impl, impl_iterator(position), OPERATORS_FORWARD(args)...));
	}

	constexpr auto erase(const_iterator const position) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(position)));
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(first), impl_iterator(last)));
	}

	constexpr void push_back(T const & value) {
		containers::push_back(m_impl, value);
	}
	constexpr void push_back(T && value) {
		containers::push_back(m_impl, std::move(value));
	}

	constexpr auto emplace_back(auto && ... args) -> reference {
		return containers::emplace_back(m_impl, OPERATORS_FORWARD(args)...);
	}

	constexpr void pop_back() {
		containers::pop_back(m_impl);
	}

	constexpr void resize(size_type const count) {
		containers::resize(m_impl, count);
	}
	constexpr void resize(size_type const count, T const & value) {
		containers::resize(m_impl, count, value);
	}

	friend constexpr auto operator<=>(vector const &, vector const &) = default;

	// Extensions
	constexpr void append_from_capacity(auto const count) {
		m_impl.append_from_capacity(count);
	}

private:
	using impl_t = containers::vector<T, max_size()>;
	static constexpr auto impl_iterator(const_iterator it) {
		return containers::iterator_t<impl_t const &>(it);
	}
	impl_t m_impl;
};

template<containers::iterator InputIterator>
vector(InputIterator, InputIterator) -> vector<containers::iter_value_t<InputIterator>>;

} // namespace std_containers
