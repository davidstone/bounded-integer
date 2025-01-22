// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.subrange;

import containers.begin_end;
import containers.c_array;
import containers.common_iterator_functions;
import containers.forward_random_access_range;
import containers.has_member_size;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.iter_value_t;
import containers.iterator;
import containers.range;
import containers.sentinel_for;
import containers.size;
import containers.sized_range;
import containers.subtractable;
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
struct subrange {
private:
	// TODO: Maybe check if we can do end - begin?
	static constexpr auto contiguous =
		to_addressable<Iterator> and
		(explicit_size<Size> or subtractable<Sentinel, Iterator>);
public:
	static_assert(explicit_sentinel<Sentinel> or explicit_size<Size>);

	constexpr subrange(Iterator first, Sentinel last, Size size_) requires explicit_size<Size>:
		m_begin(std::move(first)),
		m_end(std::move(last)),
		m_size(size_)
	{
	}
	constexpr subrange(Iterator first, Sentinel last) requires(!explicit_size<Size>):
		m_begin(std::move(first)),
		m_end(std::move(last))
	{
	}
	constexpr subrange(Iterator first, Size size_) requires(!explicit_sentinel<Sentinel>):
		m_begin(std::move(first)),
		m_size(size_)
	{
	}

	constexpr subrange(sized_range auto && r) requires explicit_size<Size>:
		subrange(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r)),
			containers::size(r)
		)
	{
	}
	constexpr subrange(range auto && r) requires(!explicit_size<Size>):
		subrange(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r))
		)
	{
	}

	constexpr auto begin() const & -> Iterator const & {
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
	
	constexpr auto end() const {
		if constexpr (explicit_sentinel<Sentinel>) {
			return m_end;
		} else {
			using offset = std::conditional_t<bounded::builtin_integer<Size>, iter_difference_t<Iterator>, Size>;
			return begin() + static_cast<offset>(size());
		}
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator==(subrange, subrange) -> bool = default;

	// TODO: spans of static extent
	constexpr operator std::span<std::remove_reference_t<iter_reference_t<Iterator>>>() const requires contiguous {
		return std::span(data(), static_cast<std::size_t>(::containers::size(*this)));
	}
	constexpr operator std::basic_string_view<iter_value_t<Iterator>>() const requires(contiguous and bounded::character<iter_value_t<Iterator>>) {
		return std::basic_string_view(data(), static_cast<std::size_t>(::containers::size(*this)));
	}
	
private:
	[[no_unique_address]] Iterator m_begin;
	[[no_unique_address]] Sentinel m_end;
	[[no_unique_address]] Size m_size;
};

template<range Range>
subrange(Range &&) -> subrange<
	decltype(containers::begin(bounded::declval<Range &&>())),
	decltype(containers::end(bounded::declval<Range &&>()))
>;

template<range Range> requires has_member_size<Range>
subrange(Range &&) -> subrange<
	decltype(containers::begin(bounded::declval<Range &&>())),
	decltype(containers::end(bounded::declval<Range &&>())),
	decltype(bounded::declval<Range &&>().size())
>;

template<forward_random_access_range Range> requires has_member_size<Range>
subrange(Range &&) -> subrange<
	decltype(containers::begin(bounded::declval<Range &&>())),
	compute_end_from_size,
	decltype(bounded::declval<Range &&>().size())
>;

template<typename T, std::size_t size>
subrange(c_array<T, size> &) -> subrange<
	contiguous_iterator<T, bounded::constant<size>>,
	compute_end_from_size,
	bounded::constant_t<bounded::normalize<size>>
>;

template<iterator Iterator, sentinel_for<Iterator> Sentinel>
subrange(Iterator, Sentinel) -> subrange<
	Iterator,
	Sentinel
>;

template<iterator Iterator, bounded::integral Size>
subrange(Iterator, Size) -> subrange<
	Iterator,
	compute_end_from_size,
	Size
>;

export template<typename>
constexpr auto is_range_view = false;

template<typename Iterator, typename Sentinel, typename Size>
constexpr auto is_range_view<subrange<Iterator, Sentinel, Size>> = true;

export template<typename Iterator, typename Sentinel>
constexpr auto to_range_view(std::pair<Iterator, Sentinel> pair) {
	return subrange(std::move(pair).first, std::move(pair).second);
}

} // namespace containers
