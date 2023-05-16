// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <iterator>
#include <string_view>

#include <bounded/assert.hpp>

export module containers.string;

import containers.algorithms.compare;
import containers.begin_end;
import containers.c_array;
import containers.clear;
import containers.data;
import containers.is_empty;
import containers.push_back;
import containers.range_value_t;
import containers.size;
import containers.small_buffer_optimized_vector;
import containers.test_reserve_and_capacity;
import containers.test_sequence_container;
import containers.test_set_size;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

// Unlike std::string, there is no null terminator.
export struct string : private small_buffer_optimized_vector<char, 0> {
	using base = small_buffer_optimized_vector<char, 0>;
public:
	using base::base;
	
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr string(Source) {
	}
	template<typename Source> requires bounded::convertible_to<Source, std::string_view> or bounded::convertible_to<Source, char const *>
	constexpr explicit(!std::same_as<Source, std::string_view> and !std::same_as<Source, char const *> and !std::same_as<Source, char *>) string(Source const sv):
		base(std::string_view(sv))
	{
	}

	string(string const &) = default;
	string(string &&) = default;
	string & operator=(string const &) & = default;
	string & operator=(string &&) & = default;
	
	using base::begin;
	using base::size;

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	
	using base::set_size;
	
	constexpr operator std::string_view() const {
		return std::string_view(data(*this), static_cast<range_size_t<std::string_view>>(size()));
	}
	constexpr operator std::span<char const>() const {
		return std::span<char const>(containers::data(*this), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<char>() {
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
		for (auto it = std::istreambuf_iterator<char>(stream); it != std::istreambuf_iterator<char>(); ++it) {
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

struct to_sv {
	constexpr operator std::string_view() const {
		return "";
	}
};

struct to_cstr {
	constexpr operator char const *() const {
		return "";
	}
};

struct to_both {
	constexpr operator char const *() const {
		return "";
	}
	constexpr operator std::string_view() const {
		return "";
	}
};

static_assert(bounded::convertible_to<char const *, containers::string>);
static_assert(bounded::convertible_to<char *, containers::string>);
static_assert(bounded::convertible_to<containers::c_array<char, 5> const &, containers::string>);
static_assert(bounded::convertible_to<containers::c_array<char, 5> &, containers::string>);
static_assert(bounded::convertible_to<std::string_view, containers::string>);
static_assert(!bounded::convertible_to<to_sv, containers::string>);
static_assert(!bounded::convertible_to<to_cstr, containers::string>);
static_assert(!bounded::convertible_to<to_both, containers::string>);
static_assert(bounded::constructible_from<containers::string, to_sv>);
static_assert(bounded::constructible_from<containers::string, to_cstr>);
static_assert(bounded::constructible_from<containers::string, to_both>);

constexpr auto check_equal(std::string_view const input) {
	auto const output = containers::string(input);
	BOUNDED_ASSERT(output == input);
	BOUNDED_ASSERT(containers::size(output) == containers::size(input));
	return true;
}

static_assert(check_equal(""));
static_assert(check_equal("a"));
static_assert(check_equal("01234567890123456789012"));