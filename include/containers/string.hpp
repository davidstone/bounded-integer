// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/concatenate.hpp>
#include <containers/begin_end.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/small_buffer_optimized_vector.hpp>

#include <bounded/clamp.hpp>

#include <istream>
#include <ostream>
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

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template<typename CharT>
auto & operator<<(std::basic_ostream<CharT> & stream, basic_string<CharT> const & str) {
	return stream << static_cast<std::string_view>(str);
}

template<typename CharT>
auto & operator>>(std::basic_istream<CharT> & stream, basic_string<CharT> & str) {
	auto const sentry = typename std::basic_istream<CharT>::sentry(stream);
	if (!sentry) {
		return stream;
	}
	constexpr auto max_width = numeric_traits::max_value<typename basic_string<CharT>::size_type>;
	auto const width = stream.width();
	auto const max_characters = width <= 0 ? max_width : bounded::clamp(bounded::integer(width), 1_bi, max_width);
	containers::clear(str);
	for (auto it = std::istreambuf_iterator(stream); it != std::istreambuf_iterator<CharT>(); ++it) {
		if (containers::size(str) == max_characters) {
			break;
		}
		if (std::isspace(*it, stream.getloc())) {
			break;
		}
		containers::push_back(str, *it);
	}
	if (containers::is_empty(str)) {
		stream.setstate(std::ios::failbit);
	}
	stream.width(0);
	return stream;
}

// TODO: Add in a null-terminated string class

}	// namespace containers
