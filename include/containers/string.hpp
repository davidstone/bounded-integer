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
#include <span>
#include <type_traits>

namespace containers {

// Unlike std::string, there is no null terminator.
struct string : private small_buffer_optimized_vector<char, 0> {
	using base = small_buffer_optimized_vector<char, 0>;
public:
	using value_type = char;
	using base::const_iterator;
	using base::iterator;
	
	using base::base;
	
	constexpr string(std::string_view const sv):
		base(sv)
	{
	}
	constexpr string(char const * const c_str):
		base(std::string_view(c_str))
	{
	}

	string(string const &) = default;
	string(string &&) = default;
	string & operator=(string const &) & = default;
	string & operator=(string &&) & = default;
	using base::operator=;
	
	using base::begin;
	using base::size;

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	
	using base::append_from_capacity;
	
	constexpr operator std::string_view() const {
		return std::string_view(data(*this), static_cast<range_size_t<std::string_view>>(size()));
	}
	constexpr operator std::span<char const>() const & {
		return std::span<char const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<char>() & {
		return std::span<char>(containers::data(*this), static_cast<std::size_t>(size()));
	}

	friend constexpr auto operator<=>(string const & lhs, std::string_view const rhs) {
		return ::containers::lexicographical_compare_3way(lhs, rhs);
	}
	friend constexpr auto operator==(string const & lhs, std::string_view const rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}

	friend auto & operator<<(std::ostream & stream, string const & str) {
		return stream << std::string_view(str);
	}

	friend auto & operator>>(std::istream & stream, string & str) {
		auto const sentry = std::istream::sentry(stream);
		if (!sentry) {
			return stream;
		}
		constexpr auto max_width = numeric_traits::max_value<range_size_t<string>>;
		auto const width = stream.width();
		auto const max_characters = width <= 0 ? max_width : bounded::clamp(bounded::integer(width), 1_bi, max_width);
		containers::clear(str);
		for (auto it = std::istreambuf_iterator(stream); it != std::istreambuf_iterator<char>(); ++it) {
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
};

} // namespace containers
