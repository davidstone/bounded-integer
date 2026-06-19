// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.string;

import containers.algorithms.compare;
import containers.c_array;
import containers.clear;
import containers.is_empty;
import containers.push_back;
import containers.range_size_t;
import containers.range_value_t;
import containers.sbo_vector;
import containers.size;
import containers.static_string;
import containers.string_view;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

namespace containers {

// Unlike std::string, there is no null terminator.
export struct string : private sbo_vector<char, 0> {
	using base = sbo_vector<char, 0>;
public:
	using base::base;
	
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr string(Source) {
	}
	template<auto size_>
	constexpr string(static_string<size_> const other):
		base(other)
	{
	}
	explicit string(std::same_as<char const *> auto) = delete("Use `_s` literal instead");
	explicit string(std::same_as<char *> auto) = delete("Use `_s` literal instead");

	string(string const &) = default;
	string(string &&) = default;
	string & operator=(string const &) & = default;
	string & operator=(string &&) & = default;
	
	using base::data;
	using base::size;

	using base::operator[];
	
	using base::capacity;
	using base::replace_empty_allocation;
	using base::reserve;
	
	using base::set_size;
	
	constexpr operator string_view() const {
		return string_view(*this);
	}
	constexpr operator std::string_view() const {
		return std::string_view(data(), static_cast<range_size_t<std::string_view>>(size()));
	}
	constexpr operator std::span<char const>() const {
		return std::span<char const>(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<char>() {
		return std::span<char>(data(), static_cast<std::size_t>(size()));
	}

	friend constexpr auto operator<=>(string const & lhs, string_view const rhs) {
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend constexpr auto operator==(string const & lhs, string_view const rhs) -> bool {
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
		for (auto it = std::istreambuf_iterator<char>(stream); it != std::default_sentinel; ++it) {
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

template<typename CharT>
struct std::formatter<containers::string, CharT> : private std::formatter<std::string_view, CharT> {
private:
	using base = std::formatter<std::string_view, CharT>;
public:
	using base::parse;

	template<typename FormatContext>
	constexpr auto format(containers::string const & str, FormatContext & context) const {
		return base::format(std::string_view(str), context);
	}
};

template<>
struct std::hash<containers::string> {
	static auto operator()(containers::string_view const str) noexcept -> std::size_t {
		return std::hash<containers::string_view>()(str);
	}
};


static_assert(bounded::convertible_to<containers::static_string<5_bi>, containers::string>);
static_assert(!bounded::convertible_to<char const *, containers::string>);
static_assert(!bounded::convertible_to<char *, containers::string>);
static_assert(!bounded::convertible_to<containers::c_array<char, 5> const &, containers::string>);
static_assert(!bounded::convertible_to<containers::c_array<char, 5> &, containers::string>);
static_assert(!bounded::convertible_to<containers::string_view, containers::string>);
static_assert(!bounded::convertible_to<std::string_view, containers::string>);
static_assert(bounded::constructible_from<containers::string, containers::static_string<5_bi>>);
static_assert(bounded::constructible_from<containers::string, containers::string_view>);
static_assert(bounded::constructible_from<containers::string, std::string_view>);
static_assert(!bounded::constructible_from<containers::string, char const *>);
static_assert(!bounded::constructible_from<containers::string, containers::c_array<char, 5> const &>);

constexpr auto check_equal(containers::string_view const input) {
	auto const output = containers::string(input);
	BOUNDED_ASSERT(output == input);
	BOUNDED_ASSERT(containers::size(output) == containers::size(input));
	return true;
}

static_assert(check_equal(""_sv));
static_assert(check_equal("a"_sv));
static_assert(check_equal("01234567890123456789012"_sv));
static_assert(containers::string({'a', 'b', 'd'}) == containers::string("abd"_s));