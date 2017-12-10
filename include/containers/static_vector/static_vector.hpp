// std::vector-like interface around a bounded::array
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

#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <containers/static_vector/forward_declaration.hpp>
#include <containers/static_vector/iterator.hpp>

#include <bounded_integer/array.hpp>
#include <bounded_integer/integer_range.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector {
	using value_type = T;
	using size_type = bounded::integer<0, capacity_>;
	using const_iterator = detail::static_vector_iterator<value_type const, capacity_>;
	using iterator = detail::static_vector_iterator<value_type, capacity_>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
private:
	using container_type = bounded::array<uninitialized_storage<value_type>, capacity_>;
public:
	using index_type = typename container_type::index_type;
	
	static_vector() = default;
	
	template<typename Count, enable_if_t<std::is_convertible<Count, size_type>::value> = clang_dummy>
	explicit static_vector(Count const count) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count)
	) {}
	template<typename Count, enable_if_t<std::is_convertible<Count, size_type>::value> = clang_dummy>
	static_vector(Count const count, value_type const & value) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count, value)
	) {}
	
	template<typename InputIterator, typename Sentinel>
	static_vector(InputIterator first, Sentinel const last) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(emplace_back(*first))) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	
	static_vector(static_vector const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(other.begin(), other.end())
	) {}
	static_vector(static_vector && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()))
	) {}

	static_vector(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(init.begin(), init.end())
	) {}

	auto & operator=(static_vector const & other) & noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
		assign(other.begin(), other.end());
		return *this;
	}
	auto & operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable<value_type>::value) {
		assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
		return *this;
	}
	auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(init.begin(), init.end()))) {
		assign(init.begin(), init.end());
		return *this;
	}

	~static_vector() {
		clear();
	}


	auto begin() const noexcept {
		return const_iterator(m_container.begin());
	}
	auto begin() noexcept {
		return iterator(m_container.begin());
	}
	auto end() const noexcept {
		return begin() + m_size;
	}
	auto end() noexcept {
		return begin() + m_size;
	}


	auto && operator[](index_type index) const noexcept {
		return *(begin() + index);
	}
	auto && operator[](index_type index) noexcept {
		return *(begin() + index);
	}
	template<typename Index>
	auto && at(Index const index) const noexcept(noexcept(static_cast<index_type>(index))) {
		return operator[](static_cast<index_type>(index));
	}
	template<typename Index>
	auto && at(Index const index) noexcept(noexcept(static_cast<index_type>(index))) {
		return operator[](static_cast<index_type>(index));
	}
	
	auto && front() const {
		return *begin();
	}
	auto && front() {
		return *begin();
	}
	auto && back() const {
		return *bounded::prev(end());
	}
	auto && back() {
		return *bounded::prev(end());
	}
	
	auto data() const noexcept {
		return reinterpret_cast<value_type const *>(m_container.data());
	}
	auto data() noexcept {
		return reinterpret_cast<value_type *>(m_container.data());
	}

	auto size() const noexcept {
		return size_type(end() - begin());
	}
	auto empty() const noexcept {
		return begin() == end();
	}

	static constexpr auto capacity() noexcept {
		return bounded::constant<capacity_>;
	}
	static constexpr auto max_size() noexcept {
		return capacity();
	}

	template<typename... Args>
	auto emplace_back(Args && ... args) {
		assert(size() != capacity());
		new(m_container.data() + size()) value_type(std::forward<Args>(args)...);
		++m_size;
	}
	auto push_back(value_type && value) {
		emplace_back(std::move(value));
	}
	auto push_back(value_type const & value) {
		emplace_back(value);
	}
	
	// TODO: replace iterator with const_iterator
	template<typename... Args>
	auto emplace(iterator const position, Args && ... args) {
		if (position == end()) {
			emplace_back(std::forward<Args>(args)...);
			return position;
		}
		emplace_back(std::move(back()));
		std::move(position, bounded::prev(end()), bounded::next(position));
		auto const pointer = std::addressof(*position);
		pointer->~value_type();
		::new(static_cast<void *>(pointer)) value_type(std::forward<Args>(args)...);
		return position;
	}
	auto insert(iterator const position, value_type const & value) {
		return emplace(position, value);
	}
	auto insert(iterator const position, value_type && value) {
		return emplace(position, std::move(value));
	}
	template<typename InputIterator, typename Sentinel>
	auto insert(iterator const position, InputIterator first, Sentinel last) {
		if (position == end()) {
			for (; first != last; ++first) {
				emplace_back(*first);
			}
			return position;
		}
		// TODO: This will likely need std::uninitialized_copy and std::move
		return position;
	}
	auto insert(iterator const position, std::initializer_list<value_type> init) {
		return insert(position, init.begin(), init.end());
	}
	template<typename Size>
	auto insert(iterator const position, Size const count, value_type const & value) {
		auto const range = detail::repeat_n(count, value);
		return insert(position, range.begin(), range.end());
	}

	template<typename InputIterator, typename Sentinel>
	auto assign(InputIterator first, Sentinel const last) {
		// TODO: Determine whether we should try to reuse storage like this or
		// just clear() + construct
		auto it = begin();
		for (; first != last; ++first) {
			if (it == end()) {
				break;
			}
			*it = *first;
			++it;
		}
		erase(it, end());
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	auto assign(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT(
		assign(init.begin(), init.end())
	)
	template<typename Size>
	auto assign(Size const count, value_type const & value) {
		auto const range = detail::repeat_n(count, value);
		assign(range.begin(), range.end());
	}
	

	auto erase(iterator const first, iterator const last) noexcept {
		auto const to_clear = std::move(last, end(), first);
		while (to_clear != end()) {
			pop_back();
		}
	}
	auto erase(iterator const it) {
		assert(it != end());
		erase(it, bounded::next(it));
	}
	
	auto clear() noexcept {
		erase(begin(), end());
	}

	void pop_back() {
		assert(!empty());
		bounded::prev(end())->~value_type();
		--m_size;
	}
	
	template<typename Size>
	auto resize(Size const count) BOUNDED_NOEXCEPT(
		resize_generic(count)
	)
	template<typename Size>
	auto resize(Size const count, value_type const & value) BOUNDED_NOEXCEPT(
		resize_generic(count, value)
	)

	
private:
	enum class count_constructor{};
	template<typename Count, typename... MaybeInitializer>
	explicit static_vector(count_constructor, Count const count, MaybeInitializer && ... args) noexcept(std::is_nothrow_constructible<value_type, MaybeInitializer && ...>::value) {
		static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
		for (auto const n : bounded::integer_range(count)) {
			static_cast<void>(n);
			emplace_back(std::forward<MaybeInitializer>(args)...);
		}
	}
	
	template<typename Size, typename... MaybeInitializer>
	auto resize_generic(Size const count, MaybeInitializer && ... args) {
		while (size() > count) {
			pop_back();
		}
		while (size() < count) {
			emplace_back(std::forward<MaybeInitializer>(args)...);
		}
	}
	
	container_type m_container = {{}};
	size_type m_size = bounded::constant<0>;
};

template<typename T, std::size_t capacity>
auto operator==(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template<typename T, std::size_t capacity>
auto operator!=(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return !(lhs == rhs);
}

template<typename T, std::size_t capacity>
auto operator<(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, std::size_t capacity>
auto operator>(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return rhs < lhs;
}
template<typename T, std::size_t capacity>
auto operator<=(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return !(rhs < lhs);
}
template<typename T, std::size_t capacity>
auto operator>=(static_vector<T, capacity> const & lhs, static_vector<T, capacity> const & rhs) {
	return !(lhs < rhs);
}


}	// namespace containers
