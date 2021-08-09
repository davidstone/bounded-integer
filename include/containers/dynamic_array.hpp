// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/assign.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/initializer_range.hpp>
#include <containers/is_iterator.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/size.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/forward.hpp>

#include <operators/bracket.hpp>

#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T, typename Size = detail::array_size_type<T>>
struct dynamic_array : private lexicographical_comparison::base {
	using size_type = Size;

	constexpr dynamic_array() = default;

	constexpr explicit dynamic_array(initializer_range<dynamic_array> auto && source):
		m_data(size_type(::containers::detail::linear_size(source)))
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
		if (std::addressof(other) == this) {
			return *this;
		}
		containers::assign(*this, other);
		return *this;
	}
	constexpr auto operator=(dynamic_array && other) & noexcept -> dynamic_array & {
		::containers::destroy_range(*this);
		m_data = std::move(other.m_data);
		other.m_data = {};
		return *this;
	}
	
	constexpr auto begin() const & {
		return contiguous_iterator<T const, bounded::builtin_max_value<size_type>>(m_data.data());
	}
	constexpr auto begin() & {
		return contiguous_iterator<T, bounded::builtin_max_value<size_type>>(m_data.data());
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
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
		auto const difference = detail::linear_size(range);
		if (difference == size()) {
			::containers::copy(OPERATORS_FORWARD(range), begin());
		} else {
			clear();
			*this = dynamic_array<T>(OPERATORS_FORWARD(range));
		}
	}

	constexpr operator std::span<T const>() const & {
		return std::span<T const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T>() & {
		return std::span<T>(containers::data(*this), static_cast<std::size_t>(size()));
	}

private:
	uninitialized_dynamic_array<T, size_type> m_data;
};

} // namespace containers
