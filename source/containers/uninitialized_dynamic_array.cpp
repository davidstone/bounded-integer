// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.uninitialized_dynamic_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, typename Capacity>
struct [[clang::trivial_abi]] uninitialized_dynamic_array {
	template<typename U, typename OtherCapacity>
	friend struct uninitialized_dynamic_array;

	constexpr uninitialized_dynamic_array() noexcept requires bounded::constructible_from<Capacity, bounded::constant_t<0>>:
		m_ptr(nullptr),
		m_capacity(0_bi)
	{
	}
	constexpr explicit uninitialized_dynamic_array(Capacity capacity):
		m_ptr(allocate(capacity)),
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
		auto const original_ptr = release();
		m_ptr = other.release();
		auto const original_capacity = std::exchange(m_capacity, other.m_capacity);
		deallocate(original_ptr, original_capacity);
		return *this;
	}
	constexpr ~uninitialized_dynamic_array() noexcept {
		deallocate(m_ptr, m_capacity);
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

	constexpr auto replace_allocation(Capacity new_capacity) -> void {
		deallocate(m_ptr, m_capacity);
		m_ptr = allocate(new_capacity);
		m_capacity = new_capacity;
	}

private:
	constexpr auto release() & noexcept {
		return std::exchange(m_ptr, nullptr);
	}
	static constexpr auto allocate(Capacity const capacity) -> T * {
		return std::allocator<T>().allocate(static_cast<std::size_t>(capacity));
	}
	static constexpr auto deallocate(T * const ptr, Capacity const capacity) noexcept -> void {
		if (ptr) {
			std::allocator<T>().deallocate(ptr, static_cast<std::size_t>(capacity));
		}
	}
	[[no_unique_address]] T * m_ptr;
	[[no_unique_address]] Capacity m_capacity;
};

} // namespace containers
