// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/common_iterator_functions.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {
namespace detail {

// Special tag type to make this iterator not constructible from a pointer
constexpr struct iterator_constructor_t{} iterator_constructor{};

// It looks like we do not need to pass in type T here, as it is
// Container::value_type, but for const_iterator, it is actually
// Container::value_type const. An alternative implementation would have two
// template parameters: typename Container, bool is_const
template<typename T, typename Container>
struct basic_array_iterator {
	using value_type = T;
	using difference_type = basic_difference_type<typename Container::size_type>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr basic_array_iterator() noexcept = default;

	template<typename U>
	constexpr explicit basic_array_iterator(U * const other, iterator_constructor_t) noexcept:
		m_it(::containers::detail::static_or_reinterpret_cast<pointer>(other)) {
	}

	// Convert iterator to const_iterator
	constexpr operator basic_array_iterator<value_type const, Container>() const noexcept {
		return basic_array_iterator<value_type const, Container>(m_it, iterator_constructor);
	}

	constexpr auto & operator*() const noexcept {
		return *m_it;
	}
	constexpr auto operator->() const noexcept {
		return std::addressof(operator*());
	}

	constexpr auto & operator[](index_type<basic_array_iterator> const index) const noexcept {
		return *(*this + index);
	}

	friend constexpr auto pointer_from(basic_array_iterator const it) noexcept {
		return it.m_it;
	}

	friend constexpr auto operator+(basic_array_iterator const lhs, difference_type const rhs) noexcept {
		return basic_array_iterator(lhs.m_it + rhs, iterator_constructor);
	}

private:
	pointer m_it = nullptr;
};

template<typename T, typename Container>
constexpr auto compare(basic_array_iterator<T, Container> const lhs, basic_array_iterator<T, Container> const rhs) noexcept {
	return bounded::compare(pointer_from(lhs), pointer_from(rhs));
}
template<typename T, typename Container>
constexpr auto compare(basic_array_iterator<T const, Container> const lhs, basic_array_iterator<T, Container> const rhs) noexcept {
	return compare(lhs, static_cast<decltype(lhs)>(rhs));
}
template<typename T, typename Container>
constexpr auto compare(basic_array_iterator<T, Container> const lhs, basic_array_iterator<T const, Container> const rhs) noexcept {
	return compare(static_cast<decltype(rhs)>(lhs), rhs);
}

template<typename T, typename Container>
constexpr auto operator-(basic_array_iterator<T, Container> const lhs, basic_array_iterator<T, Container> const rhs) noexcept {
	return static_cast<typename basic_array_iterator<T, Container>::difference_type>(pointer_from(lhs) - pointer_from(rhs));
}
template<typename T, typename Container>
constexpr auto operator-(basic_array_iterator<T const, Container> const lhs, basic_array_iterator<T, Container> const rhs) noexcept {
	return lhs - static_cast<decltype(lhs)>(rhs);
}
template<typename T, typename Container>
constexpr auto operator-(basic_array_iterator<T, Container> const lhs, basic_array_iterator<T const, Container> const rhs) noexcept {
	return static_cast<decltype(rhs)>(lhs) - rhs;
}


}	// namespace detail
}	// namespace containers
