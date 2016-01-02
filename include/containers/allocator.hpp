// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/type_list.hpp>

#include <bounded/integer.hpp>

#include <memory>

namespace containers {

template<typename T>
struct allocator {
	using value_type = T;
	
	constexpr allocator() noexcept = default;
	template<typename U>
	constexpr allocator(allocator<U> const &) noexcept {}
	
	auto allocate(typename std::allocator<T>::size_type n, typename std::allocator<void>::const_pointer hint = nullptr) {
		return std::allocator<T>{}.allocate(n, hint);
	}
	auto deallocate(typename std::allocator<T>::pointer p, typename std::allocator<T>::size_type n) BOUNDED_NOEXCEPT(
		std::allocator<T>{}.deallocate(p, n)
	)
};

template<typename LHS, typename RHS>
constexpr auto operator==(allocator<LHS>, allocator<RHS>) noexcept {
	return true;
}


namespace detail {

template<typename A, typename T, typename Args, typename Enable = void>
struct allocator_has_construct_c : std::false_type {};

template<typename A, typename T, typename... Args>
struct allocator_has_construct_c<
	A,
	T,
	type_list<Args...>,
	void_t<decltype(std::declval<A &>().construct(std::declval<T *>(), std::declval<Args>()...))>
> : std::true_type {};

template<typename A, typename T, typename... Args>
constexpr auto allocator_has_construct = allocator_has_construct_c<A, T, type_list<Args...>>::value;


template<typename A, typename T, typename Enable = void>
struct allocator_has_destroy_c : std::false_type {};

template<typename A, typename T>
struct allocator_has_destroy_c<
	A,
	T,
	void_t<decltype(std::declval<A &>().destroy(std::declval<T *>()))>
> : std::true_type {};

template<typename A, typename T>
constexpr auto allocator_has_destroy = allocator_has_destroy_c<A, T>::value;


}	// namespace detail

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
	

	template<typename T, typename... Args, BOUNDED_REQUIRES(detail::allocator_has_construct<allocator_type, T, Args...>)>
	static constexpr auto construct(allocator_type & a, T * const ptr, Args && ... args) BOUNDED_NOEXCEPT(
		static_cast<void>(a.construct(ptr, std::forward<Args>(args)...))
	)
	template<typename T, typename... Args, BOUNDED_REQUIRES(!detail::allocator_has_construct<allocator_type, T, Args...>)>
	static constexpr auto construct(allocator_type &, T * const ptr, Args && ... args) BOUNDED_NOEXCEPT(
		::bounded::construct(*ptr, std::forward<Args>(args)...)
	)
	

	template<typename T, BOUNDED_REQUIRES(detail::allocator_has_destroy<allocator_type, T>)>
	static constexpr auto destroy(allocator_type & a, T * const ptr) BOUNDED_NOEXCEPT(
		static_cast<void>(a.destroy(ptr))
	)
	template<typename T, BOUNDED_REQUIRES(!detail::allocator_has_destroy<allocator_type, T>)>
	static constexpr auto destroy(allocator_type &, T * const ptr) BOUNDED_NOEXCEPT(
		::bounded::destroy(*ptr)
	)
};

}	// namespace containers
