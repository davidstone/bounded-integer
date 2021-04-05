// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/concatenate.hpp>
#include <containers/begin_end.hpp>
#include <containers/range_view.hpp>
#include <containers/single_element_range.hpp>
#include <containers/small_buffer_optimized_vector.hpp>
#include <containers/value_type.hpp>

#include <iosfwd>
#include <type_traits>

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
	using value_type = CharT;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;
	
	using base::base;
	
	constexpr basic_string(std::basic_string_view<CharT> const sv):
		base(sv)
	{
	}
	constexpr basic_string(CharT const * const c_str):
		base(std::basic_string_view<CharT>(c_str))
	{
	}

	basic_string(basic_string const &) = default;
	basic_string(basic_string &&) = default;
	basic_string & operator=(basic_string const &) & = default;
	basic_string & operator=(basic_string &&) & = default;
	using base::operator=;
	
	using base::begin;
	using base::size;

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	
	using base::append_from_capacity;
	
	constexpr operator std::basic_string_view<CharT>() const {
		return {data(*this), static_cast<typename std::basic_string_view<CharT>::size_type>(size())};
	}
};


template<typename CharT>
constexpr auto operator<=>(basic_string<CharT> const & lhs, std::basic_string_view<std::type_identity_t<CharT>> const rhs) {
	return ::containers::lexicographical_compare_3way(lhs, rhs);
}

template<typename CharT>
constexpr auto operator==(basic_string<CharT> const & lhs, std::basic_string_view<std::type_identity_t<CharT>> const rhs) -> bool {
	return ::containers::equal(lhs, rhs);
}


namespace detail {

template<typename T>
inline constexpr auto is_string = false;

template<typename CharT>
inline constexpr auto is_string<basic_string<CharT>> = true;

template<typename T>
concept string_specialization = is_string<std::decay_t<T>>;

}	// namespace detail

template<detail::string_specialization LHS, detail::string_specialization RHS> requires(std::is_same_v<range_value_t<LHS>, range_value_t<RHS>>)
constexpr auto operator+(LHS && lhs, RHS && rhs) {
	return ::containers::concatenate<std::decay_t<LHS>>(OPERATORS_FORWARD(lhs), OPERATORS_FORWARD(rhs));
}
template<detail::string_specialization String>
constexpr auto operator+(String && lhs, std::basic_string_view<range_value_t<String>> rhs) {
	return ::containers::concatenate<std::decay_t<String>>(OPERATORS_FORWARD(lhs), rhs);
}
template<detail::string_specialization String>
constexpr auto operator+(std::basic_string_view<range_value_t<String>> lhs, String && rhs) {
	return ::containers::concatenate<std::decay_t<String>>(lhs, OPERATORS_FORWARD(rhs));
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
