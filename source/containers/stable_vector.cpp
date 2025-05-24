// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.stable_vector;

import containers.bounded_vector;
import containers.maximum_array_size;

import std_module;

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
// `capacity_` cannot be `array_size_type<T>` because that would not support
// incomplete types.
export template<typename T, array_size_type<std::byte> capacity_>
struct stable_vector : private bounded_vector<T, capacity_, capacity_> {
private:
	using base = bounded_vector<T, capacity_, capacity_>;
public:
	using base::base;
	friend constexpr auto swap(stable_vector & lhs, stable_vector & rhs) noexcept -> void {
		swap(static_cast<base &>(lhs), static_cast<base &>(rhs));
	}
	using base::data;
	using base::size;
	using base::operator[];
	using base::capacity;
	using base::set_size;
	using base::operator std::span<T const>;
	using base::operator std::span<T>;
};

} // namespace containers
