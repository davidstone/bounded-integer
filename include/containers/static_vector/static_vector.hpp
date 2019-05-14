// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/array.hpp>
#include <containers/fixed_capacity_vector.hpp>
#include <containers/uninitialized_storage.hpp>

namespace containers {
namespace detail {

// literal types cannot have destructors. If value_type is trivially
// destructible, we can skip calling their destructors, which could allow
// static_vector<value_type, n> to be a literal type. This should also improve
// performance.
//
// TODO: Theoretically, this shouldn't group different types of triviality, but
// without requires clauses it is too annoying to write out.

template<
	typename T,
	std::size_t capacity_,
	bool = std::is_trivially_destructible_v<T> and std::is_trivially_copyable_v<T>
>
struct static_vector_storage {
	static constexpr auto capacity() noexcept {
		return bounded::constant<bounded::detail::normalize<capacity_>>;
	}
	using value_type = T;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::ptrdiff_t>(capacity())>
	>;

	friend constexpr auto begin(static_vector_storage const & container) noexcept {
		return detail::static_or_reinterpret_cast<value_type const *>(data(container.storage));
	}
	friend constexpr auto begin(static_vector_storage & container) noexcept {
		return detail::static_or_reinterpret_cast<value_type *>(data(container.storage));
	}

	friend constexpr auto end(static_vector_storage const & container) noexcept {
		return begin(container) + container.size;
	}
	friend constexpr auto end(static_vector_storage & container) noexcept {
		return begin(container) + container.size;
	}

	array<trivial_storage<T>, capacity_> storage = {};
	size_type size = 0_bi;
};

template<typename T, std::size_t capacity_>
struct static_vector_storage<T, capacity_, false> : static_vector_storage<T, capacity_, true> {
	using value_type = T;

	static_vector_storage() = default;
	static_vector_storage(static_vector_storage && other) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
		std::uninitialized_move(begin(other), end(other), begin(*this));
		this->size = other.size;
	}
	static_vector_storage(static_vector_storage const & other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) {
		std::uninitialized_copy(begin(other), end(other), begin(*this));
		this->size = other.size;
	}
	~static_vector_storage() {
		detail::destroy_range(*this);
	}
};

} // namespace detail

template<typename T, std::size_t capacity>
using static_vector = fixed_capacity_vector<detail::static_vector_storage<T, capacity>>;

}	// namespace containers
