// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <version>

export module containers.dynamic_array_data;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

using namespace bounded::literal;

export template<typename T, typename Size>
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

	[[no_unique_address]] T * pointer;
	[[no_unique_address]] Size size;
};

template<typename T, typename Size>
constexpr auto allocate_at_least(Size const size) {
	#ifdef __cpp_lib_allocate_at_least
		return std::allocator<T>().allocate_at_least(static_cast<std::size_t>(size));
	#else
		struct result {
			T * ptr;
			Size count;
		};
		return result(
			std::allocator<T>().allocate(static_cast<std::size_t>(size)),
			size
		);
	#endif
} 

export template<typename T, typename Size>
constexpr auto allocate_storage(bounded::convertible_to<Size> auto const size) {
	auto const allocation = ::containers::allocate_at_least<T>(size);
	return dynamic_array_data<T, Size>(
		allocation.ptr,
		bounded::assume_in_range<Size>(bounded::min(allocation.count, numeric_traits::max_value<Size>))
	);
}

export template<typename T, typename Size>
constexpr auto deallocate_storage(dynamic_array_data<T, Size> const data) {
	std::allocator<T>().deallocate(
		data.pointer,
		static_cast<std::size_t>(data.size)
	);
}

} // namespace containers
