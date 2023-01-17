// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include "../test_assert.hpp"

export module containers.uninitialized_dynamic_array;

import containers.test_sequence_container;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, typename Capacity>
struct uninitialized_dynamic_array {
	template<typename U, typename OtherCapacity>
	friend struct uninitialized_dynamic_array;

	constexpr uninitialized_dynamic_array() noexcept requires bounded::constructible_from<Capacity, bounded::constant_t<0>>:
		m_ptr(nullptr),
		m_capacity(0_bi)
	{
	}
	constexpr explicit uninitialized_dynamic_array(Capacity capacity):
		m_ptr(std::allocator<T>().allocate(static_cast<std::size_t>(capacity))),
		m_capacity(capacity)
	{
	}
	template<typename OtherCapacity>
	constexpr explicit uninitialized_dynamic_array(uninitialized_dynamic_array<T, OtherCapacity> && other) noexcept:
		m_ptr(other.release()),
		m_capacity(bounded::assume_in_range<Capacity>(other.m_capacity))
	{
	}
	constexpr uninitialized_dynamic_array(uninitialized_dynamic_array && other) noexcept:
		m_ptr(other.release()),
		m_capacity(std::exchange(other.m_capacity, {}))
	{
	}
	constexpr uninitialized_dynamic_array & operator=(uninitialized_dynamic_array && other) & noexcept {
		auto ptr = other.release();
		deallocate();
		m_ptr = ptr;
		m_capacity = other.m_capacity;
		return *this;
	}
	constexpr ~uninitialized_dynamic_array() noexcept {
		deallocate();
	}
	friend constexpr auto swap(uninitialized_dynamic_array & lhs, uninitialized_dynamic_array & rhs) noexcept -> void {
		std::swap(lhs.m_ptr, rhs.m_ptr);
		std::swap(lhs.m_capacity, rhs.m_capacity);
	}

	constexpr auto data() const noexcept -> T const * {
		return m_ptr;
	}
	constexpr auto data() noexcept -> T * {
		return m_ptr;
	}
	constexpr auto capacity() const noexcept {
		BOUNDED_ASSERT(m_ptr != nullptr or m_capacity == 0_bi);
		return m_capacity;
	}

private:
	constexpr auto release() & noexcept {
		return std::exchange(m_ptr, nullptr);
	}
	constexpr auto deallocate() & noexcept -> void {
		if (m_ptr) {
			std::allocator<T>().deallocate(m_ptr, static_cast<std::size_t>(m_capacity));
		}
	}
	[[no_unique_address]] T * m_ptr;
	[[no_unique_address]] Capacity m_capacity;
};

} // namespace containers

template<int max>
using container = containers::uninitialized_dynamic_array<bounded_test::integer, bounded::integer<0, max>>;

static_assert(container<0>().capacity() == 0_bi);
static_assert(container<1>().capacity() == 0_bi);

constexpr auto basic_test() -> bool {
	auto a = container<10>(1_bi);
	auto b = container<10>(1_bi);
	BOUNDED_TEST(a.data() != nullptr);
	BOUNDED_TEST(b.data() != nullptr);
	BOUNDED_TEST(a.data() != b.data());
	BOUNDED_TEST(a.capacity() == 1_bi);
	BOUNDED_TEST(b.capacity() == 1_bi);
	return true;
}
static_assert(basic_test());

constexpr auto move_construction() -> bool {
	auto a = container<1>(1_bi);
	auto const original_pointer = a.data();
	auto b = std::move(a);
	BOUNDED_TEST(a.data() == nullptr);
	BOUNDED_TEST(b.data() == original_pointer);
	BOUNDED_TEST(b.capacity() == 1_bi);
	return true;
}
static_assert(move_construction());

constexpr auto check_capacity(auto const value) {
	return [=](auto const & container) {
		return container.capacity() == value;
	};
}


static_assert(containers_test::test_self_move_assignment<container<10>>(
	[] { return 0_bi; },
	check_capacity(0_bi)
));
static_assert(containers_test::test_self_move_assignment<container<10>>(
	[] { return 1_bi; },
	check_capacity(1_bi)
));
static_assert(containers_test::test_self_move_assignment<container<0>>(
	[] { return 0_bi; },
	check_capacity(0_bi)
));

static_assert(containers_test::test_self_swap(container<10>(0_bi), check_capacity(0_bi)));
static_assert(containers_test::test_self_swap(container<10>(1_bi), check_capacity(1_bi)));
static_assert(containers_test::test_self_swap(container<0>(0_bi), check_capacity(0_bi)));
