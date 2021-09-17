// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/c_array.hpp>

#include <memory>
#include <type_traits>

namespace containers {

template<typename T, std::size_t capacity>
struct uninitialized_array {
	constexpr uninitialized_array() = default;
	constexpr auto data() const noexcept -> T const * {
		return data_impl<T const *>(m_storage);
	}
	constexpr auto data() noexcept -> T * {
		return data_impl<T *>(m_storage);
	}
private:
	// reinterpret_cast is not allowed in a constexpr function, so this avoids
	// storage that requires a reinterpret_cast if possible.
	//
	// Once https://bugs.llvm.org/show_bug.cgi?id=51788 is fixed, this can use a
	// union-based implementation to drop the trivially destructible requirement
	static constexpr auto is_sufficiently_trivial = std::is_trivially_default_constructible_v<T> and std::is_trivially_destructible_v<T>;

	template<typename Result>
	static constexpr auto data_impl(auto & storage) -> Result {
		if constexpr (capacity == 0) {
			return nullptr;
		} else if constexpr (is_sufficiently_trivial) {
			return static_cast<Result>(storage);
		} else {
			return reinterpret_cast<Result>(std::addressof(storage));
		}
	}

	using storage_type = std::conditional_t<
		is_sufficiently_trivial,
		possibly_empty_c_array<T, capacity>,
		std::aligned_storage_t<sizeof(T) * capacity, alignof(T)>
	>;
	[[no_unique_address]] storage_type m_storage;
};

}	// namespace containers
