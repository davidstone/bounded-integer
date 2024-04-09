// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.vector;

import containers.bounded_vector;
import containers.c_array;
import containers.maximum_array_size;
import containers.range_value_t;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

// `max_size` cannot be `array_size_type<T>` because that would not support
// incomplete types.
export template<typename T, array_size_type<std::byte> max_size = numeric_traits::max_value<array_size_type<T>>>
struct [[clang::trivial_abi]] vector : private bounded_vector<T, 0_bi, max_size> {
private:
	using base = bounded_vector<T, 0_bi, max_size>;
public:
	using base::base;
	friend constexpr auto swap(vector & lhs, vector & rhs) noexcept -> void {
		swap(static_cast<base &>(lhs), static_cast<base &>(rhs));
	}
	using base::data;
	using base::size;
	using base::operator[];
	using base::capacity;
	using base::set_size;
	using base::replace_empty_allocation;
	using base::reserve;
	using base::operator std::span<T const>;
	using base::operator std::span<T>;
};

template<typename Range>
vector(Range &&) -> vector<std::decay_t<range_value_t<Range>>>;

template<typename T, std::size_t size>
vector(c_array<T, size> &&) -> vector<T>;

} // namespace containers
