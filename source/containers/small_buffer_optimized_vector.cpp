// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <bounded/conditional.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.small_buffer_optimized_vector;

import containers.algorithms.copy;
import containers.algorithms.destroy_range;
import containers.algorithms.uninitialized;
import containers.array;
import containers.assign;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.data;
import containers.dynamic_array_data;
import containers.initializer_range;
import containers.maximum_array_size;
import containers.range_view;
import containers.size;
import containers.test_sequence_container;
import containers.test_set_size;
import containers.uninitialized_array;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

// Explanation of SSO (small / short string optimization)
// https://stackoverflow.com/a/10319672/852254

using last_size_byte_t = unsigned char;

// If size remains below requested_small_capacity, there will be no allocations.
// requested_small_capacity is a guaranteed minimum. The implementation may
// reserve more space if it is less than minimum_small_capacity.
// minimum_small_capacity is equal to the minimum amount of overhead required to
// support the small buffer optimization, so there is no point going below it.
//
// Therefore, to request the smallest possible buffer, the user can request a
// size of 0.
template<typename T, typename Size>
constexpr auto minimum_small_capacity = (bounded::size_of<std::pair<Size, dynamic_array_data<T, Size>>> - bounded::size_of<last_size_byte_t>) / bounded::size_of<T>;

struct last_byte_t {
	last_size_byte_t size : (bounded::char_bit - 1_bi).value();
	bool is_large : 1;
};

template<typename Size>
using size_and_bit = decltype((bounded::declval<Size>() << 1_bi) + bounded::declval<bounded::integer<0, 1>>());

template<typename Size>
struct split_size_t {
	using first_t = containers::array<last_size_byte_t, bounded::size_of<size_and_bit<Size>> - 1_bi>;
	[[no_unique_address]] first_t first;
	[[no_unique_address]] last_size_byte_t last;
};

template<typename Size>
constexpr auto split_size_bytes(auto const size) {
	return std::bit_cast<split_size_t<Size>>(size_and_bit<Size>(size));
}

template<typename Size, typename Split = split_size_t<Size>>
constexpr auto recombine_size(typename Split::first_t const first_bytes, last_size_byte_t const last_byte) {
	return bounded::assume_in_range<Size>(std::bit_cast<size_and_bit<Size>>(Split(first_bytes, last_byte)));
}

template<typename T>
constexpr auto set_size_impl(T & storage, last_byte_t & last_byte, auto const new_size) {
	auto const split_size = ::containers::split_size_bytes<typename T::size_type>(bounded::assume_in_range<typename T::size_type>(new_size));
	storage.set_size(split_size.first);
	last_byte.size = split_size.last;
}

template<std::size_t max_size>
using small_buffer_size_type = bounded::integer<0, bounded::normalize<max_size>>;

template<typename T, std::size_t requested_small_capacity, std::size_t max_size>
struct small_type {
	static constexpr auto capacity() {
		return bounded::max(
			minimum_small_capacity<T, small_buffer_size_type<max_size>>,
			bounded::constant<requested_small_capacity>
		);
	}

	using size_type = bounded::integer<0, bounded::normalize<capacity()>>;
	using first_bytes_of_size_type = typename split_size_t<size_type>::first_t;

	constexpr auto size(last_size_byte_t const last) const -> size_type {
		return recombine_size<size_type>(m_first_bytes_of_size, last);
	}
	constexpr auto set_size(first_bytes_of_size_type const size) & -> void {
		m_first_bytes_of_size = size;
	}

	constexpr auto data() const -> T const * {
		return m_storage.data();
	}
	constexpr auto data() -> T * {
		return m_storage.data();
	}

private:
	[[no_unique_address]] uninitialized_array<T, capacity()> m_storage;
	[[no_unique_address]] first_bytes_of_size_type m_first_bytes_of_size;
};

template<typename T, std::size_t requested_small_capacity, std::size_t max_size>
struct [[gnu::packed]] large_type {
	using capacity_type = bounded::integer<
		bounded::normalize<small_type<T, requested_small_capacity, max_size>::capacity() + 1_bi>,
		bounded::normalize<max_size>
	>;
	using size_type = small_buffer_size_type<max_size>;
	using first_bytes_of_size_type = split_size_t<size_type>::first_t;

	constexpr large_type(dynamic_array_data<T, capacity_type> storage, first_bytes_of_size_type const first_bytes_of_size):
		m_pointer(storage.pointer),
		m_capacity(storage.size.value()),
		m_first_bytes_of_size(first_bytes_of_size)
	{
		BOUNDED_ASSERT_OR_ASSUME(m_pointer != nullptr);
	}

	constexpr auto capacity() const -> capacity_type {
		return bounded::assume_in_range<capacity_type>(m_capacity);
	}

	constexpr auto size(last_size_byte_t const last_bytes_of_size) const -> size_type {
		return containers::recombine_size<size_type>(m_first_bytes_of_size, last_bytes_of_size);
	}
	constexpr auto set_size(first_bytes_of_size_type const size) & -> void {
		m_first_bytes_of_size = size;
	}

	constexpr auto data() const -> T const * {
		return m_pointer;
	}
	constexpr auto data() -> T * {
		return m_pointer;
	}

private:
	// gnu::packed requires POD types
	[[no_unique_address]] T * m_pointer;
	[[no_unique_address]] typename capacity_type::underlying_type m_capacity;
	[[no_unique_address]] first_bytes_of_size_type m_first_bytes_of_size;
};

export template<typename T, std::size_t requested_small_capacity, std::size_t max_size = containers::maximum_array_size<T>>
struct [[clang::trivial_abi]] small_buffer_optimized_vector : private lexicographical_comparison::base {
	using size_type = small_buffer_size_type<max_size>;

	static_assert(
		numeric_traits::max_value<size_type> <= ((1_bi << (bounded::size_of_bits<T *> - 1_bi)) - 1_bi),
		"Maximum possible size is too large -- would use bit reserved for small-buffer optimization."
	);

private:
	using small_t = small_type<T, requested_small_capacity, max_size>;
	using large_t = large_type<T, requested_small_capacity, max_size>;
	
public:

	small_buffer_optimized_vector() = default;
	
	constexpr explicit small_buffer_optimized_vector(constructor_initializer_range<small_buffer_optimized_vector> auto && source):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr small_buffer_optimized_vector(c_array<T, source_size> && source):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr small_buffer_optimized_vector(Source):
		small_buffer_optimized_vector()
	{
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector const & other):
		small_buffer_optimized_vector()
	{
		::containers::assign_to_empty(*this, other);
	}

	constexpr small_buffer_optimized_vector(small_buffer_optimized_vector && other) noexcept:
		small_buffer_optimized_vector()
	{
		move_assign_to_empty(std::move(other));
	}

	constexpr auto operator=(small_buffer_optimized_vector const & other) & -> small_buffer_optimized_vector & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}
	constexpr auto operator=(small_buffer_optimized_vector && other) & noexcept -> small_buffer_optimized_vector & {
		// Work around https://github.com/llvm/llvm-project/issues/61562
		if (std::addressof(other) == this) {
			return *this;
		}
		destroy();
		move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr ~small_buffer_optimized_vector() {
		destroy();
	}

	constexpr auto begin() const {
		auto const result = is_small() ? m_state.small.data() : m_state.large.data();
		return contiguous_iterator<T const, bounded::builtin_max_value<size_type>>(result);
	}
	constexpr auto begin() {
		auto const result = is_small() ? m_state.small.data() : m_state.large.data();
		return contiguous_iterator<T, bounded::builtin_max_value<size_type>>(result);
	}
	
	constexpr auto size() const {
		return BOUNDED_CONDITIONAL(
			is_small(),
			m_state.small.size(m_state.last_byte.size),
			m_state.large.size(m_state.last_byte.size)
		);
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto capacity() const {
		return BOUNDED_CONDITIONAL(is_small(), m_state.small.capacity(), m_state.large.capacity());
	}
	// Assumes that elements are already constructed in the spare capacity
	constexpr auto set_size(auto const new_size) -> void {
		if (is_small()) {
			if constexpr (bounded::constructible_from<typename small_t::size_type, decltype(new_size), bounded::unchecked_t>) {
				set_size_impl(m_state.small, m_state.last_byte, new_size);
			} else {
				std::unreachable();
			}
		} else {
			set_size_impl(m_state.large, m_state.last_byte, new_size);
		}
		BOUNDED_ASSERT(size() == new_size);
	}

	constexpr auto reserve(size_type const requested_capacity) -> void {
		if (requested_capacity <= capacity()) {
			return;
		}
		if (requested_capacity <= small_t::capacity()) {
			relocate_to_small();
		} else {
			relocate_to_large(bounded::assume_in_range<typename large_t::capacity_type>(requested_capacity));
		}
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	constexpr auto destroy() noexcept {
		::containers::destroy_range(*this);
		deallocate_large();
	}

	constexpr auto relocate_to_small() {
		if (is_small()) {
			return;
		}
		auto temp = bounded::relocate(m_state.large);
		::bounded::construct_at(m_state.small, bounded::construct<small_t>);
		containers::uninitialized_relocate_no_overlap(range_view(temp.data(), temp.size(m_state.last_byte.size)), m_state.small.data());
		deallocate_storage(dynamic_array_data<T, size_type>(temp.data(), temp.capacity()));
		set_size_impl(m_state.small, m_state.last_byte, temp.size(m_state.last_byte.size));
		m_state.last_byte.is_large = false;
	}
	
	constexpr auto is_large() const {
		return m_state.last_byte.is_large;
	}
	constexpr auto is_small() const {
		return !is_large();
	}
	
	constexpr auto deallocate_large() {
		if (is_large()) {
			deallocate_storage(dynamic_array_data<T, size_type>(m_state.large.data(), m_state.large.capacity()));
		}
	}
	
	constexpr auto move_assign_to_empty(small_buffer_optimized_vector && other) & {
		if (other.is_small()) {
			::bounded::construct_at(m_state.small, [&] { return bounded::relocate(other.m_state.small); });
		} else {
			::bounded::construct_at(m_state.large, [&] { return bounded::relocate(other.m_state.large); });
		}
		::bounded::construct_at(other.m_state.small, bounded::construct<small_t>);
		m_state.last_byte = std::exchange(other.m_state.last_byte, {});
	}

	constexpr auto relocate_to_large(typename large_t::capacity_type const requested_capacity) {
		auto temp = allocate_storage<T, typename large_t::capacity_type>(requested_capacity);
		auto const split_size = ::containers::split_size_bytes<size_type>(size());
		containers::uninitialized_relocate_no_overlap(*this, temp.pointer);
		deallocate_large();
		::bounded::construct_at(
			m_state.large,
			[&] { return large_t(temp, split_size.first); }
		);
		m_state.last_byte.size = std::bit_cast<last_size_byte_t>(split_size.last);
		m_state.last_byte.is_large = true;
	}

	struct state {
		constexpr state():
			small(),
			last_byte()
		{
		}
		union {
			[[no_unique_address]] small_t small;
			[[no_unique_address]] large_t large;
		};
		last_byte_t last_byte;
	};
	alignas(T *) state m_state;
};

} // namespace containers

using small_char_type = containers::small_type<char, 0, containers::maximum_array_size<char>>;
static_assert(small_char_type::capacity() == 23);
static_assert(std::is_empty_v<small_char_type::first_bytes_of_size_type>);
static_assert(sizeof(small_char_type) == sizeof(void *) * 3 - 1);

static_assert(sizeof(containers::small_buffer_optimized_vector<char, 0>) == sizeof(void *) * 3);

struct check_alignment_first {
	containers::small_buffer_optimized_vector<char, 0> a;
	char b;
};
static_assert(sizeof(check_alignment_first) == sizeof(void *) * 4);

struct check_alignment_last {
	containers::small_buffer_optimized_vector<char, 0> a;
	char b;
};
static_assert(sizeof(check_alignment_last) == sizeof(void *) * 4);

static_assert(containers_test::test_sequence_container<containers::small_buffer_optimized_vector<int, 40>>());
static_assert(containers_test::test_set_size<containers::small_buffer_optimized_vector<int, 1>>());
