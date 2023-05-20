// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.forward_linked_list;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.as_mutable_ptr;
import containers.assign;
import containers.assign_to_empty;
import containers.c_array;
import containers.clear;
import containers.common_functions;
import containers.compare_container;
import containers.initializer_range;
import containers.linked_list_helper;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

struct forward_link {
	forward_link * next = nullptr;
};

export template<typename T>
struct [[clang::trivial_abi]] forward_linked_list : private lexicographical_comparison::base {
	using const_iterator = list_iterator<forward_linked_list, forward_link const, T>;
	using iterator = list_iterator<forward_linked_list, forward_link, T>;

	constexpr forward_linked_list() = default;

	constexpr explicit forward_linked_list(constructor_initializer_range<forward_linked_list> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr forward_linked_list(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr forward_linked_list(Source) {
	}

	constexpr forward_linked_list(forward_linked_list && other) noexcept {
		swap(*this, other);
	}

	constexpr forward_linked_list(forward_linked_list const & other) {
		::containers::assign_to_empty(*this, other);
	}

	constexpr ~forward_linked_list() noexcept {
		::containers::clear(*this);
	}

	constexpr auto operator=(forward_linked_list && other) & noexcept -> forward_linked_list & {
		swap(*this, other);
		return *this;
	}
	constexpr auto operator=(forward_linked_list const & other) & -> forward_linked_list & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}

	friend constexpr auto swap(forward_linked_list & lhs, forward_linked_list & rhs) noexcept -> void {
		std::swap(lhs.m_sentinel.next, rhs.m_sentinel.next);
	}

	constexpr auto before_begin() const -> const_iterator {
		return const_iterator(std::addressof(m_sentinel));
	}
	constexpr auto before_begin() -> iterator {
		return iterator(std::addressof(m_sentinel));
	}
	constexpr auto end() const -> const_iterator {
		return const_iterator(nullptr);
	}
	constexpr auto end() -> iterator {
		return iterator(nullptr);
	}

	constexpr auto mutable_iterator(const_iterator const it) & -> iterator {
		return iterator(::containers::as_mutable_ptr(it.m_links));
	}

	constexpr auto lazy_insert_after(const_iterator const before, bounded::construct_function_for<T> auto && constructor) & -> iterator {
		BOUNDED_ASSERT(before != end());
		auto ptr = containers::make_node<node_t>(OPERATORS_FORWARD(constructor));
		auto const mutable_before = mutable_iterator(before);
		ptr->next = mutable_before.m_links->next;
		mutable_before.m_links->next = ptr;
		return iterator(ptr);
	}
	constexpr auto erase_after(const_iterator const before) & -> iterator {
		BOUNDED_ASSERT(before != end());
		auto const mutable_before = mutable_iterator(before);
		auto const it = containers::next(mutable_before);
		auto const after = containers::next(it);
		mutable_before.m_links->next = after.m_links;
		auto const ptr = static_cast<node_t *>(it.m_links);
		destroy_node(ptr);
		return after;
	}

	// Unlike `std::forward_list`, the final iterator is before_last, not last.
	// This allows splice_after to operate in constant time.
	constexpr auto splice_after(const_iterator const before, forward_linked_list & other, const_iterator const before_first, const_iterator const before_last) & -> void {
		BOUNDED_ASSERT(this != std::addressof(other));
		BOUNDED_ASSERT(before_last != other.end());
		if (before_first == before_last) {
			return;
		}
		auto const mutable_before = other.mutable_iterator(before);
		auto const mutable_before_first = other.mutable_iterator(before_first);
		auto const mutable_before_last = other.mutable_iterator(before_last);
		auto const original_next = std::exchange(mutable_before.m_links->next, mutable_before_first.m_links->next);
		mutable_before_first.m_links->next = std::exchange(mutable_before_last.m_links->next, original_next);
	}

private:
	using node_t = linked_list_node<forward_link, T>;
	forward_link m_sentinel;
};

template<typename Range>
forward_linked_list(Range &&) -> forward_linked_list<std::decay_t<range_value_t<Range>>>;

} // namespace containers
