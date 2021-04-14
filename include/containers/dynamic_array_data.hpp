// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/maximum_array_size.hpp>

#include <memory>

namespace containers::detail {

using namespace bounded::literal;

template<typename T, typename Size>
struct dynamic_array_data {
	constexpr dynamic_array_data():
		pointer(nullptr),
		size(0_bi)
	{
	}

	// Do not allow CTAD to deduce the size
	constexpr dynamic_array_data(T * const pointer_, bounded::convertible_to<Size> auto const size_):
		pointer(pointer_),
		size(size_)
	{
	}

	T * pointer;
	[[no_unique_address]] Size size;
};


template<typename T, typename Size>
constexpr auto allocate_storage(auto const size) {
	return dynamic_array_data<T, Size>(
		std::allocator<T>{}.allocate(static_cast<std::size_t>(size)),
		size
	);
}

template<typename T, typename Size>
constexpr auto deallocate_storage(dynamic_array_data<T, Size> const data) {
	std::allocator<T>{}.deallocate(
		data.pointer,
		static_cast<std::size_t>(data.size)
	);
}

} // namespace containers::detail