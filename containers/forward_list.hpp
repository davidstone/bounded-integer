// An implementation of forward_list based on value_ptr
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

#ifndef CONTAINERS_FORWARD_LIST_HPP_
#define CONTAINERS_FORWARD_LIST_HPP_

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>
#include <value_ptr/value_ptr.hpp>

namespace containers {
namespace detail {
namespace fl {

template<typename T, typename Allocator>
class node;
template<typename T, typename Allocator>
class node_link;

template<typename T, typename Allocator>
class node_cloner {
public:
	node_link<T, Allocator> * operator()(node_link<T, Allocator> const * const other) const {
		typedef node<T, Allocator> node_type;
		return new node_type(static_cast<node_type const &>(*other));
	}
};

template<typename T, typename Allocator>
class node_deleter {
public:
	void operator()(node_link<T, Allocator> * ptr) const noexcept {
		delete static_cast<node<T, Allocator> *>(ptr);
	}
};

template<typename T, typename Allocator>
class node_link {
public:
	typedef node_cloner<T, Allocator> cloner_type;
	typedef node_deleter<T, Allocator> deleter_type;
	typedef smart_pointer::value_ptr<node_link, cloner_type, deleter_type> ptr_type;
	ptr_type next;
};

template<typename T, typename Allocator>
class node : public node_link<T, Allocator> {
public:
	template<typename... Args>
	explicit node(Args&&... args):
		value(std::forward<Args>(args)...) {
	}
	T value;
};

}	// namespace fl
}	// namespace detail

template<typename T, typename Allocator = std::allocator<T>>
class forward_list {
public:
	typedef T value_type;
	typedef Allocator allocator_type;
private:
	typedef detail::fl::node_link<value_type, allocator_type> node_link;
	typedef detail::fl::node<value_type, allocator_type> node;
	typedef typename node_link::ptr_type ptr_type;
public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type & reference;
	typedef value_type const & const_reference;
	typedef typename std::allocator_traits<allocator_type>::pointer pointer;
	typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;

	class iterator {
	public:
		typedef forward_list::value_type value_type;
		typedef forward_list::difference_type difference_type;
		typedef forward_list::pointer pointer;
		typedef forward_list::reference reference;
		typedef std::forward_iterator_tag iterator_category;

		constexpr iterator() noexcept:
			m_value(nullptr) {
		}
		iterator & operator++() {
			m_value = next_ptr().get();
			return *this;
		}
		iterator operator++(int) {
			auto temp = *this;
			operator++();
			return temp;
		}
		reference operator*() const {
			return static_cast<node *>(m_value)->value;
		}
		pointer operator->() const {
			return & operator*();
		}
		friend constexpr bool operator==(iterator lhs, iterator rhs) noexcept {
			return lhs.m_value == rhs.m_value;
		}
		friend constexpr bool operator!=(iterator lhs, iterator rhs) noexcept {
			return !(lhs == rhs);
		}
	private:
		friend class const_iterator;
		friend class forward_list;
		explicit constexpr iterator(node_link * other) noexcept:
			m_value(other) {
		}
		ptr_type const & next_ptr() const {
			assert(m_value != nullptr);
			return m_value->next;
		}
		ptr_type & next_ptr() {
			assert(m_value != nullptr);
			return m_value->next;
		}
		node_link * m_value;
	};
	class const_iterator {
	public:
		typedef forward_list::value_type const value_type;
		typedef forward_list::difference_type difference_type;
		typedef forward_list::const_pointer pointer;
		typedef forward_list::const_reference reference;
		typedef std::forward_iterator_tag iterator_category;
		constexpr const_iterator() noexcept:
			m_value(nullptr) {
		}
		constexpr const_iterator(iterator const other) noexcept:
			m_value(other.m_value) {
		}
		const_iterator & operator++() {
			m_value = next_ptr().get();
			return *this;
		}
		const_iterator operator++(int) {
			auto temp = *this;
			operator++();
			return temp;
		}
		reference operator*() const {
			return static_cast<node const *>(m_value)->value;
		}
		pointer operator->() const {
			return & operator*();
		}
		friend constexpr bool operator==(const_iterator lhs, const_iterator rhs) noexcept {
			return lhs.m_value == rhs.m_value;
		}
		friend constexpr bool operator!=(const_iterator lhs, const_iterator rhs) noexcept {
			return !(lhs == rhs);
		}
	private:
		explicit constexpr const_iterator(node_link const * other) noexcept:
			m_value(other) {
		}
		friend class forward_list;
		explicit operator iterator() const noexcept {
			// The only time this is called is on a container to which someone
			// has gotten an iterator (as opposed to a const_iterator), which
			// means that the container is non-const, which means that m_value
			// ultimately points to an object that is non-const, so this
			// const_cast does not invoke undefined behavior.
			return iterator(const_cast<node_link *>(m_value));
		}
		ptr_type const & next_ptr() const {
			assert(m_value != nullptr);
			return m_value->next;
		}

		node_link const * m_value;
	};


	explicit forward_list(allocator_type const & alloc = allocator_type()) { }
	explicit forward_list(size_type count, allocator_type const & alloc = allocator_type()) {
		for (size_type n = 0; n != count; ++n) {
			emplace_front();
		}
	}
	forward_list(size_type count, value_type const & value, allocator_type const & alloc = allocator_type()) {
		for (size_type n = 0; n != count; ++n) {
			emplace_front(value);
		}
	}
	template<typename InputIterator>
	forward_list(InputIterator first, InputIterator last, allocator_type const & alloc = allocator_type()) {
		emplace_front(*first);
		auto previous = begin();
		++first;
		for ( ; first != last; ++first) {
			previous = emplace_after(previous, *first);
		}
	}
	forward_list(forward_list const & other, allocator_type const & alloc):
		forward_list(other) {
	}
	forward_list(forward_list && other, allocator_type const & alloc):
		forward_list(std::move(other)) {
	}
	forward_list(std::initializer_list<value_type> init, allocator_type const & alloc = allocator_type()):
		forward_list(std::begin(init), std::end(init), alloc) {
	}

	// The default copy constructor, copy assignment operator, and destructor
	// give the correct behavior on an abstract C++ machine, but in reality this
	// implementation makes recursive calls that cause a stack overflow for
	// large lists. This implementation is theoretically slightly slower because
	// it involves setting pointers to nullptr for the destructor and
	// initializing an empty first node for the copy constructor, but the
	// optimizer should take care of that.

	forward_list(forward_list const & other):
		forward_list(std::begin(other), std::end(other)) {
	}
	forward_list(forward_list && other) = default;
	forward_list & operator=(forward_list const & other) {
		assign(std::begin(other), std::end(other));
	}
	forward_list & operator=(forward_list && other) = default;

	~forward_list() noexcept {
		clear();
	}
	
	void assign(size_type count, value_type const & value) {
		*this = forward_list(count, value);
	}
	template<typename InputIterator>
	void assign(InputIterator first, InputIterator last) {
		*this = forward_list(first, last);
	}
	void assign(std::initializer_list<value_type> init) {
		*this = forward_list(init);
	}
	
	
	const_reference front() const {
		return static_cast<node const &>(*m_before_first.next).value;
	}
	reference front() {
		auto const & self = *this;
		return const_cast<reference>(self.front());
	}
	
	const_iterator before_begin() const noexcept {
		return const_iterator(& m_before_first);
	}
	iterator before_begin() noexcept {
		return iterator(& m_before_first);
	}
	const_iterator cbefore_begin() const noexcept {
		return before_begin();
	}
	
	const_iterator begin() const noexcept {
		return ++before_begin();
	}
	iterator begin() noexcept {
		return ++before_begin();
	}
	const_iterator cbegin() const noexcept {
		return begin();
	}
	
	const_iterator end() const noexcept {
		return const_iterator();
	}
	iterator end() noexcept {
		return iterator();
	}
	const_iterator cend() const noexcept {
		return end();
	}
	
	bool empty() const noexcept {
		return begin() == end();
	}
	
	size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	
	void clear() noexcept {
		while (!empty()) {
			pop_front();
		}
	}
	
	template<typename... Args>
	iterator emplace_after(const_iterator position, Args&&... args) {
		auto ptr = smart_pointer::make_value_general<node>(typename node_link::cloner_type{}, typename node_link::deleter_type{}, std::forward<Args>(args)...);
		return insert_pointer_after(position, std::move(ptr));
	}
	template<typename... Args>
	void emplace_front(Args&&... args) {
		emplace_after(before_begin(), std::forward<Args>(args)...);
	}
	iterator insert_after(const_iterator position, value_type const & value) {
		return emplace_after(position, value);
	}
	iterator insert_after(const_iterator position, value_type && value) {
		return emplace_after(position, std::move(value));
	}
	iterator insert_after(const_iterator position, size_type count, value_type const & value) {
		for (size_type n = 0; n != count; ++n) {
			position = emplace_after(position, value);
		}
		return static_cast<iterator>(position);
	}
	template<typename InputIterator>
	iterator insert_after(const_iterator position, InputIterator first, InputIterator last) {
		for ( ; first != last; ++first) {
			position = emplace_after(position, *first);
		}
		return static_cast<iterator>(position);
	}
	iterator insert_after(const_iterator const position, std::initializer_list<value_type> init) {
		return insert_after(position, std::begin(init), std::end(init));
	}
	void push_front(value_type const & value) {
		emplace_front(value);
	}
	void push_front(value_type && value) {
		emplace_front(std::move(value));
	}
	
	iterator erase_after(const_iterator const position) {
		auto it = static_cast<iterator>(position);
		unlink_node_after(it);
		return ++it;
	}
	iterator erase_after(const_iterator first, const_iterator const last) {
		while (first != last) {
			first = erase_after(first);
		}
		return last;
	}
	void pop_front() {
		erase_after(before_begin());
	}
	
	void resize(size_type const count) {
		resize_helper(count);
	}
	void resize(size_type const count, value_type const & value) {
		resize_helper(count, value);
	}
	
	void swap(forward_list & other) noexcept {
		using std::swap;
		swap(m_before_first, other.m_before_first);
	}
	
	template<typename Compare>
	void merge(forward_list & other, Compare compare = std::less<value_type>()) {
		auto mine = before_begin();
		auto mine_next = std::next(mine);
		auto theirs = other.before_begin();
		auto theirs_next = std::next(theirs);
		while (mine_next != end() and theirs_next != other.end()) {
			if (compare(*mine_next, *theirs_next)) {
				++mine;
				++mine_next;
			}
			else {
				mine = insert_pointer_after(mine, unlink_node_after(theirs));
				mine_next = std::next(mine);
				++theirs;
				theirs_next = std::next(theirs);
			}
		}
		if (mine_next == end()) {
			insert_after(mine, theirs, std::end(other));
		}
	}
	template<typename Compare>
	void merge(forward_list && other, Compare compare = std::less<value_type>()) {
		merge(other, compare);
	}
	
	void splice_after(const_iterator position, forward_list & other) {
		auto it = static_cast<iterator>(position);
		auto other_last = other.before_end();
		auto next = it.next_ptr().release();
		using std::swap;
		swap(it.next_ptr(), other.before_begin().next_ptr());
		other_last.next_ptr().reset(next);
	}
	void splice_after(const_iterator position, forward_list && other) {
		splice_after(position, other);
	}
	void splice_after(const_iterator position, forward_list & other, const_iterator it) {
		insert_pointer_after(position, unlink_node_after(static_cast<iterator>(it)));
	}
	void splice_after(const_iterator position, forward_list && other, const_iterator it) {
		splice_after(position, other, it);
	}
	void splice_after(const_iterator position, forward_list & other, const_iterator first, const_iterator last) {
		if (first == last) {
			return;
		}
		auto it = std::next(first);
		if (it == last) {
			return;
		}
		while (std::next(it) != last) {
			++it;
		}
		splice_after(position, unlink_node_after(static_cast<iterator>(first)), it.next_ptr());
	}
	void splice_after(const_iterator position, forward_list && other, const_iterator first, const_iterator last) {
		splice_after(position, other, first, last);
	}
	
	template<typename UnaryPredicate>
	void remove_if(UnaryPredicate p) {
		for (auto it = before_begin(); std::next(it) != end(); ++it) {
			if (p(*it.next_ptr())) {
				erase_after(it);
			}
		}
	}
	void remove(value_type const & value) {
		remove_if([&](value_type const & other) {
			return other == value;
		});
	}
	
	// should this be noexcept?
	void reverse() {
		forward_list temp;
		while (before_begin().next_ptr() != nullptr) {
			temp.insert_pointer_after(temp.before_begin(), unlink_node_after(before_begin()));
		}
		swap(temp);
	}
	
	template<typename BinaryPredicate>
	void unique(BinaryPredicate p = std::equal_to<value_type>()) {
		for (auto it = before_begin(); std::next(it) != end(); ++it) {
			while (p(*it, *std::next(it))) {
				erase_after(it);
			}
		}
	}
	
	template<typename Compare>
	void sort(Compare compare) {
		// I cheat here by simply deferring to std::sort on a vector of pointers
		// to the value, and then I re-link the nodes. I will have to test this,
		// as it may actually give surprisingly good performance.
		std::vector<ptr_type> temp;
		temp.reserve(size());
		while (!empty()) {
			temp.emplace_back(unlink_node_after(before_begin()));
		}
		std::sort(std::begin(temp), std::end(temp), [](ptr_type const & lhs, ptr_type const & rhs) {
			return static_cast<node const &>(*lhs).value < static_cast<node const &>(*rhs).value;
		});
		auto it = before_begin();
		for (auto & ptr : temp) {
			it = insert_pointer_after(it, std::move(ptr));
		}
	}
	void sort() {
		sort(std::less<value_type>{});
	}
	
	friend bool operator==(forward_list const & lhs, forward_list const & rhs) {
		auto lhs_it = lhs.begin();
		auto rhs_it = rhs.begin();
		while (lhs_it != lhs.end() and rhs_it != rhs.end()) {
			if (*lhs_it != *rhs_it) {
				return false;
			}
			++lhs_it;
			++rhs_it;
		}
		return lhs_it == lhs.end() and rhs_it == rhs.end();
	}
	friend bool operator<(forward_list const & lhs, forward_list const & rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
private:
	// This is here because it simplifies my current poor implementation of sort
	size_type size() const noexcept {
		return static_cast<size_type>(std::distance(begin(), end()));
	}
	const_iterator before_end() const {
		const_iterator it = begin();
		while (it.next_ptr() != nullptr) {
			++it;
		}
		return it;
	}
	iterator before_end() {
		const_iterator it = begin();
		while (it.next_ptr() != nullptr) {
			++it;
		}
		return it;
	}
	iterator insert_pointer_after(const_iterator position, ptr_type value) {
		auto it = static_cast<iterator>(position);
		splice_after(position, value, value);
		return std::next(it);
	}
	// last can reference the same pointer as first. Any values that last links
	// to are destroyed. first == nullptr after this function returns. first and
	// last are included
	static void splice_after(const_iterator position, ptr_type & first, ptr_type & last) {
		auto it = static_cast<iterator>(position);
		assert(position.m_value != nullptr);
		assert(first != nullptr);
		assert(last != nullptr);
		last->next.reset(it.next_ptr().release());
		it.next_ptr().reset(first.release());
	}
	static ptr_type unlink_node_after(iterator it) {
		auto pointer = it.next_ptr().release();
		using std::swap;
		swap(it.next_ptr(), pointer->next);
		return ptr_type(pointer);
	}
	template<typename... Args>
	void resize_helper(size_type count, Args&&... args) {
		auto it = before_begin();
		for (size_type n = 0; n != count; ++n) {
			if (it.next_ptr() != nullptr) {
				++it;
			}
			else {
				it = emplace_after(it, std::forward<Args>(args)...);
			}
		}
		it.next_ptr().reset();
	}

	node_link m_before_first;
};

template<typename T, typename Allocator>
bool operator!=(forward_list<T, Allocator> const & lhs, forward_list<T, Allocator> const & rhs) {
	return !(lhs == rhs);
}
template<typename T, typename Allocator>
bool operator>(forward_list<T, Allocator> const & lhs, forward_list<T, Allocator> const & rhs) {
	return rhs < lhs;
}
template<typename T, typename Allocator>
bool operator>=(forward_list<T, Allocator> const & lhs, forward_list<T, Allocator> const & rhs) {
	return !(lhs < rhs);
}
template<typename T, typename Allocator>
bool operator<=(forward_list<T, Allocator> const & lhs, forward_list<T, Allocator> const & rhs) {
	return !(lhs > rhs);
}

template<typename T, typename Allocator>
void swap(forward_list<T, Allocator> & lhs, forward_list<T, Allocator> & rhs) noexcept {
	lhs.swap(rhs);
}

}	// namespace containers
#endif	// CONTAINERS_FORWARD_LIST_HPP_
