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
namespace detail {

template<typename CharT>
inline constexpr auto is_char_like = std::is_trivial_v<CharT> and std::is_standard_layout_v<CharT> and !std::is_array_v<CharT>;

} // namespace detail

// Unlike std::basic_string, there is no null terminator.
template<typename CharT>
struct basic_string : private small_buffer_optimized_vector<CharT, 1> {
	static_assert(detail::is_char_like<CharT>);
private:
	using base = small_buffer_optimized_vector<CharT, 1>;
public:
	using typename base::value_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;
	
	using base::base;
	
	constexpr basic_string(std::basic_string_view<CharT> const sv):
		base(sv)
	{
	}

	basic_string(basic_string const &) = default;
	basic_string(basic_string &&) = default;
	basic_string & operator=(basic_string const &) & = default;
	basic_string & operator=(basic_string &&) & = default;
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
	
	using base::emplace;
	
	constexpr operator std::basic_string_view<CharT>() const noexcept {
		return {data(*this), static_cast<typename std::basic_string_view<CharT>::size_type>(size(*this))};
	}
};

template<typename CharT>
inline constexpr auto is_container<basic_string<CharT>> = true;


namespace detail {

template<typename CharT>
struct c_string_sentinel_t {
};

template<typename CharT>
constexpr auto compare(CharT const * lhs, c_string_sentinel_t<CharT>) noexcept {
	return *lhs == '\0' ? bounded::strong_ordering::equal : bounded::strong_ordering::less;
}

template<typename CharT>
constexpr auto compare(c_string_sentinel_t<CharT> const lhs, CharT const * rhs) noexcept {
	return compare(0, compare(rhs, lhs));
}

template<typename CharT>
constexpr auto operator==(CharT const * first, c_string_sentinel_t<CharT>) noexcept {
	return *first == '\0';
}

template<typename CharT>
constexpr auto operator==(c_string_sentinel_t<CharT> const lhs, CharT const * rhs) noexcept {
	return rhs == lhs;
}

template<typename CharT>
inline constexpr auto c_string_sentinel = c_string_sentinel_t<CharT>{};

} // namespace detail

template<typename CharT>
constexpr auto compare(basic_string<CharT> const & lhs, CharT const * const rhs) noexcept {
	return ::containers::lexicographical_compare_3way(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>);
}
template<typename CharT>
constexpr auto compare(CharT const * const lhs, basic_string<CharT> const & rhs) noexcept {
	return compare(0, compare(rhs, lhs));
}

template<typename CharT>
constexpr auto operator==(basic_string<CharT> const & lhs, CharT const * const rhs) noexcept {
	return ::containers::equal(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>);
}
template<typename CharT>
constexpr auto operator==(CharT const * const lhs, basic_string<CharT> const & rhs) noexcept {
	return rhs == lhs;
}

template<typename CharT>
constexpr auto compare(basic_string<CharT> const & lhs, std::basic_string_view<CharT> const rhs) noexcept {
	return ::containers::lexicographical_compare_3way(lhs, rhs);
}
template<typename CharT>
constexpr auto compare(std::basic_string_view<CharT> const lhs, basic_string<CharT> const & rhs) noexcept {
	return compare(0, compare(rhs, lhs));
}

template<typename CharT>
constexpr auto operator==(basic_string<CharT> const & lhs, std::basic_string_view<CharT> const rhs) noexcept {
	return size(lhs) == rhs.size() and ::containers::equal(begin(lhs), end(lhs), begin(rhs));
}
template<typename CharT>
constexpr auto operator==(std::basic_string_view<CharT> const lhs, basic_string<CharT> const & rhs) noexcept {
	return rhs == lhs;
}


namespace detail {

template<typename T>
inline constexpr auto is_string = false;

template<typename CharT>
inline constexpr auto is_string<basic_string<CharT>> = true;

template<typename T>
inline constexpr auto string_like = false;

template<typename CharT>
inline constexpr auto string_like<basic_string<CharT>> = true;

template<typename CharT>
inline constexpr auto string_like<std::basic_string_view<CharT>> = true;

}	// namespace detail

template<typename LHS, typename RHS> requires(
	(detail::string_like<std::decay_t<LHS>> and detail::string_like<std::decay_t<RHS>>) and
	(detail::is_string<std::decay_t<LHS>> or detail::is_string<std::decay_t<RHS>>) and
	std::is_same_v<typename std::decay_t<LHS>::value_type, typename std::decay_t<RHS>::value_type>
)
auto operator+(LHS && lhs, RHS && rhs) {
	using result_t = std::decay_t<std::conditional_t<detail::is_string<std::decay_t<LHS>>, LHS, RHS>>;
	return ::containers::concatenate<result_t>(BOUNDED_FORWARD(lhs), BOUNDED_FORWARD(rhs));
}

template<typename CharT>
auto operator+(basic_string<CharT> && lhs, CharT const * const rhs) {
	return std::move(lhs) + std::basic_string_view(rhs);
}
template<typename CharT>
auto operator+(basic_string<CharT> const & lhs, CharT const * const rhs) {
	return lhs + std::basic_string_view(rhs);
}

template<typename CharT>
auto operator+(CharT const * const lhs, basic_string<CharT> && rhs) {
	return std::basic_string_view(lhs) + std::move(rhs);
}
template<typename CharT>
auto operator+(CharT const * const lhs, basic_string<CharT> const & rhs) {
	return std::basic_string_view(lhs) + rhs;
}


template<typename String> requires detail::is_string<std::decay_t<String>>
auto operator+(String && lhs, typename std::remove_reference_t<String>::value_type const rhs) {
	return containers::concatenate<std::decay_t<String>>(BOUNDED_FORWARD(lhs), ::containers::single_element_range(rhs));
}
template<typename String> requires detail::is_string<std::decay_t<String>>
auto operator+(typename std::remove_reference_t<String>::value_type const lhs, String && rhs) {
	return containers::concatenate<std::decay_t<String>>(::containers::single_element_range(lhs), BOUNDED_FORWARD(rhs));
}

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template<typename CharT, typename Traits>
auto & operator<<(std::basic_ostream<CharT, Traits> & out, basic_string<CharT> const & str) {
	return out << static_cast<std::string_view>(str);
}

template<typename CharT, typename Traits>
auto & operator>>(std::basic_istream<CharT, Traits> & in, basic_string<CharT> const & str) {
	return in >> static_cast<std::string_view>(str);
}

// TODO: Add in a null-terminated string class

}	// namespace containers
