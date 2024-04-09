// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


module;

#include <operators/forward.hpp>

export module containers.std.inplace_vector;

import containers.algorithms.compare;
import containers.algorithms.erase;
import containers.std.std_allocator;
import containers.append;
import containers.assign;
import containers.at;
import containers.back;
import containers.begin_end;
import containers.clear;
import containers.data;
import containers.emplace_back;
import containers.emplace_back_into_capacity;
import containers.front;
import containers.index_type;
import containers.insert;
import containers.is_empty;
import containers.iter_value_t;
import containers.iterator;
import containers.iterator_t;
import containers.maximum_array_size;
import containers.pop_back;
import containers.push_back;
import containers.push_back_into_capacity;
import containers.range_size_t;
import containers.range_view;
import containers.repeat_n;
import containers.resize;
import containers.shrink_to_fit;
import containers.static_vector;
import containers.to_address;

import bounded;
import bounded.test_int;
import std_module;

namespace std_containers {

template<typename, typename>
concept container_compatible_range = true;

// This has the same interface as `std::inplace_vector`, with the following exceptions:
//
// Assignment is assumed to be equivalent to destroy + construct
//
// All types are required to have a nothrow `relocate`. `relocate` is assumed to
// be equivalent to move + destroy.
//
// The assignment and relocation requirements in turn cause move construction
// and move assignment to be unconditionally noexcept.

export template<typename T, std::size_t capacity_>
struct inplace_vector {
	using value_type = T;
	using pointer = T *;
	using const_pointer = T const *;
	using reference = T &;
	using const_reference = T const &;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = T *;
	using const_iterator = T const *;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	constexpr inplace_vector() = default;

	constexpr explicit inplace_vector(size_type const count):
		m_impl(containers::repeat_default_n<T>(count))
	{
	}

	constexpr inplace_vector(size_type const count, T const & value):
		m_impl(containers::repeat_n(count, value))
	{
	}

	template<typename InputIterator>
	constexpr inplace_vector(InputIterator first, InputIterator last):
		m_impl(containers::range_view(std::move(first), std::move(last)))
	{
	}

	#if 0
	// std::from_range_t is not yet generally implemented
	template<container_compatible_range<T> R>
	constexpr inplace_vector(std::from_range_t, R && range):
		m_impl(OPERATORS_FORWARD(range))
	{
	}
	#endif

	inplace_vector(inplace_vector const &) = default;
	inplace_vector(inplace_vector &&) = default;

	constexpr inplace_vector(std::initializer_list<T> init):
		m_impl(init)
	{
	}

	auto operator=(inplace_vector const &) -> inplace_vector & = default;
	auto operator=(inplace_vector &&) -> inplace_vector & = default;

	constexpr auto operator=(std::initializer_list<T> init) -> inplace_vector & {
		containers::assign(*this, init);
		return *this;
	}

	template<containers::iterator InputIterator>
	constexpr auto assign(InputIterator first, InputIterator last) -> void {
		containers::assign(m_impl, containers::range_view(std::move(first), std::move(last)));
	}

	template<container_compatible_range<T> Range>
	constexpr auto assign_range(Range && range) -> void {
		containers::assign(m_impl, OPERATORS_FORWARD(range));
	}

	constexpr auto assign(size_type const count, T const & value) -> void {
		containers::assign(m_impl, containers::repeat_n(count, value));
	}

	constexpr auto assign(std::initializer_list<T> init) -> void {
		containers::assign(m_impl, init);
	}

	constexpr auto begin() noexcept -> iterator {
		return containers::to_address(containers::begin(m_impl));
	}
	constexpr auto begin() const noexcept -> const_iterator {
		return containers::to_address(containers::begin(m_impl));
	}
	constexpr auto end() noexcept -> iterator  {
		return containers::to_address(containers::end(m_impl));
	}
	constexpr auto end() const noexcept -> const_iterator  {
		return containers::to_address(containers::end(m_impl));
	}
	constexpr auto rbegin() noexcept -> reverse_iterator {
		return std::make_reverse_iterator(begin());
	}
	constexpr auto rbegin() const noexcept -> const_reverse_iterator {
		return std::make_reverse_iterator(begin());
	}
	constexpr auto rend() noexcept -> reverse_iterator {
		return std::make_reverse_iterator(end());
	}
	constexpr auto rend() const noexcept -> const_reverse_iterator {
		return std::make_reverse_iterator(end());
	}

	constexpr auto cbegin() const noexcept -> const_iterator {
		return begin();
	}
	constexpr auto cend() const noexcept -> const_iterator  {
		return end();
	}
	constexpr auto crbegin() const noexcept -> const_reverse_iterator {
		return rbegin();
	}
	constexpr auto crend() const noexcept -> const_reverse_iterator {
		return rend();
	}

	[[nodiscard]] constexpr auto empty() const noexcept -> bool {
		return containers::is_empty(m_impl);
	}

	constexpr auto size() const noexcept -> size_type {
		return static_cast<size_type>(m_impl.size());
	}

	static constexpr auto max_size() noexcept -> size_type {
		return static_cast<size_type>(capacity_);
	}

	static constexpr auto capacity() noexcept -> size_type {
		return static_cast<size_type>(capacity_);
	}

	constexpr auto resize(size_type const count) -> void {
		containers::resize(m_impl, count);
	}
	constexpr auto resize(size_type const count, T const & value) -> void {
		containers::resize(m_impl, count, value);
	}

	static constexpr auto reserve(size_type const requested_capacity) -> void {
		if (requested_capacity > capacity()) {
			throw std::bad_alloc();
		}
	}

	static constexpr auto shrink_to_fit() noexcept -> void {
	}

	constexpr auto operator[](size_type const index) -> reference {
		return m_impl[::bounded::assume_in_range<containers::index_type<impl_t>>(index)];
	}
	constexpr auto operator[](size_type const index) const -> const_reference {
		return m_impl[::bounded::assume_in_range<containers::index_type<impl_t>>(index)];
	}

	constexpr auto at(size_type const index) const -> const_reference {
		return containers::at(m_impl, index);
	}
	constexpr auto && at(size_type const index) {
		return containers::at(m_impl, index);
	}

	constexpr auto front() -> reference {
		return containers::front(m_impl);
	}
	constexpr auto front() const -> const_reference {
		return containers::front(m_impl);
	}

	constexpr auto back() -> reference {
		return containers::back(m_impl);
	}
	constexpr auto back() const -> const_reference {
		return containers::back(m_impl);
	}

	constexpr auto data() noexcept -> pointer {
		return containers::data(m_impl);
	}
	constexpr auto data() const noexcept -> const_pointer {
		return containers::data(m_impl);
	}

	constexpr auto emplace_back(auto && ... args) -> reference {
		return containers::emplace_back(m_impl, OPERATORS_FORWARD(args)...);
	}

	constexpr auto push_back(T const & value) -> reference {
		return containers::push_back(m_impl, value);
	}
	constexpr auto push_back(T && value) -> reference {
		return containers::push_back(m_impl, std::move(value));
	}

	template<container_compatible_range<T> Range>
	constexpr auto append_range(Range && range) -> void {
		containers::append(m_impl, OPERATORS_FORWARD(range));
	}

	constexpr auto pop_back() -> void {
		containers::pop_back(m_impl);
	}

	constexpr auto try_emplace_back(auto && ... args) -> pointer {
		return size() < capacity() ?
			std::addressof(containers::emplace_back(m_impl, OPERATORS_FORWARD(args)...)) :
			nullptr;
	}

	constexpr auto try_push_back(T const & value) -> pointer {
		return size() < capacity() ?
			std::addressof(containers::push_back(m_impl, value)) :
			nullptr;
	}
	constexpr auto try_push_back(T && value) -> pointer {
		return size() < capacity() ?
			std::addressof(containers::push_back(m_impl, std::move(value))) :
			nullptr;
	}

	#if 0
	// TODO
	template<container_compatible_range<T> Range>
	constexpr auto try_append_range(Range && range) -> ranges::borrowed_iterator_t<Range> {
	}
	#endif

	constexpr auto unchecked_emplace_back(auto && ... args) -> reference {
		return containers::emplace_back_into_capacity(m_impl, OPERATORS_FORWARD(args)...);
	}

	constexpr auto unchecked_push_back(T const & value) -> reference {
		return containers::push_back_into_capacity(m_impl, value);
	}
	constexpr auto unchecked_push_back(T && value) -> reference {
		return containers::push_back_into_capacity(m_impl, std::move(value));
	}

	constexpr auto emplace(const_iterator const position, auto && ... args) -> iterator {
		auto it = impl_iterator(position);
		// Handle https://cplusplus.github.io/LWG/issue2164
		auto out = (... and std::is_rvalue_reference_v<decltype(args)>) ?
			containers::emplace(m_impl, it, OPERATORS_FORWARD(args)...) :
			containers::insert(m_impl, it, T(OPERATORS_FORWARD(args)...));
		return containers::to_address(out);
	}

	constexpr auto insert(const_iterator const position, T const & value) -> iterator {
		if (object_is_in_storage(value)) {
			return insert(position, T(value));
		} else {
			return containers::to_address(containers::insert(m_impl, impl_iterator(position), value));
		}
	}
	constexpr auto insert(const_iterator const position, T && value) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), std::move(value)));
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
	template<container_compatible_range<T> Range>
	constexpr auto insert_range(const_iterator const position, Range && range) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), OPERATORS_FORWARD(range)));
	}
	constexpr auto insert(const_iterator const position, std::initializer_list<T> init) -> iterator {
		return containers::to_address(containers::insert(m_impl, impl_iterator(position), init));
	}

	constexpr auto erase(const_iterator const position) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(position)));
	}
	constexpr auto erase(const_iterator const first, const_iterator const last) -> iterator {
		return containers::to_address(containers::erase(m_impl, impl_iterator(first), impl_iterator(last)));
	}

	constexpr auto swap(inplace_vector & other) noexcept -> void {
		std::ranges::swap(*this, other);
	}

	constexpr auto clear() noexcept -> void {
		containers::clear(m_impl);
	}

	friend constexpr auto operator<=>(inplace_vector const &, inplace_vector const &) = default;

	friend constexpr auto swap(inplace_vector & lhs, inplace_vector & rhs) noexcept -> void {
		std::ranges::swap(lhs.m_impl, rhs.m_impl);
	}

	// Extensions
	constexpr auto set_size(auto const new_size) -> void {
		m_impl.set_size(new_size);
	}

private:
	using impl_t = containers::static_vector<T, bounded::constant<max_size()>>;
	static constexpr auto impl_iterator(const_iterator it) {
		return containers::iterator_t<impl_t const &>(it);
	}
	constexpr auto object_is_in_storage(T const & object) const {
		auto const ptr = std::addressof(object);
		return std::less_equal()(data(), ptr) and std::less()(ptr, data() + size());
	}

	impl_t m_impl;
};

template<typename T, std::size_t capacity, typename U>
constexpr auto erase(inplace_vector<T, capacity> & c, U const & value) -> typename inplace_vector<T, capacity>::size_type {
	return containers::erase_if(c, bounded::equal_to(value));
}

template<typename T, std::size_t capacity, typename Predicate>
constexpr auto erase_if(inplace_vector<T, capacity> & c, Predicate const predicate) -> typename inplace_vector<T, capacity>::size_type {
	return containers::erase_if(c, predicate);
}

} // namespace std_containers

#if 0
using namespace bounded::literal;

static_assert(containers::equal(std_containers::inplace_vector<bounded_test::integer, 1>(), containers::inplace_vector<bounded_test::integer, 1>()));
static_assert(containers::equal(std_containers::inplace_vector<bounded_test::integer, 5>{}, containers::inplace_vector<bounded_test::integer, 5>()));
static_assert(containers::equal(std_containers::inplace_vector<bounded_test::integer, 5>{1}, containers::inplace_vector<bounded_test::integer, 5>({1})));
static_assert(containers::equal(std_containers::inplace_vector<bounded_test::integer, 5>{1, 4}, containers::inplace_vector<bounded_test::integer, 5>({1, 4})));

static_assert(std_containers::inplace_vector<bounded_test::integer>() == std_containers::inplace_vector<bounded_test::integer>());
static_assert(std_containers::inplace_vector<bounded_test::integer>() == std_containers::inplace_vector<bounded_test::integer>{});
static_assert(std_containers::inplace_vector<bounded_test::integer>() != std_containers::inplace_vector<bounded_test::integer>{1});
static_assert(std_containers::inplace_vector<bounded_test::integer>{1} == std_containers::inplace_vector<bounded_test::integer>{1});

static_assert([]{
	auto v = std_containers::inplace_vector<bounded_test::integer>();
	v.assign(4, 1);
	return v == std_containers::inplace_vector<bounded_test::integer>{1, 1, 1, 1};
}());

static_assert([]{
	auto v = std_containers::inplace_vector<bounded_test::integer>();
	constexpr int input[] = {0, 3, 2, 1};
	v.assign(std::begin(input), std::end(input));
	return v == std_containers::inplace_vector<bounded_test::integer>{0, 3, 2, 1};
}());

static_assert([]{
	auto v = std_containers::inplace_vector<bounded_test::integer>();
	v.assign({1, 3, 6, 3});
	return v == std_containers::inplace_vector<bounded_test::integer>{1, 3, 6, 3};
}());

static_assert([]{
	auto v = std_containers::inplace_vector<bounded_test::integer>();
	auto const init = std_containers::inplace_vector<bounded_test::integer>();
	v.insert(v.end(), init.begin(), init.end());
	return true;
}());

static_assert([]{
	using inplace_vector = std_containers::inplace_vector<bounded_test::integer>;
	auto v = inplace_vector({1, 2});
	v.reserve(4);
	v.insert(v.begin(), v[1]);
	return v == inplace_vector({2, 1, 2});
}());

#endif