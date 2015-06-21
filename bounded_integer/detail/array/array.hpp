// Array class with better interoperability with bounded than std::array
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "iterator.hpp"

#include "../class.hpp"
#include "../comparison.hpp"
#include "../arithmetic/minus.hpp"
#include "../arithmetic/pointer.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace bounded {

namespace detail {

namespace adl {

using std::swap;

template<typename T, typename U>
struct is_nothrow_swappable : std::integral_constant<bool, noexcept(swap(std::declval<T &>(), std::declval<U &>()))> {};

}	//	namespace adl

template<typename T, std::size_t size_>
struct array {
	using value_type = T;

	using const_iterator = basic_iterator<value_type const, size_>;
	using iterator = basic_iterator<value_type, size_>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	using size_type = integer<size_, size_>;
	using difference_type = typename const_iterator::difference_type;
	using index_type = typename const_iterator::index_type;
	using const_reference = typename const_iterator::reference;
	using reference = typename iterator::reference;
	using const_pointer = typename const_iterator::pointer;
	using pointer = typename iterator::pointer;
	
	constexpr auto size() const noexcept -> size_type {
		return constant<size_>;
	}
	constexpr auto empty() const noexcept -> bool {
		return size() == constant<0>;
	}
	constexpr auto max_size() const noexcept -> size_type {
		return size();
	}
	
	
	constexpr auto data() const noexcept -> const_pointer {
		return m_value;
	}
	auto data() noexcept -> pointer {
		return m_value;
	}

	constexpr auto operator[](index_type const & index) const noexcept -> const_reference {
		return *(data() + index);
	}
	auto operator[](index_type const & index) noexcept -> reference {
		return *(data() + index);
	}
	template<typename Index>
	constexpr auto at(Index const & index) const noexcept(noexcept(static_cast<index_type>(index))) -> const_reference {
		return operator[](static_cast<index_type>(index));
	}
	template<typename Index>
	auto at(Index const & index) noexcept(noexcept(static_cast<index_type>(index))) -> reference {
		return operator[](static_cast<index_type>(index));
	}
	template<typename Index>
	constexpr auto at(Index const & index, non_check_t) const noexcept -> const_reference {
		return operator[](index_type(index, non_check));
	}
	template<typename Index>
	auto at(Index const & index, non_check_t) noexcept -> reference {
		return operator[](index_type(index, non_check));
	}

	constexpr auto front() const noexcept -> const_reference {
		return operator[](constant<0>);
	}
	auto front() noexcept -> reference {
		return operator[](constant<0>);
	}
	constexpr auto back() const noexcept -> const_reference {
		return operator[](size() - constant<1>);
	}
	auto back() noexcept -> reference {
		return operator[](size() - constant<1>);
	}

	constexpr auto begin() const noexcept -> const_iterator {
		return const_iterator(data());
	}
	auto begin() noexcept -> iterator {
		return iterator(data());
	}
	constexpr auto cbegin() const noexcept -> const_iterator {
		return begin();
	}

	constexpr auto end() const noexcept -> const_iterator {
		return const_iterator(data() + size());
	}
	auto end() noexcept -> iterator  {
		return iterator(data() + size());
	}
	constexpr auto cend() const noexcept -> const_iterator {
		return end();
	}

	constexpr auto rbegin() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator(end());
	}
	auto rbegin() noexcept -> reverse_iterator  {
		return reverse_iterator(end());
	}
	constexpr auto crbegin() const noexcept -> const_reverse_iterator {
		return rbegin();
	}

	constexpr auto rend() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator(begin());
	}
	auto rend() noexcept -> reverse_iterator {
		return reverse_iterator(begin());
	}
	constexpr auto crend() const noexcept -> const_reverse_iterator {
		return rend();
	}
	
	auto fill(const_reference value) noexcept(std::is_nothrow_copy_constructible<value_type>::value) -> void  {
		std::fill(begin(), end(), value);
	}
	
	auto swap(array & other) noexcept(adl::is_nothrow_swappable<reference, reference>::value) -> void {
		std::swap_ranges(begin(), end(), other.begin());
	}

	// Consider this private. It must be public for the class to be an
	// aggregate. Specialized for the 0-element case, see
	// https://stackoverflow.com/questions/15512827/why-is-stdarray-t-0-not-empty
	value_type m_value[size_ == 0 ? 1 : size_];
};

template<typename T, std::size_t size>
constexpr auto operator==(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(std::declval<typename array<T, size>::value_type>() == std::declval<typename array<T, size>::value_type>())) -> bool {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, std::size_t size>
constexpr auto operator!=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(lhs == rhs))) -> bool {
	return !(lhs == rhs);
}

template<typename T, std::size_t size>
constexpr auto operator<(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(std::declval<typename array<T, size>::value_type>() < std::declval<typename array<T, size>::value_type>())) -> bool {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, std::size_t size>
constexpr auto operator>(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(rhs < lhs)) -> bool {
	return rhs < lhs;
}

template<typename T, std::size_t size>
constexpr auto operator<=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(rhs < lhs))) -> bool {
	return !(rhs < lhs);
}

template<typename T, std::size_t size>
constexpr auto operator>=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(lhs < rhs))) -> bool {
	return !(lhs < rhs);
}

template<std::size_t index, typename T, std::size_t size>
constexpr auto get(array<T, size> const & a) noexcept -> T const & {
	return a[constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto get(array<T, size> & a) noexcept -> T & {
	return a[constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto get(array<T, size> && a) noexcept -> T && {
	return std::move(a[constant<index>]);
}

template<typename T, std::size_t size>
auto swap(array<T, size> & lhs, array<T, size> & rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
	lhs.swap(rhs);
}

template<typename T, std::size_t dimension, std::size_t... dimensions>
struct multi_dimensional_array {
	using type = array<typename multi_dimensional_array<T, dimensions...>::type, dimension>;
};

template<typename T, std::size_t dimension>
struct multi_dimensional_array<T, dimension> {
	using type = array<T, dimension>;
};

}	// namespace detail

template<typename T, std::size_t... dimensions>
using array = typename detail::multi_dimensional_array<T, dimensions...>::type;

}	// namespace bounded

// I am not sure yet if it is legal to specialize these classes.
#if 0
namespace std {

template<typename T, size_t size>
struct tuple_size<::array<T, size>> : integral_constant<size_t, size> {};

template<size_t index, typename T, size_t size>
struct tuple_element<index, ::array<T, size>> {
	using type = T;
};

}	// namespace std
#endif

