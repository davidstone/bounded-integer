// Array class with better interoperability with bounded than std::array
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_DETAIL_ARRAY_HPP_
#define BOUNDED_INTEGER_DETAIL_ARRAY_HPP_

#include "../arithmetic_operators.hpp"
#include "../class.hpp"
#include "../make.hpp"
#include "../policy/throw_policy.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace bounded {

template<typename T, std::size_t size>
class array;

namespace detail {

template<typename T, intmax_t size>
class iterator {
public:
	using value_type = T;
	using difference_type = integer<-size, size>;
	using index_type = integer<0, size - 1, throw_policy>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr iterator() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator iterator<T const, size>() const noexcept {
		return iterator<T const, size>(m_it);
	}

	constexpr auto operator*() const -> reference {
		return *m_it;
	}
	constexpr auto operator->() const -> pointer {
		return & operator*();
	}
	auto operator++() -> iterator & {
		++m_it;
		return *this;
	}
	auto operator++(int) -> iterator {
		auto const self = *this;
		operator++();
		return self;
	}
	auto operator--() -> iterator & {
		--m_it;
		return *this;
	}
	auto operator--(int) -> iterator {
		auto const self = *this;
		operator--();
		return self;
	}
	auto operator+=(difference_type const offset) -> iterator & {
		m_it += offset;
		return *this;
	}
	auto operator-=(difference_type const offset) -> iterator & {
		m_it -= offset;
		return *this;
	}
	friend constexpr auto operator-(iterator const & lhs, iterator const & rhs) -> difference_type {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}
	constexpr auto operator[](index_type const index) const -> reference {
		return m_it[static_cast<std::size_t>(index)];
	}

	friend constexpr auto operator==(iterator const & lhs, iterator const & rhs) noexcept -> bool {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(iterator const & lhs, iterator const & rhs) noexcept -> bool {
		return lhs.m_it < rhs.m_it;
	}

private:
	template<typename U, std::size_t size_>
	friend class ::bounded::array;
	template<typename U, intmax_t size_>
	friend class iterator;

	using base_iterator = value_type *;
	constexpr explicit iterator(base_iterator const other) noexcept:
		m_it(other) {
	}

	base_iterator m_it;
};

template<typename T, intmax_t size>
constexpr auto operator!=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs == rhs);
}
template<typename T, intmax_t size>
constexpr auto operator>(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept -> bool {
	return rhs < lhs;
}
template<typename T, intmax_t size>
constexpr auto operator<=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs > rhs);
}
template<typename T, intmax_t size>
constexpr auto operator>=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs < rhs);
}

template<typename T, intmax_t size>
constexpr auto operator+(iterator<T, size> lhs, typename iterator<T, size>::difference_type const rhs) -> iterator<T, size> {
	return lhs += rhs;
}
template<typename T, intmax_t size>
constexpr auto operator+(typename iterator<T, size>::difference_type const lhs, iterator<T, size> rhs) -> iterator<T, size> {
	return rhs += lhs;
}
template<typename T, intmax_t size>
constexpr auto operator-(typename iterator<T, size>::difference_type const lhs, iterator<T, size> rhs) -> iterator<T, size> {
	return lhs -= rhs;
}

namespace adl {

using std::swap;

template<typename T, typename U>
struct is_nothrow_swappable : std::integral_constant<bool, noexcept(swap(std::declval<T &>(), std::declval<U &>()))> {};

}	//	namespace adl
}	// namespace detail

template<typename T, std::size_t size_>
class array {
public:
	using value_type = T;

	using const_iterator = detail::iterator<value_type const, size_>;
	using iterator = detail::iterator<value_type, size_>;
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
		return make<size_>();
	}
	constexpr auto empty() const noexcept -> bool {
		return size() == make<0>();
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
		return operator[](make<0>());
	}
	auto front() noexcept -> reference {
		return operator[](make<0>());
	}
	constexpr auto back() const noexcept -> const_reference {
		return operator[](size() - make<1>());
	}
	auto back() noexcept -> reference {
		return operator[](size() - make<1>());
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
	
	auto swap(array & other) noexcept(detail::adl::is_nothrow_swappable<reference, reference>::value) -> void {
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
	return a[make<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto get(array<T, size> & a) noexcept -> T & {
	return a[make<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto get(array<T, size> && a) noexcept -> T && {
	return std::move(a[make<index>()]);
}

template<typename T, std::size_t size>
auto swap(array<T, size> & lhs, array<T, size> & rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
	lhs.swap(rhs);
}
}	// namespace bounded

// I am not sure yet if it is legal to specialize these classes.
#if 0
namespace std {

template<typename T, size_t size>
struct tuple_size<::array<T, size>> : public integral_constant<size_t, size> {};

template<size_t index, typename T, size_t size>
struct tuple_element<index, ::array<T, size>> {
	using type = T;
};

}	// namespace std
#endif

#endif	// BOUNDED_INTEGER_DETAIL_ARRAY_HPP_
