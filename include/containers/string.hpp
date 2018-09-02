// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/concatenate.hpp>
#include <containers/range_view.hpp>
#include <containers/single_element_range.hpp>
#include <containers/small_buffer_optimized_vector.hpp>

#include <istream>
#include <ostream>

namespace containers {

// Unlike std::basic_string, there is no null terminator.
template<typename CharT, typename Allocator = allocator<CharT>>
struct basic_string : private small_buffer_optimized_vector<CharT, 1, Allocator> {
private:
	using base = small_buffer_optimized_vector<CharT, 1, Allocator>;
public:
	using typename base::value_type;
	using typename base::allocator_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;
	
	using base::get_allocator;

	using base::base;
	
	constexpr basic_string(std::basic_string_view<CharT> const sv, allocator_type allocator_ = allocator_type()) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(begin(sv), end(sv), std::move(allocator_))
	) {
	}
	
	using base::operator=;
	
	friend constexpr auto begin(basic_string const & container) noexcept {
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(basic_string & container) noexcept {
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(basic_string const & container) noexcept {
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(basic_string & container) noexcept {
		return end(static_cast<base &>(container));
	}

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	using base::shrink_to_fit;
	
	using base::append_from_capacity;
	
	using base::emplace_back;
	using base::emplace;
	using base::insert;
	using base::pop_back;
	
	constexpr operator std::basic_string_view<CharT>() const noexcept {
		return {data(*this), static_cast<typename std::basic_string_view<CharT>::size_type>(size(*this))};
	}
};

template<typename CharT, typename Allocator>
constexpr auto is_container<basic_string<CharT, Allocator>> = true;


namespace detail {

template<typename CharT>
struct c_string_sentinel_t {
};

template<typename CharT>
constexpr auto compare(CharT const * first, c_string_sentinel_t<CharT>) noexcept {
	return *first == '\0' ? bounded::strong_ordering_equal : bounded::strong_ordering_less;
}

template<typename CharT>
constexpr auto c_string_sentinel = c_string_sentinel_t<CharT>{};

} // namespace detail

template<typename CharT, typename Allocator>
constexpr auto compare(basic_string<CharT, Allocator> const & lhs, CharT const * const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>)
)
template<typename CharT, typename Allocator>
constexpr auto compare(CharT const * const lhs, basic_string<CharT, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(lhs, detail::c_string_sentinel<CharT>, begin(rhs), end(rhs))
)

template<typename CharT, typename Allocator>
constexpr auto compare(basic_string<CharT, Allocator> const & lhs, std::basic_string_view<CharT> const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), begin(rhs), end(rhs))
)
template<typename CharT, typename Allocator>
constexpr auto compare(std::basic_string_view<CharT> const lhs, basic_string<CharT, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), begin(rhs), end(rhs))
)


namespace detail {

template<typename T>
constexpr auto is_string = false;

template<typename CharT, typename Allocator>
constexpr auto is_string<basic_string<CharT, Allocator>> = true;

template<typename T>
constexpr auto string_like = false;

template<typename CharT, typename Allocator>
constexpr auto string_like<basic_string<CharT, Allocator>> = true;

template<typename CharT>
constexpr auto string_like<std::basic_string_view<CharT>> = true;

}	// namespace detail

template<typename LHS, typename RHS, BOUNDED_REQUIRES(
	(detail::string_like<std::decay_t<LHS>> and detail::string_like<std::decay_t<RHS>>) and
	(detail::is_string<std::decay_t<LHS>> or detail::is_string<std::decay_t<RHS>>) and
	std::is_same_v<typename std::decay_t<LHS>::value_type, typename std::decay_t<RHS>::value_type>
)>
auto operator+(LHS && lhs, RHS && rhs) {
	using result_t = std::decay_t<std::conditional_t<detail::is_string<std::decay_t<LHS>>, LHS, RHS>>;
	return ::containers::concatenate<result_t>(BOUNDED_FORWARD(lhs), BOUNDED_FORWARD(rhs));
}

template<typename String, typename CharT>
auto operator+(String && lhs, CharT const * const rhs) BOUNDED_NOEXCEPT_VALUE(
	BOUNDED_FORWARD(lhs) + std::basic_string_view(rhs)
)
template<typename CharT, typename String>
auto operator+(CharT const * const lhs, String && rhs) BOUNDED_NOEXCEPT_VALUE(
	std::basic_string_view(lhs) + BOUNDED_FORWARD(rhs)
)


template<typename String, BOUNDED_REQUIRES(detail::is_string<std::decay_t<String>>)>
auto operator+(String && lhs, typename std::remove_reference_t<String>::value_type const rhs) BOUNDED_NOEXCEPT_VALUE(
	containers::concatenate<std::decay_t<String>>(BOUNDED_FORWARD(lhs), ::containers::single_element_range(rhs))
)
template<typename String, BOUNDED_REQUIRES(detail::is_string<std::decay_t<String>>)>
auto operator+(typename std::remove_reference_t<String>::value_type const lhs, String && rhs) BOUNDED_NOEXCEPT_VALUE(
	containers::concatenate<std::decay_t<String>>(::containers::single_element_range(lhs), BOUNDED_FORWARD(rhs))
)

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template<typename CharT, typename Traits, typename Allocator>
auto & operator<<(std::basic_ostream<CharT, Traits> & out, basic_string<CharT, Allocator> const & str) {
	return out << static_cast<std::string_view>(str);
}

template<typename CharT, typename Traits, typename Allocator>
auto & operator>>(std::basic_istream<CharT, Traits> & in, basic_string<CharT, Allocator> const & str) {
	return in >> static_cast<std::string_view>(str);
}

// TODO: Add in a null-terminated string class

}	// namespace containers
