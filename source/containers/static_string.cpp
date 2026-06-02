// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/bracket.hpp>

export module containers.static_string;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
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

	friend auto operator<=>(static_string, static_string) = default;

	friend auto operator<=>(static_string const lhs, std::string const & rhs) {
		return std::string_view(lhs) <=> std::string_view(rhs);
	}
	friend auto operator==(static_string const lhs, std::string const & rhs) -> bool {
		return std::string_view(lhs) == std::string_view(rhs);
	}

	constexpr operator std::string_view() const {
		return std::string_view(data(), static_cast<std::size_t>(size_));
	}
	constexpr operator std::span<char const>() const {
		return std::span<char const>(data(), static_cast<std::size_t>(size_));
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

template<std::size_t size_>
struct storage {
	containers::array<char, bounded::constant<size_>> string;

	constexpr storage(char const (&str)[size_ + 1]):
		string(to_array(subrange(str, bounded::constant<size_>)))
	{
	}
};
template<std::size_t size>
storage(char const (&)[size]) -> storage<size - 1>;

inline namespace string_literals {

export template<storage s>
constexpr auto operator""_ss() {
	return static_string<containers::size(s.string)>(containers::data(s.string));
}

}

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

template<auto size>
struct std::hash<containers::static_string<size>> {
	static auto operator()(containers::static_string<size> const str) noexcept -> std::size_t {
		return std::hash<std::string_view>()(str);
	}
};

using namespace containers::string_literals;
using namespace std::string_view_literals;

static_assert(bounded::convertible_to<containers::static_string<5_bi>, std::string_view>);
static_assert(bounded::convertible_to<containers::static_string<5_bi>, std::span<char const>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char>>);
static_assert(bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 5>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char, 5>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 4>>);
static_assert(!bounded::convertible_to<containers::static_string<5_bi>, std::span<char const, 6>>);

static_assert(""_ss == ""sv);
static_assert("a"_ss == "a"sv);
static_assert("01234567890123456789012"_ss == "01234567890123456789012"sv);