// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

#include <operators/bracket.hpp>

export module containers.dynamic_array;

import containers.algorithms.copy;
import containers.algorithms.destroy_range;
import containers.algorithms.distance;
import containers.algorithms.uninitialized;
import containers.assign;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.data;
import containers.initializer_range;
import containers.is_container;
import containers.is_range;
import containers.is_iterator;
import containers.maximum_array_size;
import containers.range_value_t;
import containers.range_view;
import containers.size;
import containers.test_sequence_container;
import containers.uninitialized_dynamic_array;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

export template<typename T, typename Size = array_size_type<T>>
struct dynamic_array : private lexicographical_comparison::base {
	using size_type = Size;

	constexpr dynamic_array() = default;

	constexpr explicit dynamic_array(constructor_initializer_range<dynamic_array> auto && source):
		m_data(::bounded::assume_in_range<size_type>(::containers::linear_size(source)))
	{
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), begin());
	}

	template<std::size_t source_size>
	constexpr dynamic_array(c_array<T, source_size> && source):
		dynamic_array(range_view(std::move(source)))
	{
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr dynamic_array(Source) {
	}

	constexpr dynamic_array(dynamic_array const & other):
		dynamic_array(range_view(other))
	{
	}
	
	constexpr dynamic_array(dynamic_array && other) noexcept:
		m_data(std::exchange(other.m_data, {}))
	{
	}
	
	constexpr ~dynamic_array() {
		::containers::destroy_range(*this);
	}

	constexpr auto operator=(dynamic_array const & other) & -> dynamic_array & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}
	constexpr auto operator=(dynamic_array && other) & noexcept -> dynamic_array & {
		::containers::destroy_range(*this);
		m_data = std::move(other.m_data);
		other.m_data = {};
		return *this;
	}
	
	constexpr auto begin() const {
		return contiguous_iterator<T const, bounded::builtin_max_value<size_type>>(m_data.data());
	}
	constexpr auto begin() {
		return contiguous_iterator<T, bounded::builtin_max_value<size_type>>(m_data.data());
	}

	constexpr auto size() const {
		return m_data.capacity();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	constexpr auto clear() & -> void {
		*this = {};
	}

	template<range Range> requires(!std::is_array_v<Range> or !std::is_reference_v<Range>)
	constexpr auto assign(Range && range) & -> void {
		auto const difference = ::containers::linear_size(range);
		if (difference == size()) {
			::containers::copy(OPERATORS_FORWARD(range), begin());
		} else {
			clear();
			*this = dynamic_array<T>(OPERATORS_FORWARD(range));
		}
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	uninitialized_dynamic_array<T, size_type> m_data;
};

template<typename Range>
dynamic_array(Range &&) -> dynamic_array<std::decay_t<range_value_t<Range>>>;

template<typename T, typename Size>
constexpr auto is_container<dynamic_array<T, Size>> = true;

} // namespace containers

static_assert(containers_test::test_sequence_container<containers::dynamic_array<int>>());
static_assert(containers_test::test_sequence_container<containers::dynamic_array<bounded_test::integer>>());

static_assert(bounded::convertible_to<containers::dynamic_array<bounded_test::integer> const &, std::span<bounded_test::integer const>>);
static_assert(bounded::convertible_to<containers::dynamic_array<bounded_test::integer> &, std::span<bounded_test::integer>>);

namespace {

struct recursive {
	containers::dynamic_array<recursive, bounded::integer<0, 1>> m;
};

} // namespace