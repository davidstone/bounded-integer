// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.split;

import containers.algorithms.advance;
import containers.algorithms.find;
import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_range;
import containers.legacy_iterator;
import containers.iter_difference_t;
import containers.iterator;
import containers.range_reference_t;
import containers.range_view;
import containers.reference_or_value;
import containers.size;

import bounded;
import numeric_traits;
import std_module;

namespace containers {
using namespace bounded::literal;

template<typename Integer>
using one_wider = bounded::integer<
	bounded::normalize<numeric_traits::min_value<Integer> - 1_bi>,
	bounded::normalize<numeric_traits::max_value<Integer> + 1_bi>
>;

template<typename Delimiter>
struct single_element_delimiter {
	constexpr explicit single_element_delimiter(Delimiter const & delimiter):
		m_delimiter(delimiter)
	{
	}
	constexpr auto find(auto const & first, auto const & last) const {
		return containers::find(
			first,
			last,
			m_delimiter.get()
		);
	}
	constexpr auto skip(auto iterator) const {
		++iterator;
		return iterator;
	}
private:
	[[no_unique_address]] reference_or_value<Delimiter> m_delimiter;
};

template<typename Delimiter>
struct range_delimiter {
	constexpr explicit range_delimiter(Delimiter const & delimiter):
		m_delimiter(delimiter)
	{
	}
	constexpr auto find(auto const & first, auto const & last) const {
		return std::search(
			make_legacy_iterator(first),
			make_legacy_iterator(last),
			make_legacy_iterator(containers::begin(m_delimiter.get())),
			make_legacy_iterator(containers::end(m_delimiter.get()))
		).base();
	}
	template<iterator Iterator>
	constexpr auto skip(Iterator it) const {
		auto const delimiter_size = bounded::assume_in_range<iter_difference_t<Iterator>>(
			containers::size(m_delimiter.get())
		);
		return containers::next(std::move(it), delimiter_size);
	}
private:
	[[no_unique_address]] reference_or_value<Delimiter> m_delimiter;
};

template<typename Iterator, typename Sentinel, typename Delimiter>
struct split_iterator {
	using difference_type = one_wider<iter_difference_t<Iterator>>;
	constexpr split_iterator(Iterator first, Sentinel last, Delimiter delimiter):
		m_first(first),
		m_last(last),
		m_delimiter(delimiter),
		m_empty_trailing_range(m_first == m_last),
		m_middle(find_from_first())
	{
	}

	constexpr auto operator*() const {
		return range_view(m_first, m_middle);
	}
	friend constexpr auto operator+(split_iterator lhs, bounded::constant_t<1>) {
		lhs.m_first = lhs.m_middle;
		if (lhs.m_first == lhs.m_last) {
			lhs.m_empty_trailing_range = false;
			return lhs;
		}
		lhs.skip_delimiter();
		lhs.m_empty_trailing_range = lhs.m_first == lhs.m_last;
		lhs.m_middle = lhs.find_from_first();
		return lhs;
	}

	// Can be called on any iterator, even an end iterator
	constexpr auto remainder() const {
		return range_view(m_first, m_last);
	}

	// It is undefined behavior to compare iterators into two different `split`
	// ranges
	friend constexpr auto operator<=>(split_iterator const & lhs, split_iterator const & rhs) requires bounded::ordered<Iterator> {
		if (auto cmp = lhs.m_first <=> rhs.m_first; cmp != 0) {
			return cmp;
		}
		return lhs.m_empty_trailing_range <=> rhs.m_empty_trailing_range;
	}
	friend constexpr auto operator==(split_iterator const & lhs, split_iterator const & rhs) -> bool {
		return lhs.m_first == rhs.m_first and lhs.m_empty_trailing_range == rhs.m_empty_trailing_range;
	}
	friend constexpr auto operator==(split_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_first == lhs.m_last and !lhs.m_empty_trailing_range;
	}
private:
	constexpr auto find_from_first() const -> Iterator {
		return m_delimiter.find(m_first, m_last);
	}
	constexpr auto skip_delimiter() -> void {
		m_first = m_delimiter.skip(std::move(m_first));
	}
	[[no_unique_address]] Iterator m_first;
	[[no_unique_address]] Sentinel m_last;
	[[no_unique_address]] Delimiter m_delimiter;
	[[no_unique_address]] bool m_empty_trailing_range;
	[[no_unique_address]] Iterator m_middle;
};

export template<forward_range Range, bounded::equality_comparable<range_reference_t<Range>> Delimiter>
struct split {
	static_assert(!std::is_array_v<std::remove_cvref_t<Range>>);
	constexpr split(Range && range, Delimiter && delimiter):
		m_range(OPERATORS_FORWARD(range)),
		m_delimiter(OPERATORS_FORWARD(delimiter))
	{
	}
	constexpr auto begin() const & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			single_element_delimiter(m_delimiter)
		);
	}
	constexpr auto begin() & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			single_element_delimiter(m_delimiter)
		);
	}
	constexpr auto begin() && {
		return split_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range)),
			single_element_delimiter(m_delimiter)
		);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Delimiter m_delimiter;
};

template<typename Range, typename Delimiter>
split(Range &&, Delimiter &&) -> split<Range, Delimiter>;

// The delimiter is a range that can be searched for in the range-to-be-split
export template<forward_range Range, forward_range Delimiter>
struct split_range {
	static_assert(!std::is_array_v<std::remove_cvref_t<Range>>);
	constexpr split_range(Range && range, Delimiter && delimiter):
		m_range(OPERATORS_FORWARD(range)),
		m_delimiter(OPERATORS_FORWARD(delimiter))
	{
	}
	constexpr auto begin() const & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			range_delimiter(m_delimiter)
		);
	}
	constexpr auto begin() & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			range_delimiter(m_delimiter)
		);
	}
	constexpr auto begin() && {
		return split_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range)),
			range_delimiter(m_delimiter)
		);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Delimiter m_delimiter;
};

template<typename Range, typename Delimiter>
split_range(Range &&, Delimiter &&) -> split_range<Range, Delimiter>;

template<typename Range>
split_range(Range &&, char const *) -> split_range<Range, std::string_view>;

} // namespace containers