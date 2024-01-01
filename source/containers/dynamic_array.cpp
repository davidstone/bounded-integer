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
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.c_array;
import containers.common_functions;
import containers.compare_container;
import containers.initializer_range;
import containers.is_container;
import containers.linear_size;
import containers.maximum_array_size;
import containers.range;
import containers.range_value_t;
import containers.range_view;
import containers.size_then_use_range;
import containers.uninitialized_dynamic_array;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

export template<typename T, typename Size = array_size_type<T>>
struct [[clang::trivial_abi]] dynamic_array : private lexicographical_comparison::base {
	using size_type = Size;

	constexpr dynamic_array() = default;

	template<constructor_initializer_range<dynamic_array> Source> requires size_then_use_range<Source>
	constexpr explicit dynamic_array(Source && source):
		m_data(::bounded::check_in_range<size_type>(::containers::linear_size(source)))
	{
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), ::containers::begin(*this));
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
			assign(other);
		}
		return *this;
	}
	constexpr auto operator=(dynamic_array && other) & noexcept -> dynamic_array & {
		::containers::destroy_range(*this);
		m_data = std::move(other.m_data);
		other.m_data = {};
		return *this;
	}
	
	constexpr auto data() const {
		return m_data.data();
	}
	constexpr auto data() {
		return m_data.data();
	}

	constexpr auto size() const {
		return m_data.capacity();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	constexpr auto clear() & -> void {
		*this = {};
	}

	template<size_then_use_range Range> requires(!std::is_array_v<Range> or !std::is_reference_v<Range>)
	constexpr auto assign(Range && range) & -> void {
		auto const difference = ::containers::linear_size(range);
		if (difference == size()) {
			::containers::copy(OPERATORS_FORWARD(range), ::containers::begin(*this));
		} else {
			clear();
			*this = dynamic_array<T, Size>(OPERATORS_FORWARD(range));
		}
	}

	constexpr operator std::span<T const>() const {
		return std::span<T const>(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() {
		return std::span<T>(data(), static_cast<std::size_t>(size()));
	}

private:
	uninitialized_dynamic_array<T, size_type> m_data;
};

template<typename Range>
dynamic_array(Range &&) -> dynamic_array<std::decay_t<range_value_t<Range>>>;

template<typename T, typename Size>
constexpr auto is_container<dynamic_array<T, Size>> = true;

} // namespace containers
