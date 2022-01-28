// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/assign.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/initializer_range.hpp>
#include <containers/is_container.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/range_value_t.hpp>

#include <operators/forward.hpp>

#include <concepts>
#include <span>
#include <type_traits>
#include <utility>

namespace containers {

// Traditionally, "next" on the last element of a linked list is a nullptr. This
// implementation is a little more complicated than that because we want:
// * a noexcept default constructor
// * a noexcept move constructor
// * a noexcept move assignment operator
// * stability of the end iterator except for move and swap
//
// This implementation also separates out the concept of the links of the node
// (`previous` and `next`) from the data because we store the sentinel links
// directly in the list, and we do not want to increase the stack storage costs
// by `sizeof(T)`.
//
// Every instance of `list_node_links` has a `list_node` that derives from it,
// except for the sentinel stored directly in the list.

template<typename T>
struct bidirectional_linked_list;

namespace detail {

struct list_node_links {
	constexpr list_node_links() noexcept:
		previous(this),
		next(this)
	{
	}
	list_node_links(list_node_links const &) = delete;
	auto operator=(list_node_links const &) = delete;

	list_node_links * previous;
	list_node_links * next;
};

constexpr auto unlink_range(list_node_links * const first, list_node_links * const last) -> void {
	auto const before_first = first->previous;
	before_first->next = last;
	last->previous = before_first;
}

constexpr auto link_range(list_node_links * const original_first, list_node_links * const additional_first, list_node_links * const additional_before_last, list_node_links * const original_last) -> void {
	BOUNDED_ASSERT(original_first->next == original_last);
	BOUNDED_ASSERT(original_last->previous == original_first);
	original_first->next = additional_first;
	additional_first->previous = original_first;
	additional_before_last->next = original_last;
	original_last->previous = additional_before_last;
}


template<typename T>
struct list_node : list_node_links {
	constexpr explicit list_node(bounded::construct_function_for<T> auto && constructor):
		value(OPERATORS_FORWARD(constructor)())
	{
	}
	[[no_unique_address]] T value;
};

template<typename T, bool is_const>
struct list_iterator {
private:
	using links_ptr = std::conditional_t<is_const, list_node_links const *, list_node_links *>;
public:
	friend bidirectional_linked_list<T>;

	// ???
	using difference_type = array_size_type<T>;

	list_iterator() = default;
	constexpr explicit list_iterator(links_ptr const links):
		m_links(links)
	{
	}

	constexpr operator list_iterator<T, true>() const {
		return list_iterator<T, true>(m_links);
	}

	constexpr auto & operator*() const {
		using node_ptr = std::conditional_t<is_const, list_node<T> const *, list_node<T> *>;
		return static_cast<node_ptr>(m_links)->value;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend auto operator<=>(list_iterator, list_iterator) = default;

	friend constexpr auto operator+(list_iterator const it, bounded::constant_t<1>) {
		return list_iterator<T, is_const>(it.m_links->next);
	}
	friend constexpr auto operator-(list_iterator const it, bounded::constant_t<1>) {
		return list_iterator<T, is_const>(it.m_links->previous);
	}
private:
	links_ptr m_links = nullptr;
};

// Defined to prevent binding to temporaries
template<typename T>
constexpr auto & as_mutable(T & value) {
	return const_cast<std::remove_const_t<T> &>(value);
}
template<typename T>
constexpr auto as_mutable(T const * value) -> T * {
	return const_cast<T *>(value);
}

} // namespace detail

template<typename T>
struct bidirectional_linked_list : private lexicographical_comparison::base {
	using const_iterator = detail::list_iterator<T, true>;
	using iterator = detail::list_iterator<T, false>;

	constexpr bidirectional_linked_list() = default;

	constexpr explicit bidirectional_linked_list(initializer_range<bidirectional_linked_list> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr bidirectional_linked_list(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr bidirectional_linked_list(Source) {
	}

	constexpr bidirectional_linked_list(bidirectional_linked_list && other) noexcept {
		swap(*this, other);
	}

	constexpr bidirectional_linked_list(bidirectional_linked_list const & other) {
		::containers::assign_to_empty(*this, other);
	}

	constexpr ~bidirectional_linked_list() noexcept {
		::containers::clear(*this);
	}

	constexpr auto operator=(bidirectional_linked_list && other) & noexcept -> bidirectional_linked_list & {
		swap(*this, other);
		return *this;
	}
	constexpr auto operator=(bidirectional_linked_list const & other) & -> bidirectional_linked_list & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}

	friend constexpr auto swap(bidirectional_linked_list & lhs, bidirectional_linked_list & rhs) noexcept -> void {
		std::swap(lhs.m_sentinel.previous, rhs.m_sentinel.previous);
		std::swap(lhs.m_sentinel.next, rhs.m_sentinel.next);
		constexpr auto link_in = [](detail::list_node_links & node, detail::list_node_links const & other) {
			if (node.previous == std::addressof(other)) {
				BOUNDED_ASSERT(node.next == std::addressof(other));
				node.previous = std::addressof(node);
				node.next = std::addressof(node);
			} else {
				BOUNDED_ASSERT(node.next != std::addressof(other));
				node.previous->next = std::addressof(node);
				node.next->previous = std::addressof(node);
			}
		};
		link_in(lhs.m_sentinel, rhs.m_sentinel);
		link_in(rhs.m_sentinel, lhs.m_sentinel);
	}

	constexpr auto begin() const -> const_iterator {
		return const_iterator(m_sentinel.next);
	}
	constexpr auto begin() -> iterator {
		return iterator(m_sentinel.next);
	}
	constexpr auto end() const -> const_iterator {
		return const_iterator(std::addressof(m_sentinel));
	}
	constexpr auto end() -> iterator {
		return iterator(std::addressof(m_sentinel));
	}

	constexpr auto mutable_iterator(const_iterator const it) & -> iterator {
		return iterator(as_mutable(it.m_links));
	}

	constexpr auto lazy_push_back(bounded::construct_function_for<T> auto && constructor) & -> T & {
		auto ptr = allocate_node();
		try {
			bounded::construct(*ptr, [&] {
				return detail::list_node<T>(OPERATORS_FORWARD(constructor));
			});
		} catch (...) {
			deallocate_node(ptr);
		}
		link_range(end().m_links->previous, ptr, ptr, end().m_links);
		return ptr->value;
	}
	constexpr auto pop_back() & -> void {
		auto const it = containers::prev(end());
		unlink_range(it.m_links, end().m_links);
		auto const ptr = static_cast<detail::list_node<T> *>(it.m_links);
		bounded::destroy(*ptr);
		deallocate_node(ptr);
	}

	constexpr auto splice(const_iterator const position, bidirectional_linked_list &, const_iterator const first, const_iterator const last) & -> void {
		if (first == last) {
			return;
		}
		auto const first_links = ::containers::detail::as_mutable(first.m_links);
		auto const last_links = ::containers::detail::as_mutable(last.m_links);
		auto const position_links = ::containers::detail::as_mutable(position.m_links);
		auto const before_last_links = last_links->previous;
		auto const before_position_links = position_links->previous;
		unlink_range(first_links, last_links);
		link_range(before_position_links, first_links, before_last_links, position_links);
	}
	constexpr auto splice(const_iterator const position, bidirectional_linked_list & other) & -> void {
		splice(position, other, other.begin(), other.end());
	}
	constexpr auto splice(const_iterator const position, bidirectional_linked_list && other) & -> void {
		splice(position, other, other.begin(), other.end());
	}

private:
	static constexpr auto allocator() -> std::allocator<detail::list_node<T>> {
		return {};
	}
	static constexpr auto allocate_node() -> detail::list_node<T> * {
		return allocator().allocate(1);
	}
	static constexpr auto deallocate_node(detail::list_node<T> * ptr) -> void {
		allocator().deallocate(ptr, 1);
	}

	detail::list_node_links m_sentinel;
};

template<typename Range>
bidirectional_linked_list(Range &&) -> bidirectional_linked_list<std::decay_t<range_value_t<Range>>>;

template<typename T>
inline constexpr auto is_container<bidirectional_linked_list<T>> = true;

} // namespace containers
