// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/compare_adl.hpp>
#include <containers/index_type.hpp>
#include <containers/operator_arrow.hpp>
#include <containers/reference_wrapper.hpp>
#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {

struct default_dereference {
	template<typename Iterator>
	static constexpr auto dereference(Iterator const & it) BOUNDED_NOEXCEPT_DECLTYPE(
		*it
	)
};

struct default_add {
	template<typename Iterator, typename Offset>
	static constexpr auto add(Iterator const & it, Offset const & offset) BOUNDED_NOEXCEPT_DECLTYPE(
		it + offset
	)
};

struct default_subtract {
	template<typename Iterator>
	static constexpr auto subtract(Iterator const & lhs, Iterator const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		lhs - rhs
	)
};

struct default_compare {
	template<typename LHS, typename RHS>
	static constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		::containers::detail::compare_adl::indirect_compare(lhs, rhs)
	)
};

namespace detail {

template<typename Iterator, typename Traits,
	typename = decltype(std::declval<Traits>().subtract(std::declval<Iterator>(), std::declval<Iterator>()))
>
constexpr auto is_random_access_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_random_access_iterator(Args...) {
	return false;
}

template<typename Iterator, typename Traits,
	typename = decltype(std::declval<Traits>().add(std::declval<Iterator>(), bounded::constant<1>)),
	typename = decltype(std::declval<Traits>().add(std::declval<Iterator>(), bounded::constant<-1>))
>
constexpr auto is_bidirectional_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_bidirectional_iterator(Args...) {
	return false;
}

template<typename Iterator, typename Traits,
	typename = decltype(std::declval<Traits>().add(std::declval<Iterator>(), bounded::constant<1>))
>
constexpr auto is_forward_iterator() {
	return true;
}
template<typename, typename, typename... Args>
constexpr auto is_forward_iterator(Args...) {
	return false;
}

}	// namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Add: it + 5_bi
// 3) Subtract: it1 - it2
// 4) Compare: compare(it1, it2)
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these four behaviors. Adding,
// subtracting, and comparing are all linked to the same algorithm, but
// unfortunately, there does not appear to be a way to provide a function that
// unifies these operations.

template<typename Iterator, typename Traits>
struct adapt_iterator : detail::operator_arrow<adapt_iterator<Iterator, Traits>> {
private:
	// TODO: [[no_unique_address]]
	using tuple_t = tuple<Iterator, Traits>;
	static_assert(std::is_copy_assignable_v<Iterator>);
	static_assert(std::is_copy_assignable_v<Traits>);

	using base_iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	using traits_t = decltype(containers::unwrap(std::declval<Traits>()));

public:
	using value_type = decltype(containers::unwrap(std::declval<Traits>()).dereference(std::declval<Iterator>()));
	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category =
		std::conditional_t<std::is_same_v<base_iterator_category, std::output_iterator_tag>, std::output_iterator_tag,
		std::conditional_t<std::is_same_v<base_iterator_category, std::input_iterator_tag>, std::input_iterator_tag,
		std::conditional_t<detail::is_random_access_iterator<Iterator, traits_t>(), std::random_access_iterator_tag,
		std::conditional_t<detail::is_bidirectional_iterator<Iterator, traits_t>(), std::bidirectional_iterator_tag,
		std::conditional_t<detail::is_forward_iterator<Iterator, traits_t>(), std::forward_iterator_tag,
		void
	>>>>>;
	static_assert(not std::is_void_v<iterator_category>);

	// Not sure what these actually mean...
	using pointer = std::remove_reference_t<value_type> *;
	using reference = value_type;

	constexpr adapt_iterator(Iterator it, Traits traits) noexcept(std::is_nothrow_constructible_v<tuple_t, Iterator, Traits>):
		m_data(std::move(it), std::move(traits))
	{
	}
	
	constexpr auto base() const noexcept {
		return m_data[0_bi];
	}

	constexpr auto const & traits() const noexcept {
		return containers::unwrap(m_data[1_bi]);
	}

	template<typename Index>
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<adapt_iterator>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}

private:
	tuple_t m_data;
};


template<typename Iterator, typename Traits>
constexpr auto operator*(adapt_iterator<Iterator, Traits> const it) BOUNDED_NOEXCEPT_DECLTYPE(
	it.traits().dereference(it.base())
)

template<typename Iterator, typename Traits, typename Offset>
constexpr auto operator+(adapt_iterator<Iterator, Traits> const lhs, Offset const rhs) BOUNDED_NOEXCEPT_VALUE(
	adapt_iterator<Iterator, Traits>(lhs.traits().add(lhs.base(), rhs), lhs.traits())
)


template<typename Iterator, typename Traits>
constexpr auto operator-(adapt_iterator<Iterator, Traits> const lhs, adapt_iterator<Iterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().subtract(lhs.base(), rhs.base())
)

template<typename Iterator, typename Traits>
constexpr auto compare(adapt_iterator<Iterator, Traits> const lhs, adapt_iterator<Iterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().compare(lhs.base(), rhs.base())
)

}	// namespace containers
