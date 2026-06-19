// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/bracket.hpp>

export module containers.string_view;

import containers.algorithms.compare;
import containers.c_array;
import containers.contiguous_range;
import containers.data;
import containers.maximum_array_size;
import containers.range_size_t;
import containers.range_value_t;
import containers.size;
import containers.static_string;
import containers.subrange;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

namespace containers {

template<typename T>
constexpr auto is_subrange = false;
	
template<typename Iterator, typename Sentinel, typename Size>
constexpr auto is_subrange<subrange<Iterator, Sentinel, Size>> = true;

template<typename T>
concept contiguous_subrange_of_char =
	is_subrange<T> and
	std::same_as<char, range_value_t<T>> and
	contiguous_range<T>;

export struct string_view {
	string_view() = default;

	template<array_size_type<char> size_>
	constexpr string_view(static_string<size_> const str):
		m_data(str.data()),
		m_size(str.size())
	{
	}
	template<array_size_type<char> size_>
	constexpr string_view(static_null_terminated_string<size_> const str):
		m_data(str.data()),
		m_size(str.size())
	{
	}
	constexpr string_view(std::string_view const str):
		m_data(str.data()),
		m_size(bounded::assume_in_range<array_size_type<char>>(str.size()))
	{
	}
	constexpr string_view(std::string const & str):
		m_data(str.data()),
		m_size(bounded::assume_in_range<array_size_type<char>>(str.size()))
	{
	}
	constexpr string_view(contiguous_subrange_of_char auto const other):
		m_data(containers::data(other)),
		m_size(bounded::assume_in_range<array_size_type<char>>(containers::size(other)))
	{
	}
	constexpr explicit string_view(char const * const ptr):
		m_data(ptr),
		m_size(bounded::assume_in_range<array_size_type<char>>(std::char_traits<char>::length(ptr)))
	{
	}
	constexpr explicit string_view(char const * const ptr, array_size_type<char> const size_):
		m_data(ptr),
		m_size(size_)
	{
	}
	template<contiguous_range R> requires(std::same_as<range_value_t<R>, char> and !contiguous_subrange_of_char<R>)
	constexpr explicit string_view(R const & source):
		m_data(containers::data(source)),
		m_size(bounded::assume_in_range<array_size_type<char>>(containers::size(source)))
	{
	}

	static constexpr auto from_c_str(char const * const ptr, std::size_t const size) -> string_view {
		return string_view(ptr, bounded::assume_in_range<array_size_type<char>>(size));
	}

	constexpr auto data() const -> char const * {
		return m_data;
	}
	constexpr auto size() const -> array_size_type<char> {
		return m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	friend constexpr auto operator<=>(string_view const lhs, string_view const rhs) {
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend constexpr auto operator==(string_view const lhs, string_view const rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}

	friend auto operator<<(std::ostream & stream, string_view const str) -> std::ostream & {
		return stream << std::string_view(str);
	}

	constexpr operator std::string_view() const {
		return std::string_view(data(), static_cast<range_size_t<std::string_view>>(size()));
	}
	constexpr operator std::span<char const>() const {
		return std::span<char const>(data(), static_cast<std::size_t>(size()));
	}

private:
	char const * m_data = nullptr;
	array_size_type<char> m_size = 0_bi;
};

inline namespace string_literals {

export constexpr auto operator""_sv(char const * const ptr, std::size_t const size) -> string_view {
	return string_view(ptr, bounded::assume_in_range<array_size_type<char>>(size));
}

} // inline namespace string_literals
} // namespace containers

template<auto lhs_size, auto rhs_size> requires(lhs_size != rhs_size)
struct std::common_type<containers::static_string<lhs_size>, containers::static_string<rhs_size>> {
	using type = containers::string_view;
};

template<typename CharT>
struct std::formatter<containers::string_view, CharT> : private std::formatter<std::string_view, CharT> {
private:
	using base = std::formatter<std::string_view, CharT>;
public:
	using base::parse;

	template<typename FormatContext>
	constexpr auto format(containers::string_view const str, FormatContext & context) const {
		return base::format(std::string_view(str), context);
	}
};

template<>
struct std::hash<containers::string_view> {
	static auto operator()(containers::string_view const str) noexcept -> std::size_t {
		return std::hash<std::string_view>()(str);
	}
};


static_assert(bounded::convertible_to<containers::static_string<5_bi>, containers::string_view>);
static_assert(bounded::convertible_to<containers::subrange<char const *>, containers::string_view>);
static_assert(bounded::convertible_to<containers::subrange<char *>, containers::string_view>);
static_assert(!bounded::convertible_to<char const *, containers::string_view>);
static_assert(!bounded::convertible_to<char *, containers::string_view>);
static_assert(!bounded::convertible_to<containers::c_array<char, 5> const &, containers::string_view>);
static_assert(!bounded::convertible_to<containers::c_array<char, 5> &, containers::string_view>);
static_assert(bounded::convertible_to<std::string_view, containers::string_view>);
static_assert(bounded::constructible_from<containers::string_view, containers::subrange<char const *>>);
static_assert(bounded::constructible_from<containers::string_view, containers::static_string<5_bi>>);
static_assert(bounded::constructible_from<containers::string_view, std::string_view>);
static_assert(bounded::constructible_from<containers::string_view, char const *>);
static_assert(bounded::constructible_from<containers::string_view, containers::c_array<char, 5> const &>);

constexpr auto check_equal(auto const input) {
	auto const output = containers::string_view(input);
	BOUNDED_ASSERT(containers::data(output) == containers::data(input));
	BOUNDED_ASSERT(containers::size(output) == containers::size(input));
	BOUNDED_ASSERT(output == input);
	BOUNDED_ASSERT((output <=> input) == 0);
	return true;
}

static_assert(check_equal(""_s));
static_assert(check_equal("abc"_s));

static_assert(check_equal(""_null_terminated));
static_assert(check_equal("abc"_null_terminated));

static_assert(""_sv == ""_s);
static_assert("abc"_sv == "abc"_s);