// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.range_view;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.forward_random_access_range;
import containers.has_member_size;
import containers.is_iterator_sentinel;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.iter_value_t;
import containers.range;
import containers.size;
import containers.sized_range;
import containers.to_address;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

struct compute_end_from_size {
	constexpr compute_end_from_size() = default;
	constexpr compute_end_from_size(auto const &) {
	}
	friend auto operator<=>(compute_end_from_size, compute_end_from_size) = default;
};

template<typename T>
concept explicit_sentinel = !std::same_as<T, compute_end_from_size>;

template<typename Sentinel, typename Iterator>
concept range_view_sentinel = sentinel_for<Sentinel, Iterator> or std::same_as<Sentinel, compute_end_from_size>;

struct no_explicit_size {
	constexpr no_explicit_size() = default;
	constexpr no_explicit_size(auto const &) {
	}
	friend auto operator<=>(no_explicit_size, no_explicit_size) = default;
};

template<typename T>
concept explicit_size = !std::same_as<T, no_explicit_size>;

template<typename T>
concept range_view_size = bounded::integral<T> or std::same_as<T, no_explicit_size>;

export template<iterator Iterator, range_view_sentinel<Iterator> Sentinel = Iterator, range_view_size Size = no_explicit_size>
struct range_view {
private:
	// TODO: Maybe check if we can do end - begin?
	static constexpr auto contiguous = to_addressable<Iterator> and explicit_size<Size>;
public:
	static_assert(explicit_sentinel<Sentinel> or explicit_size<Size>);

	constexpr range_view(Iterator first, Sentinel last, Size size_) requires explicit_size<Size>:
		m_begin(std::move(first)),
		m_end(std::move(last)),
		m_size(size_)
	{
	}
	constexpr range_view(Iterator first, Sentinel last) requires(!explicit_size<Size>):
		m_begin(std::move(first)),
		m_end(std::move(last))
	{
	}
	constexpr range_view(Iterator first, Size size_) requires(!explicit_sentinel<Sentinel>):
		m_begin(std::move(first)),
		m_size(size_)
	{
	}

	constexpr range_view(sized_range auto && r) requires explicit_size<Size>:
		range_view(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r)),
			containers::size(r)
		)
	{
	}
	constexpr range_view(range auto && r) requires(!explicit_size<Size>):
		range_view(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r))
		)
	{
	}

	constexpr auto begin() const & -> Iterator requires bounded::copy_constructible<Iterator> {
		return m_begin;
	}
	constexpr auto begin() && -> Iterator {
		return std::move(m_begin);
	}

	constexpr auto size() const -> Size requires explicit_size<Size> {
		return m_size;
	}

	constexpr auto data() const requires contiguous {
		return containers::to_address(m_begin);
	}
	constexpr auto data() requires contiguous {
		return containers::to_address(m_begin);
	}
	
	constexpr auto end() const & -> Sentinel requires explicit_sentinel<Sentinel> and bounded::copy_constructible<Sentinel> {
		return m_end;
	}
	constexpr auto end() && -> Sentinel requires explicit_sentinel<Sentinel> {
		return std::move(m_end);
	}
	// These are required for range-based for loops to work. A constrained `end`
	// is found by language rules and then causes an error when it cannot be
	// called.
	constexpr auto end() const & -> Iterator requires(!explicit_sentinel<Sentinel> and bounded::copy_constructible<Iterator>) {
		using offset = std::conditional_t<bounded::builtin_integer<Size>, iter_difference_t<Iterator>, Size>;
		return begin() + static_cast<offset>(size());
	}
	constexpr auto end() && -> Iterator {
		using offset = std::conditional_t<bounded::builtin_integer<Size>, iter_difference_t<Iterator>, Size>;
		return std::move(*this).begin() + static_cast<offset>(size());
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator==(range_view, range_view) -> bool = default;

	// TODO: spans of static extent
	constexpr operator std::span<std::remove_reference_t<iter_reference_t<Iterator>>>() const requires contiguous {
		return std::span(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::span<std::remove_reference_t<iter_reference_t<Iterator>>>() requires contiguous {
		return std::span(data(), static_cast<std::size_t>(size()));
	}
	constexpr operator std::basic_string_view<iter_value_t<Iterator>>() const requires(contiguous and std::is_trivial_v<iter_value_t<Iterator>>) {
		return std::basic_string_view(data(), static_cast<std::size_t>(size()));
	}
	
private:
	[[no_unique_address]] Iterator m_begin;
	[[no_unique_address]] Sentinel m_end;
	[[no_unique_address]] Size m_size;
};

template<range Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(bounded::declval<Range &&>())),
	decltype(containers::end(bounded::declval<Range &&>()))
>;

template<range Range> requires has_member_size<Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(bounded::declval<Range &&>())),
	decltype(containers::end(bounded::declval<Range &&>())),
	decltype(bounded::declval<Range &&>().size())
>;

template<forward_random_access_range Range> requires has_member_size<Range>
range_view(Range &&) -> range_view<
	decltype(containers::begin(bounded::declval<Range &&>())),
	compute_end_from_size,
	decltype(bounded::declval<Range &&>().size())
>;

template<iterator Iterator, sentinel_for<Iterator> Sentinel>
range_view(Iterator, Sentinel) -> range_view<
	Iterator,
	Sentinel
>;

template<iterator Iterator, bounded::integral Size>
range_view(Iterator, Size) -> range_view<
	Iterator,
	compute_end_from_size,
	Size
>;

export template<typename>
constexpr auto is_range_view = false;

template<typename Iterator, typename Sentinel, typename Size>
constexpr auto is_range_view<range_view<Iterator, Sentinel, Size>> = true;

export template<typename Iterator, typename Sentinel>
constexpr auto to_range_view(std::pair<Iterator, Sentinel> pair) {
	return range_view(std::move(pair).first, std::move(pair).second);
}


} // namespace containers

static_assert(containers::range_view<int *>(nullptr, nullptr).begin() == nullptr);
static_assert(containers::range_view(static_cast<int *>(nullptr), 0).data() == nullptr);
static_assert(containers::range_view<int *>(nullptr, nullptr).end() == nullptr);
static_assert(containers::range<containers::range_view<int *>>);

constexpr auto a = containers::array({0, 1, 2, 3, 4});

static_assert(std::same_as<
	decltype(containers::range_view(a)),
	containers::range_view<
		containers::contiguous_iterator<int const, 5_bi>,
		containers::compute_end_from_size,
		bounded::constant_t<5>
	>
>);

static_assert(containers::equal(a, containers::range_view(a)));

static_assert(bounded::convertible_to<
	std::string_view,
	containers::range_view<int *, int *, std::size_t>
>);
static_assert(bounded::convertible_to<
	std::span<int>,
	containers::range_view<int *, int *, std::size_t>
>);
static_assert(bounded::convertible_to<
	std::span<int const>,
	containers::range_view<int *, int *, std::size_t>
>);
