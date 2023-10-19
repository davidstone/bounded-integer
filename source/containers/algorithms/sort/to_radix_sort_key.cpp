// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.sort.to_radix_sort_key;

import containers.algorithms.sort.is_sorted;

import containers.array;
import containers.range;

import bounded;
import bounded.test_int;
import numeric_traits;
import tv;
import std_module;

using namespace bounded::literal;

namespace containers {

// TODO: Also validate get and tuple_element
export template<typename T>
concept tuple_like = requires{
	std::tuple_size<std::decay_t<T>>::value;
};

export template<typename T>
concept indexable_range =
	range<T> and (
		requires(T && value) { OPERATORS_FORWARD(value)[0]; } or
		requires(T && value) { OPERATORS_FORWARD(value)[0_bi]; }
	);

struct unknown_floating_point;
template<typename T>
using float_to_unsigned =
	std::conditional_t<sizeof(T) == sizeof(std::uint16_t), std::uint16_t,
	std::conditional_t<sizeof(T) == sizeof(std::uint32_t), std::uint32_t,
	std::conditional_t<sizeof(T) == sizeof(std::uint64_t), std::uint64_t,
	unknown_floating_point
>>>;

template<typename T>
concept character = 
	std::same_as<T, char> or
	std::same_as<T, char8_t> or
	std::same_as<T, char16_t> or
	std::same_as<T, char32_t> or
	std::same_as<T, wchar_t>;

namespace to_radix_sort_key_adl {

constexpr auto to_radix_sort_key(bool const value) {
	return value;
}

constexpr auto to_radix_sort_key(std::byte const value) {
	return static_cast<unsigned char>(value);
}

template<bounded::builtin_integer T>
constexpr auto to_radix_sort_key(T const value) {
	if constexpr (character<T>) {
		return static_cast<std::make_unsigned_t<T>>(value);
	} else if constexpr (bounded::unsigned_builtin<T>) {
		return value;
	} else {
		static_assert(bounded::signed_builtin<T>);
		using unsigned_t = std::make_unsigned_t<T>;
		return static_cast<unsigned_t>(static_cast<unsigned_t>(value) + static_cast<unsigned_t>(numeric_traits::min_value<T>));
	}
}

constexpr auto to_radix_sort_key(bounded::bounded_integer auto const value) {
	return to_radix_sort_key(value.value());
}

template<typename T> requires std::is_enum_v<T>
constexpr auto to_radix_sort_key(T const value) {
	return to_radix_sort_key(bounded::integer(value));
}

template<std::floating_point T>
constexpr auto to_radix_sort_key(T const value) {
	static_assert(std::numeric_limits<T>::is_iec559);
	using unsigned_t = float_to_unsigned<T>;
	static_assert(sizeof(T) == sizeof(unsigned_t));
	auto const u = std::bit_cast<unsigned_t>(value);
	constexpr auto sign_bit_position = std::numeric_limits<unsigned_t>::digits - 1U;
	auto const sign_bit = static_cast<unsigned_t>(-static_cast<std::make_signed_t<unsigned_t>>(u >> (sign_bit_position)));
	return u ^ (sign_bit | (static_cast<unsigned_t>(1U) << sign_bit_position));
}

auto to_radix_sort_key(auto * ptr) {
	return reinterpret_cast<std::uintptr_t>(ptr);
}

// libc++ doesn't have this constexpr yet
inline auto to_radix_sort_key(std::vector<bool>::reference const value) {
	return static_cast<bool>(value);
}

// TODO: This should not be the default behavior.
constexpr auto && to_radix_sort_key(indexable_range auto && value) {
	return OPERATORS_FORWARD(value);
}

// TODO: This should not be the default behavior.
template<tuple_like T> requires(!indexable_range<T>)
constexpr auto && to_radix_sort_key(T && value) {
	return OPERATORS_FORWARD(value);
}

template<typename T>
concept default_ska_sortable_value = requires(T && value) {
	to_radix_sort_key(OPERATORS_FORWARD(value));
};

constexpr decltype(auto) to_radix_sort_key_impl(default_ska_sortable_value auto && value) {
	return to_radix_sort_key(OPERATORS_FORWARD(value));
}

} // namespace to_radix_sort_key_adl

using to_radix_sort_key_adl::default_ska_sortable_value;

export struct to_radix_sort_key_t {
	static constexpr auto operator()(default_ska_sortable_value auto && value) -> decltype(auto) {
		return containers::to_radix_sort_key_adl::to_radix_sort_key_impl(OPERATORS_FORWARD(value));
	}
};
export constexpr auto to_radix_sort_key = to_radix_sort_key_t();

} // namespace containers

// nextafter is not constexpr. This signature is easier to implement
template<typename T>
constexpr auto increase_float_magnitude(T const value, unsigned const offset) {
	return std::bit_cast<T>(
		std::bit_cast<containers::float_to_unsigned<T>>(value) + offset
	);
}

static_assert(increase_float_magnitude(0.0F, 1) > 0.0F);
static_assert(increase_float_magnitude(0.0F, 1) < 0.1F);
static_assert(increase_float_magnitude(-0.0F, 1) < -0.0F);
static_assert(increase_float_magnitude(-0.0F, 1) > -0.1F);

static_assert(increase_float_magnitude(0.0, 1) > 0.0);
static_assert(increase_float_magnitude(0.0, 1) < 0.1);
static_assert(increase_float_magnitude(-0.0, 1) < -0.0);
static_assert(increase_float_magnitude(-0.0, 1) > -0.1);


constexpr auto is_sorted_to_radix(auto... values) {
	auto const original = containers::array{values...};
	BOUNDED_ASSERT(containers::is_sorted(original));
	auto const converted = containers::array{containers::to_radix_sort_key(values)...};
	return containers::is_sorted(converted);
}

template<typename T>
constexpr auto is_sorted_converted_to_radix(auto... values) {
	return is_sorted_to_radix(static_cast<T>(values)...);
}


static_assert(is_sorted_converted_to_radix<std::int8_t>(
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127
));

static_assert(is_sorted_converted_to_radix<std::int16_t>(
	-32768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1000,
	32767
));

static_assert(is_sorted_converted_to_radix<std::int32_t>(
	numeric_traits::min_value<std::int32_t>,
	numeric_traits::min_value<std::int32_t> + 1,
	-1000001,
	-32769,
	-32768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1000,
	32767,
	32768,
	1000000,
	numeric_traits::max_value<std::int32_t> - 1,
	numeric_traits::max_value<std::int32_t>
));

static_assert(is_sorted_converted_to_radix<std::int64_t>(
	numeric_traits::min_value<std::int64_t>,
	-1'000'000'000'000,
	static_cast<std::int64_t>(numeric_traits::min_value<std::int32_t>) - 1,
	numeric_traits::min_value<std::int32_t>,
	numeric_traits::min_value<std::int32_t> + 1,
	-1'000'001,
	-32'769,
	-32'768,
	-256,
	-129,
	-128,
	-55,
	-4,
	0,
	2,
	5,
	5,
	6,
	6,
	7,
	8,
	19,
	23,
	99,
	127,
	1'000,
	32'767,
	32'768,
	1'000'000,
	numeric_traits::max_value<std::int32_t> - 1,
	numeric_traits::max_value<std::int32_t>,
	numeric_traits::max_value<std::int32_t>,
	1'000'000'000'000,
	numeric_traits::max_value<std::int64_t>
));


template<typename T>
constexpr auto test_floating_point() {
	return is_sorted_converted_to_radix<T>(
		-std::numeric_limits<T>::infinity(),
		-777777777.7,
		-1000001.0,
		-32769.0,
		-32768.0,
		-256.0,
		-129.0,
		-128.0,
		-55.0,
		-12.4,
		-4.0,
		-0.0000002,
		increase_float_magnitude(T(-0.0), 2),
		increase_float_magnitude(T(-0.0), 1),
		-0.0,
		0.0,
		increase_float_magnitude(T(0.0), 1),
		increase_float_magnitude(T(0.0), 2),
		0.1,
		2.0,
		2.5,
		5.0,
		6.0,
		7.0,
		8.0,
		17.8,
		19.0,
		23.0,
		99.0,
		127.0,
		1000.0,
		32767.0,
		32768.0,
		1000000.0,
		444444444444.4,
		std::numeric_limits<T>::infinity()
	);
}

static_assert(test_floating_point<float>());
static_assert(test_floating_point<double>());

template<typename Char>
constexpr auto test_character() {
	return is_sorted_converted_to_radix<Char>(
		'\0',
		' ',
		'!',
		'0',
		'A',
		'Z',
		'a',
		'b',
		'c',
		'z'
	);
}

static_assert(test_character<char>());
static_assert(test_character<char8_t>());
static_assert(test_character<char16_t>());
static_assert(test_character<char32_t>());
static_assert(test_character<wchar_t>());

static_assert(is_sorted_converted_to_radix<std::byte>(
	std::byte(0x00),
	std::byte(0x01),
	std::byte(0xFF)
));

static_assert(is_sorted_converted_to_radix<tv::tuple<std::byte>>(
	tv::tuple(std::byte(0x00)),
	tv::tuple(std::byte(0x01)),
	tv::tuple(std::byte(0xFF))
));

