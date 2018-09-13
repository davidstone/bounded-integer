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
	using difference_type = bounded::integer<-max_difference, max_difference>;
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

template<typename T, std::ptrdiff_t max_difference>
constexpr auto compare(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<T, max_difference> const rhs) noexcept {
	return bounded::compare(pointer_from(lhs), pointer_from(rhs));
}

template<typename T, std::ptrdiff_t max_difference>
constexpr auto compare(contiguous_iterator<T const, max_difference> const lhs, contiguous_iterator<T, max_difference> const rhs) noexcept {
	return compare(lhs, static_cast<decltype(lhs)>(rhs));
}

template<typename T, std::ptrdiff_t max_difference>
constexpr auto compare(contiguous_iterator<T, max_difference> const lhs, contiguous_iterator<T const, max_difference> const rhs) noexcept {
	return compare(static_cast<decltype(rhs)>(lhs), rhs);
}


template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator+(
	contiguous_iterator<T, max_difference> const lhs,
	typename contiguous_iterator<T, max_difference>::difference_type const rhs
) noexcept {
	return contiguous_iterator<T, max_difference>(pointer_from(lhs) + rhs);
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
