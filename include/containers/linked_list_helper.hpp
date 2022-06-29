// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/maximum_array_size.hpp>

#include <bounded/concepts.hpp>
#include <bounded/detail/construct_destroy.hpp>
#include <bounded/lazy_init.hpp>

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

#include <compare>
#include <memory>
#include <type_traits>

namespace containers::detail {

template<typename T>
constexpr auto & as_mutable(T & value) {
	return const_cast<std::remove_const_t<T> &>(value);
}
template<typename T>
constexpr auto as_mutable(T const * value) -> T * {
	return const_cast<T *>(value);
}

// We separate out the links of the node (`previous` and `next`) from the data
// because we store the sentinel links directly in the list, allowing:
// * a non-allocating, noexcept default constructor
// * a non-allocating, noexcept move constructor
// * a non-allocating, noexcept move assignment operator
// * stability of the end iterator (where applicable) except for move and swap
//
// Other approaches would increase the size of the list object by `sizeof(T)`
// and potentially not be usable in `constexpr`.
//
// Every instance of `Links` has a `linked_list_node` that derives from it,
// except for sentinels stored directly in the list.
template<typename Links, typename T>
struct linked_list_node : Links {
	constexpr explicit linked_list_node(bounded::construct_function_for<T> auto && constructor):
		value(OPERATORS_FORWARD(constructor)())
	{
	}
	[[no_unique_address]] T value;
};

template<typename Node>
constexpr auto make_node(bounded::explicitly_convertible_to<Node> auto && arg) -> Node * {
	auto allocator = std::allocator<Node>();
	auto node = allocator.allocate(1);
	try {
		bounded::construct_at(*node, [&] { return Node(OPERATORS_FORWARD(arg)); });
	} catch (...) {
		allocator.deallocate(node, 1);
		throw;
	}
	return node;
}

template<typename Node>
constexpr auto destroy_node(Node * node) -> void {
	bounded::destroy(*node);
	std::allocator<Node>().deallocate(node, 1);
}

template<typename Links>
concept has_previous = requires(Links const links) { links.previous; };

template<typename Container, typename Links, typename T>
struct list_iterator {
	friend Container;

	// ???
	using difference_type = bounded::integer<
		-detail::maximum_array_size<T>,
		detail::maximum_array_size<T>
	>;

	list_iterator() = default;
	constexpr explicit list_iterator(Links * const links):
		m_links(links)
	{
	}

	constexpr operator list_iterator<Container, Links const, T>() const {
		return list_iterator<Container, Links const, T>(m_links);
	}

	constexpr auto & operator*() const {
		using node_t = linked_list_node<std::remove_const_t<Links>, T>;
		using node_ptr = std::conditional_t<std::is_const_v<Links>, node_t const *, node_t *>;
		return static_cast<node_ptr>(m_links)->value;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend auto operator<=>(list_iterator, list_iterator) = default;

	friend constexpr auto operator+(list_iterator const it, bounded::constant_t<1>) {
		return list_iterator<Container, Links, T>(it.m_links->next);
	}
	friend constexpr auto operator-(list_iterator const it, bounded::constant_t<1>) requires has_previous<Links> {
		return list_iterator<Container, Links, T>(it.m_links->previous);
	}
private:
	Links * m_links = nullptr;
};

} // namespace containers::detail
