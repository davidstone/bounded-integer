// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

module;

#include <operators/arrow.hpp>
#include <operators/bracket.hpp>

export module containers.legacy_iterator;

import containers.bidirectional_iterator;
import containers.common_iterator_functions;
import containers.contiguous_iterator;
import containers.forward_iterator;
import containers.iterator;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.iter_value_t;
import containers.iterator_addable;
import containers.random_access_iterator;
import containers.subtractable;
import containers.to_address;

import bounded;
import operators;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Iterator>
struct iterator_category_impl {
	using type = typename Iterator::iterator_category;
};

template<forward_iterator Iterator>
struct iterator_category_impl<Iterator> {
	using type =
		std::conditional_t<random_access_iterator<Iterator>, std::random_access_iterator_tag,
		std::conditional_t<bidirectional_iterator<Iterator>, std::bidirectional_iterator_tag,
		std::forward_iterator_tag
	>>;
};

export template<typename Iterator>
struct legacy_iterator {
	using value_type = iter_value_t<Iterator>;
	using difference_type = std::ptrdiff_t;
	using reference = iter_reference_t<Iterator>;
	using pointer = std::add_pointer_t<reference>;
	using iterator_category = typename iterator_category_impl<Iterator>::type;

	legacy_iterator() = default;
	constexpr legacy_iterator(Iterator it):
		m_it(std::move(it)) {
	}
	template<bounded::convertible_to<Iterator> It>
	constexpr legacy_iterator(legacy_iterator<It> other):
		m_it(other.base())
	{
	}
	
	constexpr auto base() const {
		return m_it;
	}

	constexpr decltype(auto) operator*() const {
		return *base();
	}
	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	constexpr auto to_address() const requires to_addressable<Iterator> {
		return containers::to_address(m_it);
	}

	friend auto operator<=>(legacy_iterator, legacy_iterator) = default;

	friend constexpr auto operator+(legacy_iterator const lhs, bounded::integral auto const rhs) requires iterator_addable<Iterator, iter_difference_t<Iterator>> {
		if constexpr (std::same_as<iter_difference_t<Iterator>, bounded::constant_t<0>>) {
			std::unreachable();
			return lhs;
		} else {
			return legacy_iterator<Iterator>(lhs.base() + ::bounded::assume_in_range<iter_difference_t<Iterator>>
			(rhs));
		}
	}
	friend constexpr auto operator++(legacy_iterator & it) -> legacy_iterator & {
		++it.m_it;
		return it;
	}

	friend constexpr auto operator-(legacy_iterator const lhs, legacy_iterator const rhs) requires subtractable<Iterator> {
		return static_cast<typename legacy_iterator<Iterator>::difference_type>(lhs.base() - rhs.base());
	}
	friend constexpr auto operator--(legacy_iterator & it) -> legacy_iterator & {
		--it.m_it;
		return it;
	}

private:
	Iterator m_it;
};

export constexpr auto make_legacy_iterator = []<iterator Iterator>(Iterator it) {
	return containers::legacy_iterator<Iterator>(std::move(it));
};

export constexpr auto maybe_legacy_iterator = []<iterator Iterator>(Iterator it) {
	if constexpr (bounded::bounded_integer<iter_difference_t<Iterator>>) {
		return containers::legacy_iterator(std::move(it));
	} else {
		return it;
	}
};

} // namespace containers

static_assert(bounded::convertible_to<containers::legacy_iterator<int const *>, containers::legacy_iterator<int const *>>);
static_assert(bounded::convertible_to<containers::legacy_iterator<int *>, containers::legacy_iterator<int *>>);
static_assert(bounded::convertible_to<containers::legacy_iterator<int *>, containers::legacy_iterator<int const *>>);
static_assert(!bounded::convertible_to<containers::legacy_iterator<int const *>, containers::legacy_iterator<int *>>);

template<typename T>
concept incrementable = requires(T value) { ++value; };

static_assert(incrementable<containers::legacy_iterator<int *>>);
static_assert(incrementable<containers::legacy_iterator<containers::contiguous_iterator<int, 1_bi>>>);
static_assert(incrementable<containers::contiguous_iterator<int, 0_bi>>);
static_assert(incrementable<containers::legacy_iterator<containers::contiguous_iterator<int, 0_bi>>>);
