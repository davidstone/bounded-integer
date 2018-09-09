// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/type.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <memory>

namespace containers {

template<typename T>
struct allocator {
	using value_type = T;
	
	constexpr allocator() noexcept = default;
	template<typename U>
	constexpr allocator(allocator<U> const &) noexcept {}
	
	auto allocate(typename std::allocator<T>::size_type n) {
		return std::allocator<T>{}.allocate(n);
	}
	auto deallocate(typename std::allocator<T>::pointer p, typename std::allocator<T>::size_type n) BOUNDED_NOEXCEPT(
		std::allocator<T>{}.deallocate(p, n)
	)
};

// TODO: Support only strong_equality, not strong_ordering
template<typename LHS, typename RHS>
constexpr auto compare(allocator<LHS>, allocator<RHS>) noexcept {
	return bounded::strong_ordering_equal;
}


// No construct or destroy
template<typename Allocator>
struct allocator_traits : private std::allocator_traits<std::decay_t<Allocator>> {
private:
	using base = std::allocator_traits<std::decay_t<Allocator>>;
public:
	using typename base::allocator_type;
	using typename base::value_type;
	using typename base::pointer;
	using typename base::const_pointer;
	using typename base::void_pointer;
	using typename base::const_void_pointer;
	using typename base::difference_type;
	using typename base::size_type;
	using typename base::propagate_on_container_copy_assignment;
	using typename base::propagate_on_container_move_assignment;
	using typename base::propagate_on_container_swap;
	// using typename base::is_always_equal;
	
	template<typename T>
	using rebind_alloc = typename base::template rebind_alloc<T>;
	template<typename T>
	using rebind_traits = std::allocator_traits<rebind_alloc<T>>;
	
	using base::allocate;
	using base::deallocate;
	using base::max_size;
	using base::select_on_container_copy_construction;
};

}	// namespace containers
