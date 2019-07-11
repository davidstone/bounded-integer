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

	friend auto swap(stable_vector_storage & lhs, stable_vector_storage & rhs) noexcept {
		std::swap(lhs.storage, rhs.storage);
		std::swap(lhs.size, rhs.size);
	}
	// Allocates the full capacity
	stable_vector_storage():
		storage(make_storage()),
		size(0_bi)
	{
	}
	// TODO: Support trivial relocatability
	stable_vector_storage(stable_vector_storage && other) noexcept:
		storage(std::move(other.storage)),
		size(std::exchange(other.size, 0_bi))
	{
	}
	stable_vector_storage(stable_vector_storage const & other):
		stable_vector_storage()
	{
		std::uninitialized_copy(begin(other), end(other), begin(*this));
		size = other.size;
	}
	~stable_vector_storage() {
		detail::destroy_range(*this);
	}
	constexpr auto & operator=(stable_vector_storage && other) & noexcept {
		swap(*this, other);
		return *this;
	}
	auto & operator=(stable_vector_storage const & other) & noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
		assign(*this, begin(other), end(other));
		return *this;
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
	raw_storage storage;
	size_type size;

private:
	static auto make_storage() {
		return raw_storage(std::allocator<T>{}.allocate(static_cast<std::ptrdiff_t>(capacity())));
	}
};

} // namespace detail

template<typename T, std::size_t capacity>
using stable_vector = fixed_capacity_vector<detail::stable_vector_storage<T, capacity>>;

}	// namespace containers