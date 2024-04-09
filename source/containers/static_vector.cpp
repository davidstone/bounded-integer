// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.static_vector;

import containers.algorithms.copy;
import containers.algorithms.destroy_range;
import containers.algorithms.uninitialized;
import containers.array;
import containers.assign;
import containers.assign_to_empty;
import containers.assign_to_empty_into_capacity;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.data;
import containers.index_type;
import containers.initializer_range;
import containers.integer_range;
import containers.maximum_array_size;
import containers.pop_back;
import containers.range;
import containers.range_size_t;
import containers.range_value_t;
import containers.range_view;
import containers.size;
import containers.uninitialized_array;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, array_size_type<T> capacity_>
struct [[clang::trivial_abi]] static_vector : private lexicographical_comparison::base {
	static_vector() = default;

	static_vector(static_vector &&) requires(capacity_ == 0_bi or bounded::trivially_move_constructible<T>) = default;
	constexpr static_vector(static_vector && other) noexcept
		requires(capacity_ != 0_bi and !bounded::trivially_move_constructible<T> and bounded::move_constructible<T>)
	{
		containers::uninitialized_relocate_no_overlap(other, ::containers::begin(*this));
		this->m_size = std::exchange(other.m_size, 0_bi);
	}

	static_vector(static_vector const &) requires(capacity_ == 0_bi or bounded::trivially_copy_constructible<T>) = default;
	constexpr static_vector(static_vector const & other)
		requires(capacity_ != 0_bi and !bounded::trivially_copy_constructible<T> and bounded::copy_constructible<T>)
	{
		::containers::assign_to_empty_into_capacity(*this, other);
	}

	constexpr explicit static_vector(constructor_initializer_range<static_vector> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size> requires(source_size <= capacity_)
	constexpr static_vector(c_array<T, source_size> && source) {
		::containers::assign_to_empty_into_capacity(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr static_vector(Source) {
	}
	
	~static_vector() requires(capacity_ == 0_bi or bounded::trivially_destructible<T>) = default;
	constexpr ~static_vector() {
		::containers::destroy_range(*this);
	}

	auto operator=(static_vector &&) & -> static_vector & requires(capacity_ == 0_bi or bounded::trivially_move_assignable<T>) = default;
	constexpr auto operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable_v<T> and std::is_nothrow_move_constructible_v<T>)  -> static_vector &
		requires(capacity_ != 0_bi and !bounded::trivially_move_assignable<T> and bounded::move_assignable<T>)
	{
		containers::assign(*this, std::move(other));
		return *this;
	}

	auto operator=(static_vector const &) & -> static_vector & requires(capacity_ == 0_bi or bounded::trivially_copy_assignable<T>) = default;
	constexpr auto operator=(static_vector const & other) & -> static_vector &
		requires(capacity_ != 0_bi and !bounded::trivially_copy_assignable<T> and bounded::copy_assignable<T>)
	{
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}

	constexpr auto data() const -> T const * {
		return m_storage.data();
	}
	constexpr auto data() -> T * {
		return m_storage.data();
	}

	constexpr auto size() const {
		return m_size;
	}

	static constexpr auto capacity() {
		return bounded::constant<capacity_>;
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr void set_size(auto const new_size) {
		BOUNDED_ASSERT(new_size <= capacity());
		this->m_size = new_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

	friend constexpr auto swap(static_vector & lhs, static_vector & rhs) noexcept(std::is_nothrow_swappable_v<T>) -> void requires(!bounded::trivially_swappable<T>) {
		if (std::addressof(lhs) == std::addressof(rhs)) {
			return;
		}
		auto swap_impl = [](static_vector & longer, static_vector & shorter) {
			auto shorter_last = containers::end(shorter);
			auto longer_it = std::swap_ranges(containers::begin(shorter), shorter_last, containers::begin(longer));
			containers::uninitialized_relocate_no_overlap(containers::range_view(longer_it, containers::end(longer)), shorter_last);
		};
		if (containers::size(lhs) >= containers::size(rhs)) {
			swap_impl(lhs, rhs);
		} else {
			swap_impl(rhs, lhs);
		}
		std::swap(lhs.m_size, rhs.m_size);
	}

private:
	constexpr explicit static_vector(bounded::tombstone_tag, auto const make) noexcept:
		m_size(make())
	{
	}

	[[no_unique_address]] uninitialized_array<T, capacity_> m_storage = {};
	[[no_unique_address]] bounded::integer<0, bounded::normalize<capacity_>> m_size = 0_bi;

	friend bounded::tombstone_traits<static_vector<T, capacity_>>;
	friend bounded::tombstone_traits_composer<&static_vector<T, capacity_>::m_size>;
};

export template<range Source>
constexpr auto make_static_vector(Source && source) {
	constexpr auto size = bounded::integer(numeric_traits::max_value<range_size_t<Source>>);
	return static_vector<range_value_t<Source>, size>(OPERATORS_FORWARD(source));
}

export template<typename T, std::size_t size>
constexpr auto make_static_vector(c_array<T, size> && source) {
	return static_vector<T, bounded::constant<size>>(std::move(source));
}

} // namespace containers

template<typename T, containers::array_size_type<T> capacity>
struct bounded::tombstone_traits<containers::static_vector<T, capacity>> : bounded::tombstone_traits_composer<&containers::static_vector<T, capacity>::m_size> {
};
