// A vector-like class that can take advantage of cheap moves
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

#ifndef CONTAINERS_MOVING_VECTOR_HPP_
#define CONTAINERS_MOVING_VECTOR_HPP_

#include <iterator>
#include <type_traits>
#include <vector>
#include <value_ptr/enable_if.hpp>
#include <value_ptr/value_ptr.hpp>

namespace containers {

template<typename T, typename Allocator = std::allocator<T>>
class moving_vector;

namespace detail {
namespace vector {
using namespace smart_pointer;

template<typename T, typename ValueType,
	enable_if_t<
		std::is_same<
			typename std::remove_const<T>::type,
			typename std::remove_const<ValueType>::type
		>::value
	> = enabler_dummy
>
ValueType const * remove_double_indirection(value_ptr<T> const * ptr) {
	return ptr->get();
}
template<typename T, typename ValueType,
	enable_if_t<
		std::is_same<
			typename std::remove_const<T>::type,
			typename std::remove_const<ValueType>::type
		>::value
	> = enabler_dummy
>
ValueType * remove_double_indirection(value_ptr<T> * ptr) {
	return ptr->get();
}
template<typename T, typename ValueType,
	enable_if_t<
		std::is_same<
			value_ptr<T>,
			typename std::remove_const<ValueType>::type
		>::value
	> = enabler_dummy
>
ValueType const * remove_double_indirection(value_ptr<T> const * ptr) {
	return ptr;
}
template<typename T, typename ValueType,
	enable_if_t<
		std::is_same<
			value_ptr<T>,
			typename std::remove_const<ValueType>::type
		>::value
	> = enabler_dummy
>
ValueType * remove_double_indirection(value_ptr<T> * ptr) {
	return ptr;
}

template<typename T, typename ValueType>
class iterator_base {
private:
	using indirection_type = value_ptr<T>;
public:
	using value_type = ValueType;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr iterator_base() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator iterator_base<T, value_type const>() const noexcept {
		return iterator_base<T, value_type const>(it);
	}
	// Convert iterator to indirect_iterator.
	constexpr explicit operator iterator_base<T, indirection_type>() const noexcept {
		return iterator_base<T, indirection_type>(it);
	}

	reference operator*() const {
		return *remove_double_indirection<T, ValueType>(it);
	}
	pointer operator->() const {
		return & this->operator*();
	}
	iterator_base & operator++() {
		++it;
		return *this;
	}
	iterator_base operator++(int) {
		auto const self = *this;
		operator++();
		return self;
	}
	iterator_base & operator--() {
		--it;
		return *this;
	}
	iterator_base operator--(int) {
		auto const self = *this;
		operator--();
		return self;
	}
	iterator_base & operator+=(difference_type const offset) {
		it += offset;
		return *this;
	}
	iterator_base & operator-=(difference_type const offset) {
		it -= offset;
		return *this;
	}
	friend constexpr difference_type operator-(iterator_base const & lhs, iterator_base const & rhs) {
		return lhs.it - rhs.it;
	}
	template<typename Integer>
	reference operator[](Integer const index) {
		return it[index];
	}

	friend constexpr bool operator==(iterator_base const & lhs, iterator_base const & rhs) noexcept {
		return lhs.it == rhs.it;
	}
	friend constexpr bool operator<(iterator_base const & lhs, iterator_base const & rhs) noexcept {
		return lhs.it < rhs.it;
	}
	
private:
	template<typename U, typename Allocator>
	friend class ::containers::moving_vector;
	template<typename U, typename VT>
	friend class iterator_base;

	static constexpr bool is_const = std::is_const<value_type>::value;
	using base_iterator = typename std::conditional<is_const, indirection_type const, indirection_type>::type *;

	constexpr explicit iterator_base(base_iterator const other) noexcept:
		it(other) {
	}
	constexpr explicit iterator_base(typename std::vector<indirection_type>::iterator const other):
		it(&*other) {
	}
	constexpr explicit iterator_base(typename std::vector<indirection_type>::const_iterator const other):
		it(&*other) {
	}

	// Convert const_iterator to iterator
	constexpr explicit operator iterator_base<T, T>() const noexcept {
		using mutable_type = typename std::conditional<std::is_const<T>::value, indirection_type const, indirection_type>::type *;
		return iterator_base<T, T>(const_cast<mutable_type>(it));
	}

	base_iterator it;
};

template<typename T, typename ValueType>
constexpr bool operator!=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, typename ValueType>
constexpr bool operator>(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return rhs < lhs;
}
template<typename T, typename ValueType>
constexpr bool operator<=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, typename ValueType>
constexpr bool operator>=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs < rhs);
}

template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator+(iterator_base<T, ValueType> lhs, typename iterator_base<T, ValueType>::difference_type const rhs) {
	return lhs += rhs;
}
template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator+(typename iterator_base<T, ValueType>::difference_type const lhs, iterator_base<T, ValueType> rhs) {
	return rhs += lhs;
}
template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator-(iterator_base<T, ValueType> lhs, typename iterator_base<T, ValueType>::difference_type const rhs) {
	return lhs -= rhs;
}

}	// namespace vector
}	// namespace detail

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
	
	using const_iterator = detail::vector::iterator_base<value_type, value_type const>;
	using iterator = detail::vector::iterator_base<value_type, value_type>;
	// There is no const_indirect_iterator because there is no way to enforce it
	using indirect_iterator = detail::vector::iterator_base<value_type, element_type>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using reverse_indirect_iterator = std::reverse_iterator<indirect_iterator>;
	
	explicit moving_vector(allocator_type const & allocator = allocator_type{}) {
	}
	explicit moving_vector(size_type count, allocator_type const & allocator = allocator_type{}) {
		for (size_type n = 0; n != count; ++n) {
			emplace_back();
		}
	}
	moving_vector(size_type count, value_type const & value, allocator_type const & allocator = allocator_type{}) {
		assign(count, value);
	}
	template<typename InputIterator>
	moving_vector(InputIterator first, InputIterator last, allocator_type const & allocator = allocator_type{}) {
		assign(first, last);
	}
	moving_vector(moving_vector const & other, allocator_type const & allocator):
		moving_vector(other) {
	}
	moving_vector(moving_vector && other, allocator_type const & allocator):
		moving_vector(std::move(other)) {
	}
	moving_vector(std::initializer_list<value_type> init, allocator_type const & allocator = allocator_type{}):
		moving_vector(std::begin(init), std::end(init), allocator) {
	}
	
	void assign(size_type count, value_type const & value) {
		clear();
		reserve(count);
		for (size_type n = 0; n != count; ++n) {
			emplace_back(value);
		}
	}
	template<typename InputIterator>
	void assign(InputIterator first, InputIterator last) {
		clear();
		if (std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, std::random_access_iterator_tag>::value) {
			reserve(static_cast<size_type>(std::distance(first, last)));
		}
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	void assign(std::initializer_list<value_type> init) {
		assign(std::begin(init), std::end(init));
	}
	
	constexpr const_reference at(size_type position) const {
		return *m_container.at(position);
	}
	reference at(size_type position) {
		return *m_container.at(position);
	}
	constexpr const_reference operator[](size_type position) const {
		return *m_container[position];
	}
	reference operator[](size_type position) {
		return *m_container[position];
	}
	
	constexpr const_reference front() const {
		return *begin();
	}
	reference front() {
		return *begin();
	}
	constexpr const_reference back() const {
		return *std::prev(end());
	}
	reference back() {
		return *std::prev(end());
	}
	
	// data() intentionally missing
	
	const_iterator begin() const noexcept {
		return const_iterator(m_container.begin());
	}
	iterator begin() noexcept {
		return iterator(m_container.begin());
	}
	const_iterator cbegin() const noexcept {
		return begin();
	}
	indirect_iterator indirect_begin() noexcept {
		return indirect_iterator(m_container.begin());
	}
	
	const_iterator end() const noexcept {
		return begin() + static_cast<difference_type>(size());
	}
	iterator end() noexcept {
		return begin() + static_cast<difference_type>(size());
	}
	const_iterator cend() const noexcept {
		return end();
	}
	indirect_iterator indirect_end() noexcept {
		return indirect_iterator(m_container.end());
	}
	
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}
	indirect_iterator indirect_rbegin() noexcept {
		return indirect_iterator(indirect_end());
	}
	
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	const_reverse_iterator crend() const noexcept {
		return rend();
	}
	indirect_iterator indirect_rend() noexcept {
		return indirect_iterator(indirect_begin());
	}
	
	bool empty() const noexcept {
		return m_container.empty();
	}
	size_type size() const noexcept {
		return m_container.size();
	}
	size_type max_size() const noexcept {
		return m_container.max_size();
	}

	size_type capacity() const noexcept {
		return m_container.capacity();
	}
	void reserve(size_type const new_capacity) noexcept {
		return m_container.reserve(new_capacity);
	}
	void shink_to_fit() {
		m_container.shrink_to_fit();
	}
	
	void clear() noexcept {
		m_container.clear();
	}
	
	template<typename... Args>
	iterator emplace(const_iterator const position, Args && ... args) {
		return iterator(m_container.emplace(make_base_iterator(position), smart_pointer::make_value<value_type>(std::forward<Args>(args)...)));
	}
	
	iterator insert(const_iterator const position, value_type const & value) {
		return emplace(position, value);
	}
	iterator insert(const_iterator const position, value_type && value) {
		return emplace(position, std::move(value));
	}
	iterator insert(const_iterator const position, size_type const count, value_type const & value) {
		for (size_type n = 0; n != count; ++n) {
			emplace(position, value);
		}
	}
	template<typename InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator const last) {
		if (first == last) {
			return static_cast<iterator>(position);
		}
		auto const offset = position - begin();
		m_container.insert(make_base_iterator(position), first, last);
		return begin() + offset + 1;
	}
	iterator insert(const_iterator const position, std::initializer_list<value_type> ilist) {
		return insert(position, std::begin(ilist), std::end(ilist));
	}

	template<typename... Args>
	void emplace_back(Args && ... args) {
		m_container.emplace_back(smart_pointer::make_value<value_type>(std::forward<Args>(args)...));
	}
	
	void push_back(value_type const & value) {
		emplace_back(value);
	}
	void push_back(value_type && value) {
		emplace_back(std::move(value));
	}
	
	iterator erase(const_iterator const position) {
		return iterator(m_container.erase(make_base_iterator(position)));
	}
	iterator erase(const_iterator const first, const_iterator const last) {
		return iterator(m_container.erase(make_base_iterator(first), make_base_iterator(last)));
	}
	void pop_back() {
		m_container.pop_back();
	}
	
	void resize(size_type const new_size) {
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
	void resize(size_type const count, value_type const & value) {
		m_container.resize(count, smart_pointer::make_value<value_type>(value));
	}
	void resize(size_type const count, value_type && value) {
		m_container.resize(count, smart_pointer::make_value<value_type>(std::move(value)));
	}
	
	void swap(moving_vector & other) noexcept {
		m_container.swap(other.m_container);
	}

	friend bool operator==(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	friend bool operator<(moving_vector const & lhs, moving_vector const & rhs) noexcept {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
private:
	constexpr typename container_type::const_iterator make_base_iterator(const_iterator const it) const {
		return m_container.begin() + std::distance(cbegin(), it);
	}
	typename container_type::iterator make_base_iterator(const_iterator const it) {
		return m_container.begin() + std::distance(cbegin(), it);
	}
	container_type m_container;
};

// For regular containers, the iterator you need to move elements around is just
// a regular iterator
template<typename Container>
typename std::remove_reference<Container>::type::iterator moving_begin(Container && container) {
	return std::forward<Container>(container).begin();
}
template<typename T, typename Allocator>
typename moving_vector<T, Allocator>::indirect_iterator moving_begin(moving_vector<T, Allocator> & container) {
	return container.indirect_begin();
}
template<typename T, typename Allocator>
typename moving_vector<T, Allocator>::indirect_iterator moving_begin(moving_vector<T, Allocator> && container) {
	return std::move(container).begin();
}
template<typename Container>
typename std::remove_reference<Container>::type::iterator moving_end(Container && container) {
	return std::forward<Container>(container).end();
}
template<typename T, typename Allocator>
typename moving_vector<T, Allocator>::indirect_iterator moving_end(moving_vector<T, Allocator> & container) {
	return container.indirect_end();
}
template<typename T, typename Allocator>
typename moving_vector<T, Allocator>::indirect_iterator moving_end(moving_vector<T, Allocator> && container) {
	return std::move(container).indirect_end();
}

template<typename T, typename Allocator>
void swap(moving_vector<T, Allocator> & lhs, moving_vector<T, Allocator> & rhs) noexcept {
	lhs.swap(rhs);
}

template<typename T, typename Allocator>
bool operator!=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, typename Allocator>
bool operator>(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename T, typename Allocator>
bool operator<=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, typename Allocator>
bool operator>=(moving_vector<T, Allocator> const & lhs, moving_vector<T, Allocator> const & rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace containers
#endif	// CONTAINERS_MOVING_VECTOR_HPP_
