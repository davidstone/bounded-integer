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

#include <iosfwd>

namespace containers {
namespace detail {

template<typename CharT>
concept char_like = std::is_trivial_v<CharT> and std::is_standard_layout_v<CharT> and !std::is_array_v<CharT>;

} // namespace detail

// Unlike std::basic_string, there is no null terminator.
template<typename CharT>
struct basic_string : private small_buffer_optimized_vector<CharT, 0> {
	static_assert(detail::char_like<CharT>);
private:
	using base = small_buffer_optimized_vector<CharT, 0>;
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
	explicit constexpr basic_string(CharT const * const c_str):
		base(std::basic_string_view<CharT>(c_str))
	{
	}

	basic_string(basic_string const &) = default;
	basic_string(basic_string &&) = default;
	basic_string & operator=(basic_string const &) & = default;
	basic_string & operator=(basic_string &&) & = default;
	using base::operator=;
	
	friend constexpr auto begin(basic_string const & container) {
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(basic_string & container) {
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(basic_string const & container) {
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(basic_string & container) {
		return end(static_cast<base &>(container));
	}

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	using base::shrink_to_fit;
	
	using base::append_from_capacity;
	
	constexpr operator std::basic_string_view<CharT>() const {
		return {data(*this), static_cast<typename std::basic_string_view<CharT>::size_type>(size(*this))};
	}
};

template<typename CharT>
inline constexpr auto is_container<basic_string<CharT>> = true;


namespace detail {

// TODO: non-template
template<typename CharT>
struct c_string_sentinel_t {
};

template<typename CharT>
constexpr auto operator<=>(CharT const * lhs, c_string_sentinel_t<CharT>) {
	return *lhs == '\0' ? std::strong_ordering::equal : std::strong_ordering::less;
}

template<typename CharT>
constexpr auto operator==(CharT const * first, c_string_sentinel_t<CharT>) -> bool {
	return *first == '\0';
}

template<typename CharT>
inline constexpr auto c_string_sentinel = c_string_sentinel_t<CharT>{};

} // namespace detail

template<typename CharT>
constexpr auto operator<=>(basic_string<CharT> const & lhs, CharT const * const rhs) {
	return ::containers::lexicographical_compare_3way(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>);
}

template<typename CharT>
constexpr auto operator==(basic_string<CharT> const & lhs, CharT const * const rhs) -> bool {
	return ::containers::equal(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>);
}

template<typename CharT>
constexpr auto operator<=>(basic_string<CharT> const & lhs, std::basic_string_view<CharT> const rhs) {
	return ::containers::lexicographical_compare_3way(lhs, rhs);
}

template<typename CharT>
constexpr auto operator==(basic_string<CharT> const & lhs, std::basic_string_view<CharT> const rhs) -> bool {
	return size(lhs) == rhs.size() and ::containers::equal(begin(lhs), end(lhs), begin(rhs));
}


namespace detail {

template<typename T>
inline constexpr auto is_string = false;

template<typename CharT>
inline constexpr auto is_string<basic_string<CharT>> = true;

template<typename T>
concept string_specialization = is_string<std::decay_t<T>>;

template<typename T>
inline constexpr auto is_string_like = false;

template<typename CharT>
inline constexpr auto is_string_like<basic_string<CharT>> = true;

template<typename CharT>
inline constexpr auto is_string_like<std::basic_string_view<CharT>> = true;

template<typename T>
concept string_like = is_string_like<std::decay_t<T>>;

}	// namespace detail

template<detail::string_like LHS, detail::string_like RHS> requires(
	(detail::string_specialization<LHS> or detail::string_specialization<RHS>) and
	std::is_same_v<typename std::decay_t<LHS>::value_type, typename std::decay_t<RHS>::value_type>
)
constexpr auto operator+(LHS && lhs, RHS && rhs) {
	using result_t = std::decay_t<std::conditional_t<detail::is_string<std::decay_t<LHS>>, LHS, RHS>>;
	return ::containers::concatenate<result_t>(OPERATORS_FORWARD(lhs), OPERATORS_FORWARD(rhs));
}

template<typename CharT>
constexpr auto operator+(basic_string<CharT> && lhs, CharT const * const rhs) {
	return std::move(lhs) + std::basic_string_view(rhs);
}
template<typename CharT>
constexpr auto operator+(basic_string<CharT> const & lhs, CharT const * const rhs) {
	return lhs + std::basic_string_view(rhs);
}

template<typename CharT>
constexpr auto operator+(CharT const * const lhs, basic_string<CharT> && rhs) {
	return std::basic_string_view(lhs) + std::move(rhs);
}
template<typename CharT>
constexpr auto operator+(CharT const * const lhs, basic_string<CharT> const & rhs) {
	return std::basic_string_view(lhs) + rhs;
}


template<detail::string_specialization String>
constexpr auto operator+(String && lhs, typename std::remove_reference_t<String>::value_type const rhs) {
	return containers::concatenate<std::decay_t<String>>(OPERATORS_FORWARD(lhs), ::containers::single_element_range(rhs));
}
template<detail::string_specialization String>
constexpr auto operator+(typename std::remove_reference_t<String>::value_type const lhs, String && rhs) {
	return containers::concatenate<std::decay_t<String>>(::containers::single_element_range(lhs), OPERATORS_FORWARD(rhs));
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
