// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.bidirectional_linked_list;

import containers.algorithms.advance;
import containers.as_mutable_ptr;
import containers.assign;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.clear;
import containers.compare_container;
import containers.initializer_range;
import containers.linked_list_helper;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

struct bidirectional_links {
	constexpr bidirectional_links() noexcept:
		previous(this),
		next(this)
	{
	}
	bidirectional_links(bidirectional_links &&) = delete;
	bidirectional_links(bidirectional_links const &) = delete;
	auto operator=(bidirectional_links &&) = delete;
	auto operator=(bidirectional_links const &) = delete;

	bidirectional_links * previous;
	bidirectional_links * next;
};

constexpr auto unlink_range(bidirectional_links * const first, bidirectional_links * const last) -> void {
	auto const before_first = first->previous;
	before_first->next = last;
	last->previous = before_first;
}

constexpr auto link_range(bidirectional_links * const original_first, bidirectional_links * const additional_first, bidirectional_links * const additional_before_last, bidirectional_links * const original_last) -> void {
	BOUNDED_ASSERT(original_first->next == original_last);
	BOUNDED_ASSERT(original_last->previous == original_first);
	original_first->next = additional_first;
	additional_first->previous = original_first;
	additional_before_last->next = original_last;
	original_last->previous = additional_before_last;
}

export template<typename T>
struct bidirectional_linked_list : private lexicographical_comparison::base {
	using const_iterator = list_iterator<bidirectional_linked_list, bidirectional_links const, T>;
	using iterator = list_iterator<bidirectional_linked_list, bidirectional_links, T>;

	constexpr bidirectional_linked_list() = default;

	constexpr explicit bidirectional_linked_list(constructor_initializer_range<bidirectional_linked_list> auto && source) {
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
		constexpr auto link_in = [](bidirectional_links & node, bidirectional_links const & other) {
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
		return iterator(::containers::as_mutable_ptr(it.m_links));
	}

	constexpr auto lazy_push_back(bounded::construct_function_for<T> auto && constructor) & -> T & {
		auto ptr = ::containers::make_node<node_t>(OPERATORS_FORWARD(constructor));
		link_range(end().m_links->previous, ptr, ptr, end().m_links);
		return ptr->value;
	}
	constexpr auto pop_back() & -> void {
		auto const it = containers::prev(end());
		unlink_range(it.m_links, end().m_links);
		auto const ptr = static_cast<node_t *>(it.m_links);
		::containers::destroy_node(ptr);
	}

	constexpr auto splice(const_iterator const position, bidirectional_linked_list &, const_iterator const first, const_iterator const last) & -> void {
		if (first == last) {
			return;
		}
		auto const first_links = ::containers::as_mutable_ptr(first.m_links);
		auto const last_links = ::containers::as_mutable_ptr(last.m_links);
		auto const position_links = ::containers::as_mutable_ptr(position.m_links);
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
	using node_t = linked_list_node<bidirectional_links, T>;
	bidirectional_links m_sentinel;
};

template<typename Range>
bidirectional_linked_list(Range &&) -> bidirectional_linked_list<std::decay_t<range_value_t<Range>>>;

} // namespace containers
