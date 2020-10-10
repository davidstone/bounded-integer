// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assert.hpp>
#include <bounded/concepts.hpp>
#include <bounded/integer.hpp>

#include <memory>

namespace containers {

using namespace bounded::literal;

template<typename T, typename Capacity>
struct uninitialized_dynamic_array {
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
	constexpr uninitialized_dynamic_array(uninitialized_dynamic_array && other) noexcept:
		m_ptr(other.release()),
		m_capacity(other.m_capacity)
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
	friend constexpr void swap(uninitialized_dynamic_array & lhs, uninitialized_dynamic_array & rhs) noexcept {
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
	constexpr auto deallocate() & noexcept {
		if (m_ptr) {
			std::allocator<T>().deallocate(m_ptr, static_cast<std::size_t>(m_capacity));
		}
	}
	T * m_ptr;
	[[no_unique_address]] Capacity m_capacity;
};

} // namespace containers