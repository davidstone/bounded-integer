// A vector-like class that can take advantage of cheap moves
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

#include <containers/algorithms/iterator.hpp>
#include <containers/moving_vector/forward_declaration.hpp>
#include <containers/moving_vector/moving_vector_iterator.hpp>

#include <value_ptr/value_ptr.hpp>

#include <iterator>
#include <type_traits>
#include <vector>

namespace containers {

template<typename T, typename Allocator>
class moving_vector {
public:
	using value_type = T;
private:
	using element_type = smart_pointer::value_ptr<value_type>;
	using container_type = std::vector<element_type>;
public:
	using allocator_type = Allocator;
	using size_type = typename container_type::size_type;
	using difference_type = typename container_type::difference_type;
	using const_reference = value_type const &;
	using reference = value_type &;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	
	using const_iterator = detail::moving_vector_iterator<value_type, value_type const>;
	using iterator = detail::moving_vector_iterator<value_type, value_type>;
	// There is no const_indirect_iterator because there is no way to enforce it
	using indirect_iterator = detail::moving_vector_iterator<value_type, element_type>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using reverse_indirect_iterator = std::reverse_iterator<indirect_iterator>;
	
	moving_vector() {
	}
	explicit moving_vector(allocator_type const &) {
	}
	explicit moving_vector(size_type count, allocator_type const & = allocator_type{}) {
		for (size_type n = 0; n != count; ++n) {
			emplace_back();
		}
	}
	moving_vector(size_type count, value_type const & value, allocator_type const & = allocator_type{}) {
		assign(count, value);
	}
	template<typename InputIterator>
	moving_vector(InputIterator first, InputIterator last, allocator_type const & = allocator_type{}) {
		assign(first, last);
	}
	moving_vector(moving_vector const & other, allocator_type const &):
		moving_vector(other) {
	}                                                                       
	moving_vector(moving_vector && other, allocator_type const &):
		moving_vector(std::move(other)) {
	}
	moving_vector(std::initializer_list<value_type> init, allocator_type const & allocator = allocator_type{}):
		moving_vector(std::begin(init), std::end(init), allocator) {
	}
	
	auto assign(size_type count, value_type const & value) {
		clear();
		reserve(count);
		for (size_type n = 0; n != count; ++n) {
			emplace_back(value);
		}
	}
	template<typename InputIterator>
	auto assign(InputIterator first, InputIterator last) {
		clear();
		if (std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::random_access_iterator_tag>::value) {
			reserve(static_cast<size_type>(std::distance(first, last)));
		}
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	auto assign(std::initializer_list<value_type> init) {
		assign(std::begin(init), std::end(init));
	}
	
	constexpr auto && at(size_type position) const {
		return *m_container.at(position);
	}
	auto && at(size_type position) {
		return *m_container.at(position);
	}
	constexpr auto && operator[](size_type position) const {
		return *m_container[position];
	}
	auto && operator[](size_type position) {
		return *m_container[position];
	}
	
	constexpr auto && front() const {
		return *begin();
	}
	auto && front() {
		return *begin();
	}
	constexpr auto && back() const {
		return *::containers::prev(end());
	}
	auto && back() {
		return *::containers::prev(end());
	}
	
	// data() intentionally missing
	
	auto begin() const noexcept {
		return const_iterator(m_container.begin());
	}
	auto begin() noexcept {
		return iterator(m_container.begin());
	}
	auto cbegin() const noexcept {
		return begin();
	}
	auto indirect_begin() noexcept {
		return indirect_iterator(m_container.begin());
	}
	
	auto end() const noexcept {
		return begin() + static_cast<difference_type>(size());
	}
	auto end() noexcept {
		return begin() + static_cast<difference_type>(size());
	}
	auto cend() const noexcept {
		return end();
	}
	auto indirect_end() noexcept {
		return indirect_iterator(m_container.end());
	}
	
	auto rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	auto rbegin() noexcept {
		return reverse_iterator(end());
	}
	auto crbegin() const noexcept {
		return rbegin();
	}
	auto indirect_rbegin() noexcept {
		return indirect_iterator(indirect_end());
	}
	
	auto rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	auto rend() noexcept {
		return reverse_iterator(begin());
	}
	auto crend() const noexcept {
		return rend();
	}
	auto indirect_rend() noexcept {
		return indirect_iterator(indirect_begin());
	}
	
	auto empty() const noexcept {
		return m_container.empty();
	}
	auto size() const noexcept {
		return m_container.size();
	}
	auto max_size() const noexcept {
		return m_container.max_size();
	}

	auto capacity() const noexcept {
		return m_container.capacity();
	}
	auto reserve(size_type const new_capacity) noexcept {
		return m_container.reserve(new_capacity);
	}
	auto shink_to_fit() {
		m_container.shrink_to_fit();
	}
	
	auto clear() noexcept {
		m_container.clear();
	}
	
	template<typename... Args>
	auto emplace(const_iterator const position, Args && ... args) {
		return iterator(m_container.emplace(make_base_iterator(position), smart_pointer::make_value<value_type>(std::forward<Args>(args)...)));
	}
	
	auto insert(const_iterator const position, value_type const & value) {
		return emplace(position, value);
	}
	auto insert(const_iterator const position, value_type && value) {
		return emplace(position, std::move(value));
	}
	auto insert(const_iterator const position, size_type const count, value_type const & value) {
		for (size_type n = 0; n != count; ++n) {
			emplace(position, value);
		}
	}
	template<typename InputIterator>
	auto insert(const_iterator position, InputIterator first, InputIterator const last) {
		if (first == last) {
			return static_cast<iterator>(position);
		}
		auto const offset = position - begin();
		m_container.insert(make_base_iterator(position), first, last);
		return begin() + offset + 1;
	}
	auto insert(const_iterator const position, std::initializer_list<value_type> ilist) {
		return insert(position, std::begin(ilist), std::end(ilist));
	}

	template<typename... Args>
	auto emplace_back(Args && ... args) {
		m_container.emplace_back(smart_pointer::make_value<value_type>(std::forward<Args>(args)...));
	}
	
	auto push_back(value_type const & value) {
		emplace_back(value);
	}
	auto push_back(value_type && value) {
		emplace_back(std::move(value));
	}
	
	auto erase(const_iterator const position) {
		return iterator(m_container.erase(make_base_iterator(position)));
	}
	auto erase(const_iterator const first, const_iterator const last) {
		return iterator(m_container.erase(make_base_iterator(first), make_base_iterator(last)));
	}
	auto pop_back() {
		m_container.pop_back();
	}
	
	auto resize(size_type const new_size) {
		auto old_size = size();
		if (old_size < new_size) {
			m_container.reserve(new_size);
			for ( ; old_size != new_size; ++old_size) {
				m_container.emplace_back();
			}
		}
		else {
			m_container.resize(new_size);
		}
	}
	auto resize(size_type const count, value_type const & value) {
		m_container.resize(count, smart_pointer::make_value<value_type>(value));
	}
	auto resize(size_type const count, value_type && value) {
		m_container.resize(count, smart_pointer::make_value<value_type>(std::move(value)));
	}
	
	friend auto operator==(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	friend auto operator<(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
private:
	constexpr auto make_base_iterator(const_iterator const it) const {
		return m_container.begin() + std::distance(cbegin(), it);
	}
	auto make_base_iterator(const_iterator const it) {
		return m_container.begin() + std::distance(cbegin(), it);
	}
	container_type m_container;
};

// For regular containers, the iterator you need to move elements around is just
// a regular iterator
template<typename Container>
auto moving_begin(Container && container) {
	return std::forward<Container>(container).begin();
}
template<typename T, typename Allocator>
auto moving_begin(moving_vector<T, Allocator> & container) {
	return container.indirect_begin();
}
template<typename T, typename Allocator>
auto moving_begin(moving_vector<T, Allocator> && container) {
	return std::move(container).begin();
}
template<typename Container>
auto moving_end(Container && container) {
	return std::forward<Container>(container).end();
}
template<typename T, typename Allocator>
auto moving_end(moving_vector<T, Allocator> & container) {
	return container.indirect_end();
}
template<typename T, typename Allocator>
auto moving_end(moving_vector<T, Allocator> && container) {
	return std::move(container).indirect_end();
}

template<typename T, typename Allocator>
auto operator!=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, typename Allocator>
auto operator>(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename T, typename Allocator>
auto operator<=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, typename Allocator>
auto operator>=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace containers
