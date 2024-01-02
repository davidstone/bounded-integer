// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.range;

import containers.c_array;
import containers.data;
import containers.has_member_size;
import containers.random_access_iterator;
import containers.sentinel_for;

import bounded;

namespace containers {

template<typename T>
concept size_based_range = has_member_size<T> and requires {
	{ bounded::declval<T>().begin() } -> random_access_iterator;
};

template<typename T>
concept basic_range = requires {
	{ bounded::declval<T>().end() } -> sentinel_for<decltype(bounded::declval<T>().begin())>;
};

template<typename T>
concept forward_linked_range = requires(T range) {
	range.before_begin();
	{ range.end() } -> sentinel_for<decltype(range.before_begin())>;
};

export template<typename T>
concept range = basic_range<T> or size_based_range<T> or contiguous_range<T> or forward_linked_range<T>;

} // namespace containers

template<typename... Ts>
concept all_qualifications_are_ranges_helper = (... and containers::range<Ts>);

template<typename T>
concept all_qualifications_are_ranges = all_qualifications_are_ranges_helper<T, T const, T &, T const &, T &&>;

struct begin_and_end {
	auto begin() const -> int *;
	auto end() const -> int *;
};
static_assert(all_qualifications_are_ranges<begin_and_end>);

struct begin_and_end_rvalue {
	auto begin() && -> int *;
	auto end() && -> int *;
};
static_assert(containers::range<begin_and_end_rvalue>);
static_assert(!containers::range<begin_and_end_rvalue const &>);

struct begin_and_size {
	auto begin() const -> int *;
	auto size() const -> int;
};
static_assert(all_qualifications_are_ranges<begin_and_size>);

struct begin_and_size_rvalue {
	auto begin() && -> int *;
	auto size() const -> int;
};
static_assert(containers::range<begin_and_size_rvalue>);
static_assert(!containers::range<begin_and_size_rvalue const &>);

struct before_begin_and_end {
	auto before_begin() const -> int *;
	auto end() const -> int *;
};
static_assert(all_qualifications_are_ranges<before_begin_and_end>);

struct data_and_size {
	auto data() const -> int *;
	auto size() const -> int;
};
static_assert(all_qualifications_are_ranges<data_and_size>);

static_assert(all_qualifications_are_ranges<containers::c_array<int, 5>>);

static_assert(!containers::range<int>);
static_assert(!containers::range<int *>);

struct data_and_end {
	auto data() const -> int *;
	auto end() const -> int *;
};
static_assert(!containers::range<data_and_end>);

struct before_begin_and_size {
	auto before_begin() const -> int *;
	auto size() const -> int;
};
static_assert(!containers::range<before_begin_and_size>);

struct begin_only {
	auto begin() const -> int *;
};
static_assert(!containers::range<begin_only>);

