// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.linked_list_helper;

export import containers.common_iterator_functions;
import containers.maximum_array_size;

import bounded;
import std_module;

namespace containers {

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
export template<typename Links, typename T>
struct linked_list_node : Links {
	constexpr explicit linked_list_node(bounded::construct_function_for<T> auto && constructor):
		value(OPERATORS_FORWARD(constructor)())
	{
	}
	[[no_unique_address]] T value;
};

export template<typename Node>
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

export template<typename Node>
constexpr auto destroy_node(Node * node) -> void {
	bounded::destroy(*node);
	std::allocator<Node>().deallocate(node, 1);
}

export template<typename Links>
concept has_previous = requires(Links const links) { links.previous; };

export template<typename Container, typename Links, typename T>
struct list_iterator {
	friend Container;

	// ???
	using difference_type = bounded::integer<
		-maximum_array_size<T>,
		maximum_array_size<T>
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

} // namespace containers
