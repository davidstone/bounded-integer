// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.span;

import containers.begin_end;
export import containers.common_iterator_functions;
import containers.contiguous_range;
import containers.data;
import containers.index_type;
import containers.iter_reference_t;
import containers.maximum_array_size;
import containers.random_access_sentinel_for;
import containers.range_reference_t;
import containers.size;
import containers.to_address;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename U, typename T>
concept element_compatible = bounded::convertible_to<std::remove_reference_t<U>(*)[], T(*)[]>;

export template<typename T>
struct span {
	span() = default;

	template<contiguous_range Range>
		requires element_compatible<range_reference_t<Range>, T>
	constexpr span(Range && r):
		m_data(containers::data(r)),
		m_size(containers::size(r))
	{
	}

	template<to_addressable Iterator>
		requires element_compatible<iter_reference_t<Iterator>, T>
	constexpr span(Iterator const first, array_size_type<T> const size_):
		m_data(containers::to_address(first)),
		m_size(size_)
	{
	}

	template<to_addressable Iterator, random_access_sentinel_for<Iterator> Sentinel>
		requires element_compatible<iter_reference_t<Iterator>, T>
	constexpr span(Iterator const first, Sentinel const last):
		m_data(containers::to_address(first)),
		m_size(bounded::increase_min<0>(last - first))
	{
	}

	constexpr auto data() const -> T * {
		return m_data;
	}
	constexpr auto size() const -> array_size_type<T> {
		return m_size;
	}

	constexpr auto operator[](index_type<span> const index) const -> T & {
		return *(data() + index);
	}
	
	constexpr operator std::span<T>() const {
		return std::span<T>(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<T const>() const requires(!std::same_as<T, T const>) {
		return std::span<T>(data(), static_cast<std::size_t>(size()));
	}

private:
	T * m_data = nullptr;
	array_size_type<T> m_size = 0_bi;
};

template<typename Range>
span(Range &&) -> span<std::remove_reference_t<range_reference_t<Range>>>;

template<typename Iterator>
span(
	Iterator,
	auto
) -> span<std::remove_reference_t<iter_reference_t<Iterator>>>;

} // namespace containers
