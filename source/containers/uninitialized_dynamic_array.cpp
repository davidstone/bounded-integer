// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.uninitialized_dynamic_array;

import containers.dynamic_array_data;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, typename Capacity>
struct [[clang::trivial_abi]] uninitialized_dynamic_array {
	template<typename U, typename OtherCapacity>
	friend struct uninitialized_dynamic_array;

	constexpr uninitialized_dynamic_array() noexcept requires bounded::constructible_from<Capacity, bounded::constant_t<0>> = default;
	constexpr uninitialized_dynamic_array(bounded::constant_t<0>) noexcept requires bounded::constructible_from<Capacity, bounded::constant_t<0>> {
	}
	constexpr explicit uninitialized_dynamic_array(Capacity capacity):
		m_storage(allocate(capacity))
	{
	}
	template<typename OtherCapacity>
	constexpr explicit uninitialized_dynamic_array(uninitialized_dynamic_array<T, OtherCapacity> && other) noexcept:
		m_storage(
			other.m_storage.pointer,
			bounded::assume_in_range<Capacity>(other.m_storage.size)
		)
	{
		other.m_storage.pointer = nullptr;
	}
	constexpr uninitialized_dynamic_array(uninitialized_dynamic_array && other) noexcept:
		m_storage(other.m_storage)
	{
		other.m_storage.pointer = nullptr;
		other.m_storage.size = {};
	}
	constexpr auto operator=(uninitialized_dynamic_array && other) & noexcept -> uninitialized_dynamic_array & {
		auto const original_storage = m_storage;
		m_storage = other.m_storage;
		other.m_storage.pointer = nullptr;
		deallocate(original_storage);
		return *this;
	}
	constexpr ~uninitialized_dynamic_array() noexcept {
		deallocate(m_storage);
	}
	friend constexpr auto swap(uninitialized_dynamic_array & lhs, uninitialized_dynamic_array & rhs) noexcept -> void {
		std::swap(lhs.m_storage, rhs.m_storage);
	}

	constexpr auto data() const noexcept -> T const * {
		return m_storage.pointer;
	}
	constexpr auto data() noexcept -> T * {
		return m_storage.pointer;
	}
	constexpr auto capacity() const noexcept -> Capacity {
		BOUNDED_ASSERT(m_storage.pointer != nullptr or m_storage.size == 0_bi);
		return m_storage.size;
	}

	constexpr auto replace_allocation(Capacity new_capacity) -> void {
		deallocate(m_storage);
		m_storage = allocate(new_capacity);
	}

private:
	using storage_t = dynamic_array_data<T, Capacity>;
	static constexpr auto allocate(Capacity const capacity) -> storage_t {
		return ::containers::allocate_storage<T, Capacity>(capacity);
	}
	static constexpr auto deallocate(storage_t const storage) noexcept -> void {
		if (storage.pointer) {
			::containers::deallocate_storage(storage);
		}
	}
	storage_t m_storage;
};

} // namespace containers
