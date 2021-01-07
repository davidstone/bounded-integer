// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
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
#include <containers/uninitialized_storage.hpp>

#include <operators/forward.hpp>

#include <operators/bracket.hpp>

#include <iterator>
#include <memory>

namespace containers::detail {

using namespace bounded::literal;

template<typename T>
struct dynamic_array_data {
	using size_type = bounded::integer<0, maximum_array_size<T>>;
	
	constexpr dynamic_array_data() = default;

	constexpr dynamic_array_data(T * const pointer_, auto const size_):
		pointer(pointer_),
		size(static_cast<size_type>(size_))
	{
	}

	T * pointer = nullptr;
	size_type size = 0_bi;
};


template<typename T>
constexpr auto allocate_storage(auto const size) {
	return dynamic_array_data(
		std::allocator<T>{}.allocate(static_cast<std::size_t>(size)),
		size
	);
}

template<typename T>
constexpr auto deallocate_storage(dynamic_array_data<T> const data) {
	std::allocator<T>{}.deallocate(
		data.pointer,
		static_cast<std::size_t>(data.size)
	);
}

} // namespace containers::detail
