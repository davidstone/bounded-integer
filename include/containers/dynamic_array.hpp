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
#include <containers/is_iterator.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/size.hpp>
#include <containers/uninitialized_dynamic_array.hpp>

#include <operators/forward.hpp>

#include <operators/bracket.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T, typename Size = detail::array_size_type<T>>
struct dynamic_array : private lexicographical_comparison::base {
	using value_type = T;
	using size_type = Size;

	using const_iterator = contiguous_iterator<value_type const, bounded::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::builtin_max_value<size_type>>;
	
	constexpr dynamic_array() = default;

	template<range Range> requires(!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>)
	constexpr explicit dynamic_array(Range && init):
		m_data(size_type(::containers::detail::linear_size(init)))
	{
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(init), begin());
	}

	template<std::size_t init_size>
	constexpr dynamic_array(c_array<value_type, init_size> && init):
		dynamic_array(range_view(std::move(init)))
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

	constexpr auto & operator=(dynamic_array const & other) & {
		containers::assign(*this, other);
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		::containers::destroy_range(*this);
		m_data = std::move(other.m_data);
		other.m_data = {};
		return *this;
	}
	
	constexpr auto begin() const & {
		return const_iterator(m_data.data());
	}
	constexpr auto begin() & {
		return iterator(m_data.data());
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}

	constexpr auto size() const {
		return m_data.capacity();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	constexpr void clear() & {
		*this = {};
	}

	template<range Range> requires(!std::is_array_v<Range> or !std::is_reference_v<Range>)
	constexpr auto assign(Range && range) & {
		auto const difference = detail::linear_size(range);
		if (difference == size()) {
			::containers::copy(OPERATORS_FORWARD(range), begin());
		} else {
			clear();
			*this = dynamic_array<T>(OPERATORS_FORWARD(range));
		}
	}

private:
	uninitialized_dynamic_array<value_type, size_type> m_data;
};

} // namespace containers
