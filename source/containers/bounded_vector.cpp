// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.bounded_vector;

import containers.algorithms.destroy_range;
import containers.algorithms.uninitialized;
import containers.assign;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.get_source_size;
import containers.initializer_range;
import containers.maximum_array_size;
import containers.reserve_space_for;
import containers.size_then_use_range;
import containers.uninitialized_dynamic_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Cannot use `array_size_type<T>` because that would not support incomplete
// types.
export template<typename T, array_size_type<std::byte> min_capacity, array_size_type<std::byte> max_capacity>
struct [[clang::trivial_abi]] bounded_vector trivially_relocatable_if_eligible replaceable_if_eligible : private lexicographical_comparison::base {
	template<typename U, array_size_type<std::byte> other_min_capacity, array_size_type<std::byte> other_max_capacity>
	friend struct bounded_vector;

	using size_type = bounded::integer<0, bounded::normalize<max_capacity>>;

	template<typename Capacity>
	constexpr bounded_vector(reserve_space_for<Capacity> const capacity_):
		m_storage(capacity_.value),
		m_size(0_bi)
	{
	}

	constexpr bounded_vector() noexcept(min_capacity == 0_bi):
		m_storage(bounded::constant<min_capacity>),
		m_size(0_bi)
	{
	}

	template<constructor_initializer_range<bounded_vector> Source>
	constexpr explicit bounded_vector(Source && source):
		m_storage(bounded::constant<min_capacity>),
		m_size(0_bi)
	{
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<constructor_initializer_range<bounded_vector> Source> requires size_then_use_range<Source>
	constexpr explicit bounded_vector(Source && source):
		bounded_vector(
			OPERATORS_FORWARD(source),
			::containers::get_source_size<bounded_vector>(source)
		)
	{
	}
	
	template<std::size_t source_size> requires(source_size <= max_capacity)
	constexpr bounded_vector(c_array<T, source_size> && source):
		bounded_vector(std::move(source), bounded::constant<source_size>)
	{
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr bounded_vector(Source):
		bounded_vector()
	{
	}

	// If `other.capacity()` is outside [min_capacity, max_capacity], the
	// behavior is undefined.
	// TODO: throw exception instead
	template<array_size_type<std::byte> other_min_capacity, array_size_type<std::byte> other_max_capacity>
	constexpr explicit bounded_vector(bounded_vector<T, other_min_capacity, other_max_capacity> && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(bounded::assume_in_range<size_type>(std::exchange(other.m_size, 0_bi)))
	{
	}
	// TODO: Support trivial relocatability
	constexpr bounded_vector(bounded_vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}

	constexpr bounded_vector(bounded_vector const & other):
		m_storage(reservation_size(other.size())),
		m_size(other.size())
	{
		::containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(other), data());
	}

	constexpr ~bounded_vector() {
		::containers::destroy_range(*this);
	}

	constexpr auto operator=(bounded_vector && other) & noexcept -> bounded_vector & {
		::containers::destroy_range(*this);
		m_size = other.m_size;
		other.m_size = 0_bi;
		m_storage = std::move(other.m_storage);
		return *this;
	}
	constexpr auto operator=(bounded_vector const & other) & -> bounded_vector & {
		if (this == std::addressof(other)) {
			return *this;
		}
		if (!m_storage.data()) {
			replace_empty_allocation(other.size());
			::containers::uninitialized_copy_no_overlap(other, data());
			m_size = other.m_size;
			return *this;
		}
		containers::assign(*this, other);
		return *this;
	}

	friend constexpr auto swap(bounded_vector & lhs, bounded_vector & rhs) noexcept -> void {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	constexpr auto data() const -> T const * {
		return m_storage.data();
	}
	constexpr auto data() -> T * {
		return m_storage.data();
	}
	constexpr auto size() const -> size_type {
		return m_size;
	}
	
	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	static constexpr auto capacity() requires(min_capacity == max_capacity) {
		return bounded::constant<min_capacity>;
	}
	constexpr auto capacity() const requires(min_capacity != max_capacity) {
		return m_storage.capacity();
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto set_size(auto const new_size) -> void {
		BOUNDED_ASSERT(new_size <= capacity());
		m_size = new_size;
	}

	constexpr auto replace_empty_allocation(size_type const requested_capacity) -> void {
		BOUNDED_ASSERT(size() == 0_bi);
		m_storage.replace_allocation(reservation_size(requested_capacity));
	}
	constexpr auto reserve(size_type const requested_capacity) -> void {
		if (requested_capacity <= capacity()) {
			return;
		}
		auto temp = storage_type(reservation_size(requested_capacity));
		containers::uninitialized_relocate_no_overlap(
			*this,
			temp.data()
		);
		m_storage = std::move(temp);
		// m_size remains the same
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(data(), static_cast<std::size_t>(size()));
	}

private:
	static constexpr auto reservation_size(auto const source_size) {
		return bounded::assume_in_range<capacity_type>(bounded::max(source_size, bounded::constant<min_capacity>));
	}
	constexpr explicit bounded_vector(auto && source, auto const source_size):
		m_storage(reservation_size(source_size)),
		m_size(bounded::assume_in_range<size_type>(source_size))
	{
		::containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), data());
	}

	using capacity_type = bounded::integer<
		bounded::normalize<min_capacity>,
		bounded::normalize<max_capacity>
	>;
	using storage_type = uninitialized_dynamic_array<T, capacity_type>;
	[[no_unique_address]] storage_type m_storage;
	[[no_unique_address]] size_type m_size = 0_bi;
};

} // namespace containers
