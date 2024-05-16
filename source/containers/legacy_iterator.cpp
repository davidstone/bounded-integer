// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

module;

#include <bounded/assert.hpp>
#include <operators/arrow.hpp>
#include <operators/bracket.hpp>

export module containers.legacy_iterator;

import containers.bidirectional_iterator;
export import containers.common_iterator_functions;
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
import tv;

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

template<typename T>
struct non_optional {
	non_optional(tv::none_t) {
		std::unreachable();
	}

	template<bounded::explicitly_convertible_to<T> Value = T>
	constexpr explicit(!bounded::convertible_to<Value, T>) non_optional(Value && other):
		m_value(OPERATORS_FORWARD(other))
	{
	}
	
	constexpr auto operator*() const & -> T const & {
		return m_value;
	}
	constexpr auto operator*() & -> T & {
		return m_value;
	}
	constexpr auto operator*() && -> T && {
		return std::move(m_value);
	}
	constexpr explicit operator bool() const {
		return true;
	}

	friend auto operator<=>(non_optional const &, non_optional const &) = default;
private:
	[[no_unique_address]] T m_value;
};

export template<typename Iterator>
struct legacy_iterator {
	using iterator_category = typename iterator_category_impl<Iterator>::type;
private:
	static constexpr auto always_exists = bounded::default_constructible<Iterator> or !std::derived_from<iterator_category, std::forward_iterator_tag>;
public:
	using value_type = iter_value_t<Iterator>;
	using difference_type = std::ptrdiff_t;
	using reference = iter_reference_t<Iterator>;
	using pointer = std::add_pointer_t<reference>;

	legacy_iterator() = default;
	constexpr legacy_iterator(Iterator it):
		m_it(std::move(it)) {
	}
	template<bounded::convertible_to<Iterator> It>
	constexpr legacy_iterator(legacy_iterator<It> other):
		m_it(other.m_it ? storage(*std::move(other).m_it) : tv::none)
	{
	}
	
	constexpr auto base() const requires always_exists {
		return *m_it;
	}

	constexpr auto operator*() const -> decltype(auto) {
		return **m_it;
	}
	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	constexpr auto to_address() const requires to_addressable<Iterator> {
		return m_it ? containers::to_address(*m_it) : nullptr;
	}

	friend auto operator<=>(legacy_iterator, legacy_iterator) = default;

	friend constexpr auto operator+(legacy_iterator const lhs, bounded::integral auto const rhs) -> legacy_iterator requires iterator_addable<Iterator, iter_difference_t<Iterator>> {
		return rhs == 0_bi ?
			lhs :
			legacy_iterator<Iterator>(*lhs.m_it + ::bounded::assume_in_range<iter_difference_t<Iterator>>(rhs));
	}
	friend constexpr auto operator++(legacy_iterator & it) -> legacy_iterator & {
		++*it.m_it;
		return it;
	}

	friend constexpr auto operator-(legacy_iterator const lhs, legacy_iterator const rhs) requires subtractable<Iterator> {
		BOUNDED_ASSERT(static_cast<bool>(lhs.m_it) == static_cast<bool>(rhs.m_it));
		return lhs.m_it ? static_cast<typename legacy_iterator<Iterator>::difference_type>(*lhs.m_it - *rhs.m_it) : 0;
	}
	friend constexpr auto operator--(legacy_iterator & it) -> legacy_iterator & {
		--*it.m_it;
		return it;
	}

private:
	template<typename I>
	friend struct legacy_iterator;

	using storage = std::conditional_t<always_exists, non_optional<Iterator>, tv::optional<Iterator>>;
	storage m_it;
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
