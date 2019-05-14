// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/array/array.hpp>
#include <containers/fixed_capacity_vector.hpp>
#include <containers/uninitialized_storage.hpp>

namespace containers {
namespace detail {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
template<typename T, std::size_t capacity_>
struct stable_vector_storage {
	static constexpr auto capacity() noexcept {
		return bounded::constant<bounded::detail::normalize<capacity_>>;
	}

	using value_type = T;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::ptrdiff_t>(capacity())>
	>;

	stable_vector_storage() = default;
	// TODO: Support trivial relocatability
	stable_vector_storage(stable_vector_storage && other) noexcept(false) {
		std::swap(storage, other.storage);
		std::swap(size, other.size);
	}
	stable_vector_storage(stable_vector_storage const & other) noexcept(false) {
		std::uninitialized_copy(begin(other), end(other), begin(*this));
		size = other.size;
	}
	~stable_vector_storage() {
		detail::destroy_range(*this);
	}

	friend auto begin(stable_vector_storage const & container) noexcept -> T const * {
		return container.storage.get();
	}
	friend auto begin(stable_vector_storage & container) noexcept -> T * {
		return container.storage.get();
	}
	friend auto end(stable_vector_storage const & container) noexcept -> T const * {
		return begin(container) + container.size;
	}
	friend auto end(stable_vector_storage & container) noexcept -> T * {
		return begin(container) + container.size;
	}

	struct deleter {
		auto operator()(T * ptr) const noexcept {
			std::allocator<T>{}.deallocate(ptr, static_cast<std::ptrdiff_t>(capacity()));
		}
	};
	using raw_storage = std::unique_ptr<T, deleter>;
	raw_storage storage{std::allocator<T>{}.allocate(static_cast<std::ptrdiff_t>(capacity()))};
	size_type size = 0_bi;
};

} // namespace detail

template<typename T, std::size_t capacity>
using stable_vector = fixed_capacity_vector<detail::stable_vector_storage<T, capacity>>;

}	// namespace containers