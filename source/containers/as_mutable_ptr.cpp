// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.as_mutable_ptr;

namespace containers {

export template<typename T>
constexpr auto as_mutable_ptr(T const * value) -> T * {
	return const_cast<T *>(value);
}

} // namespace containers
