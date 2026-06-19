// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/bracket.hpp>

export module containers.static_string;

import containers.algorithms.compare;
import containers.algorithms.copy;
import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.data;
import containers.maximum_array_size;
import containers.size;
import containers.subrange;
import containers.to_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {
	
export template<array_size_type<char> size_>
struct static_string {
	constexpr explicit static_string(char const * const ptr):
		m_ptr(ptr)
	{
	}
	static constexpr auto size() {
		return bounded::constant<size_>;
	}
	constexpr auto data() const -> char const * {
		return m_ptr;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend constexpr auto operator<=>(static_string const lhs, static_string const rhs) {
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend constexpr auto operator==(static_string const lhs, static_string const rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}
	friend auto operator<=>(static_string const lhs, std::string const & rhs) {
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend auto operator==(static_string const lhs, std::string const & rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}

	constexpr operator std::string_view() const {
		return std::string_view(data(), static_cast<std::size_t>(size_));
	}
	constexpr operator std::span<char const, static_cast<std::size_t>(size_)>() const {
		return std::span<char const, static_cast<std::size_t>(size_)>(data());
	}
	explicit constexpr operator std::string() const {
		return std::string(data(), static_cast<std::size_t>(size()));
	}

	friend auto & operator<<(std::ostream & stream, static_string const & str) {
		return stream << std::string_view(str);
	}

private:
	char const * m_ptr;
};

template<std::size_t size>
struct storage {
	containers::array<char, bounded::constant<size>> string;

	constexpr storage(c_array<char, size + 1> const & str):
		string(to_array(subrange(str, bounded::constant<size>)))
	{
	}
};
template<std::size_t size>
storage(c_array<char, size> const &) -> storage<size - 1>;

export template<array_size_type<char> size_>
struct static_null_terminated_string {
private:
	using str_t = c_array<char, static_cast<std::size_t>(size_ + 1_bi)>;
public:
	constexpr explicit static_null_terminated_string(str_t const & str):
		m_str(std::addressof(str))
	{
		BOUNDED_ASSERT(str[static_cast<std::size_t>(size_)] == '\0');
	}
	static constexpr auto size() {
		return bounded::constant<size_>;
	}
	constexpr auto data() const -> char const * {
		return *m_str;
	}
	constexpr auto c_str() const -> str_t const & {
		return *m_str;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend constexpr auto operator<=>(
		static_null_terminated_string const lhs,
		static_null_terminated_string const rhs)
	{
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend constexpr auto operator==(
		static_null_terminated_string const lhs,
		static_null_terminated_string const rhs
	) -> bool {
		return ::containers::equal(lhs, rhs);
	}
	friend auto operator<=>(
		static_null_terminated_string const lhs,
		std::string const & rhs
	) {
		return ::containers::lexicographical_compare(lhs, rhs);
	}
	friend auto operator==(
		static_null_terminated_string const lhs,
		std::string const & rhs
	) -> bool {
		return ::containers::equal(lhs, rhs);
	}

	constexpr operator std::string_view() const {
		return std::string_view(data(), static_cast<std::size_t>(size_));
	}
	constexpr operator std::span<char const, static_cast<std::size_t>(size_)>() const {
		return std::span<char const, static_cast<std::size_t>(size_)>(data());
	}
	explicit constexpr operator std::string() const {
		return std::string(data(), static_cast<std::size_t>(size()));
	}

	friend auto & operator<<(std::ostream & stream, static_null_terminated_string const & str) {
		return stream << std::string_view(str);
	}

private:
	str_t const * m_str;
};

template<std::size_t size>
struct null_terminated_storage {
	containers::c_array<char, size> string;

	constexpr null_terminated_storage(containers::c_array<char, size> const & str) {
		containers::copy(str, string);
	}
};
template<std::size_t size>
null_terminated_storage(char const (&)[size]) -> null_terminated_storage<size>;

inline namespace string_literals {

export template<storage s>
constexpr auto operator""_s() {
	return static_string<containers::size(s.string)>(containers::data(s.string));
}

export template<null_terminated_storage s>
constexpr auto operator""_null_terminated() {
	return static_null_terminated_string<containers::size(s.string) - 1_bi>(s.string);
}

} // inline namespace string_literals

} // namespace containers

template<auto size, typename CharT>
struct std::formatter<containers::static_string<size>, CharT> : private std::formatter<std::string_view, CharT> {
private:
	using base = std::formatter<std::string_view, CharT>;
public:
	using base::parse;

	template<typename FormatContext>
	constexpr auto format(containers::static_string<size> const str, FormatContext & context) const {
		return base::format(std::string_view(str), context);
	}
};

template<auto size, typename CharT>
struct std::formatter<containers::static_null_terminated_string<size>, CharT> : private std::formatter<std::string_view, CharT> {
private:
	using base = std::formatter<std::string_view, CharT>;
public:
	using base::parse;

	template<typename FormatContext>
	constexpr auto format(containers::static_null_terminated_string<size> const str, FormatContext & context) const {
		return base::format(std::string_view(str), context);
	}
};

template<auto size>
struct std::hash<containers::static_string<size>> {
	static auto operator()(containers::static_string<size> const str) noexcept -> std::size_t {
		return std::hash<std::string_view>()(str);
	}
};

template<auto size>
struct std::hash<containers::static_null_terminated_string<size>> {
	static auto operator()(containers::static_null_terminated_string<size> const str) noexcept -> std::size_t {
		return std::hash<std::string_view>()(str);
	}
};

using namespace containers::string_literals;
using namespace std::string_view_literals;

static_assert(bounded::convertible_to<containers::static_string<5_bi>, std::string_view>);
static_assert(bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 5>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char, 5>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 4>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 6>>);

static_assert(""_s == ""sv);
static_assert("a"_s == "a"sv);
static_assert("01234567890123456789012"_s == "01234567890123456789012"sv);

static_assert(bounded::convertible_to<containers::static_null_terminated_string<5_bi>, std::string_view>);
static_assert(bounded::convertible_to<containers::static_null_terminated_string<5_bi>, std::span<char const, 5>>);
static_assert(!bounded::convertible_to<containers::static_null_terminated_string<5_bi>, std::span<char, 5>>);
static_assert(!bounded::convertible_to<containers::static_null_terminated_string<5_bi>, std::span<char const, 4>>);
static_assert(!bounded::convertible_to<containers::static_null_terminated_string<5_bi>, std::span<char const, 6>>);

static_assert(""_null_terminated == ""sv);
static_assert("a"_null_terminated == "a"sv);
static_assert("01234567890123456789012"_null_terminated == "01234567890123456789012"sv);
static_assert(containers::equal(""_null_terminated.c_str(), containers::array{'\0'}));