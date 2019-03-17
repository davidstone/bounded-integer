// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/operator_arrow.hpp>
#include <containers/reference_wrapper.hpp>

#include <bounded/integer.hpp>
#include <bounded/detail/tuple.hpp>

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

namespace detail {
namespace compare_impl_detail {

using bounded::compare;

template<typename LHS, typename RHS>
constexpr auto compare_impl(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs)
)

} // namespace compare_impl_detail
} // namespace detail

struct default_compare {
	template<typename LHS, typename RHS>
	static constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		detail::compare_impl_detail::compare_impl(lhs, rhs)
	)
	template<typename LHS, typename RHS>
	static constexpr auto equal(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		lhs == rhs
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

// This provides the nested typedefs iterator_category and difference_type if
// and only if Iterator is an iterator, not a sentinel
template<typename Iterator, typename Traits, bool = is_iterator<Iterator>>
struct iterator_typedefs_base {
private:
	using base_category = typename std::iterator_traits<Iterator>::iterator_category;
public:
	using value_type = decltype(containers::unwrap(std::declval<Traits>()).dereference(std::declval<Iterator>()));

	// Not sure what these actually mean...
	using pointer = std::remove_reference_t<value_type> *;
	using reference = value_type;

	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category =
		std::conditional_t<std::is_same_v<base_category, std::output_iterator_tag>, std::output_iterator_tag,
		std::conditional_t<std::is_same_v<base_category, std::input_iterator_tag>, std::input_iterator_tag,
		std::conditional_t<detail::is_random_access_iterator<Iterator, Traits>(), std::random_access_iterator_tag,
		std::conditional_t<detail::is_bidirectional_iterator<Iterator, Traits>(), std::bidirectional_iterator_tag,
		std::conditional_t<detail::is_forward_iterator<Iterator, Traits>(), std::forward_iterator_tag,
		void
	>>>>>;
	static_assert(not std::is_void_v<iterator_category>);
};

template<typename Iterator, typename Traits>
struct iterator_typedefs_base<Iterator, Traits, false> {
};

}	// namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Add: it + 5_bi
// 3) Subtract: it1 - it2
// 4) Equal: it1 == it2
// 5) Compare: it1 <=> it2
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these behaviors. Adding,
// subtracting, comparing, and equality are all linked to the same algorithm,
// but unfortunately, there does not appear to be a way to provide a function
// that unifies these operations.

template<typename Iterator, typename Traits>
struct adapt_iterator :
	detail::iterator_typedefs_base<Iterator, decltype(containers::unwrap(std::declval<Traits>()))>,
	detail::operator_arrow<adapt_iterator<Iterator, Traits>>
{
private:
	// TODO: [[no_unique_address]]
	using tuple_t = bounded::tuple<Iterator, Traits>;
	static_assert(std::is_copy_assignable_v<Iterator>);
	static_assert(std::is_copy_assignable_v<Traits>);
public:
	adapt_iterator() = default;
	constexpr adapt_iterator(Iterator it, Traits traits) noexcept(std::is_nothrow_constructible_v<tuple_t, Iterator, Traits>):
		m_data(std::move(it), std::move(traits))
	{
	}
	
	constexpr auto base() const noexcept {
		return m_data[bounded::constant<0>];
	}

	constexpr auto && traits() const noexcept {
		return containers::unwrap(m_data[bounded::constant<1>]);
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


template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto operator-(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().subtract(lhs.base(), rhs.base())
)

template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto compare(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().compare(lhs.base(), rhs.base())
)

template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto operator==(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs.traits().equal(lhs.base(), rhs.base())
)

}	// namespace containers
