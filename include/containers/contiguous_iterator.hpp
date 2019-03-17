// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>

#include <bounded/integer.hpp>

#include <cstddef>

namespace containers {

template<typename T, std::ptrdiff_t max_difference>
struct contiguous_iterator {
	using value_type = T;
	using difference_type = bounded::integer<bounded::detail::normalize<-max_difference>, bounded::detail::normalize<max_difference>>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	contiguous_iterator() = default;
	constexpr explicit contiguous_iterator(T * ptr) noexcept:
		m_ptr(ptr)
	{
	}

	// Convert iterator to const_iterator
	constexpr operator contiguous_iterator<T const, max_difference>() const noexcept {
		return contiguous_iterator<T const, max_difference>(m_ptr);
	}

	constexpr auto & operator*() const noexcept {
		return *m_ptr;
	}
	constexpr auto operator->() const noexcept {
		return std::addressof(operator*());
	}

	constexpr auto & operator[](index_type<contiguous_iterator> const index) const noexcept {
		return *(*this + index);
	}

	friend constexpr auto pointer_from(contiguous_iterator const it) noexcept {
		return it.m_ptr;
	}
private:
	T * m_ptr = nullptr;
};

template<typename T, typename U, std::ptrdiff_t max_difference, BOUNDED_REQUIRES(
	std::is_same_v<std::remove_const_t<T>, std::remove_const_t<U>>
)>
constexpr auto compare(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<U, max_difference> const rhs) noexcept {
	return pointer_from(lhs) <=> pointer_from(rhs);
}

template<typename T, typename U, std::ptrdiff_t max_difference, BOUNDED_REQUIRES(
	std::is_same_v<std::remove_const_t<T>, std::remove_const_t<U>>
)>
constexpr auto operator==(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<U, max_difference> const rhs) noexcept {
	return pointer_from(lhs) == pointer_from(rhs);
}


template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator+(
	contiguous_iterator<T, max_difference> const lhs,
	typename contiguous_iterator<T, max_difference>::difference_type const rhs
) noexcept {
	return contiguous_iterator<T, max_difference>(pointer_from(lhs) + rhs);
}

// Iterators over ranges of zero size tend to come up only in generic code, and
// it is annoying to be unable to do things like use a range-based for loop over
// an empty array
template<typename T>
constexpr auto operator+(
	contiguous_iterator<T, 0> const lhs,
	bounded::constant_t<1>
) noexcept {
	assert(false);
	// Not sure what the best behavior is here. I guess a likely infinite loop
	// (by returning the original value) is better than invalid memory access?
	return lhs;
}


template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator-(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<T, max_difference> const rhs) noexcept {
	return static_cast<typename contiguous_iterator<T, max_difference>::difference_type>(pointer_from(lhs) - pointer_from(rhs));
}

template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator-(contiguous_iterator<T const, max_difference> const lhs, contiguous_iterator<T, max_difference> const rhs) noexcept {
	return lhs - static_cast<decltype(lhs)>(rhs);
}

template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator-(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<T const, max_difference> const rhs) noexcept {
	return static_cast<decltype(rhs)>(lhs) - rhs;
}

}	// namespace containers
