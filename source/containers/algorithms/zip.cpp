// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.zip;

import containers.begin_end;
import containers.iter_difference_t;
import containers.range;
import containers.size;
import containers.sized_range;
import containers.subrange;
import containers.take;

import bounded;
import numeric_traits;
import std_module;
import tv;

using namespace bounded::literal;

namespace containers {

template<typename... Iterators>
struct zip_iterator;

template<typename... Iterators>
struct zip_difference_type_impl {
	static constexpr auto min_max = bounded::min(
		bounded::constant<numeric_traits::max_value<iter_difference_t<Iterators>>>...
	);
	using type = bounded::integer<
		bounded::normalize<-min_max>,
		bounded::normalize<min_max>
	>;
};

template<>
struct zip_difference_type_impl<> {
	using type = bounded::constant_t<0>;
};

template<typename... Sentinels>
struct zip_sentinel;

template<typename... Sentinels>
struct zip_smallest_sentinel;

template<typename... Iterators>
struct zip_iterator {
	using difference_type = typename zip_difference_type_impl<Iterators...>::type;

	constexpr zip_iterator() = default;
	constexpr explicit zip_iterator(Iterators... its) requires(sizeof...(Iterators) > 0):
		m_its(std::move(its)...)
	{
	}

	friend constexpr auto operator*(zip_iterator const & it) {
		auto const & [...its] = it.m_its;
		return tv::tuple<decltype(*its)...>(*its...);
	}
	friend constexpr auto operator+(zip_iterator it, bounded::constant_t<1> const offset) -> zip_iterator {
		if constexpr (sizeof...(Iterators) == 0) {
			std::unreachable();
		} else {
			auto && [...its] = std::move(it).m_its;
			return zip_iterator((std::move(its) + offset)...);
		}
	}
	friend constexpr auto operator+(zip_iterator it, bounded::convertible_to<difference_type> auto const offset) -> zip_iterator {
		auto && [...its] = std::move(it).m_its;
		return zip_iterator((std::move(its) + offset)...);
	}

	friend auto operator<=>(zip_iterator const &, zip_iterator const &) = default;
private:
	template<typename... Sentinels>
	friend struct zip_sentinel;
	template<typename... Sentinels>
	friend struct zip_smallest_sentinel;
	[[no_unique_address]] tv::tuple<Iterators...> m_its;
};

template<typename... Sentinels>
struct zip_sentinel {
	constexpr explicit zip_sentinel(Sentinels... sentinels):
		m_sentinels(std::move(sentinels)...)
	{
	}
	template<typename... Iterators> requires(sizeof...(Iterators) == sizeof...(Sentinels))
	friend constexpr auto operator==(zip_iterator<Iterators...> const & lhs, zip_sentinel const rhs) -> bool {
		auto const [...indexes] = bounded::index_sequence_struct<sizeof...(Sentinels)>();
		if ((... and (lhs.m_its[indexes] != rhs.m_sentinels[indexes]))) {
			return false;
		} else if ((... and (lhs.m_its[indexes] == rhs.m_sentinels[indexes]))) {
			return true;
		} else {
			throw std::runtime_error("Mismatched sizes for inputs to zip");
		}
	}
private:
	[[no_unique_address]] tv::tuple<Sentinels...> m_sentinels;
};

template<typename... Sentinels>
struct zip_smallest_sentinel {
	constexpr explicit zip_smallest_sentinel(Sentinels... sentinels):
		m_sentinels(std::move(sentinels)...)
	{
	}
	template<typename... Iterators> requires(sizeof...(Iterators) == sizeof...(Sentinels))
	friend constexpr auto operator==(zip_iterator<Iterators...> const & lhs, zip_smallest_sentinel const rhs) -> bool {
		auto const [...indexes] = bounded::index_sequence_struct<sizeof...(Sentinels)>();
		return (... or (lhs.m_its[indexes] == rhs.m_sentinels[indexes]));
	}
private:
	[[no_unique_address]] tv::tuple<Sentinels...> m_sentinels;
};

constexpr auto all_are_equal(auto const size, auto const ... sizes) -> bool {
	return (... and (size == sizes));
}

constexpr auto all_are_equal() -> bool {
	return true;
}

template<typename Range>
concept supports_begin = requires(Range r) {
	containers::begin(OPERATORS_FORWARD(r));
};

template<typename... Ranges>
concept all_support_begin = (... and supports_begin<Ranges>);

template<typename Range>
concept supports_end = requires(Range r) {
	containers::end(OPERATORS_FORWARD(r));
};

template<typename... Ranges>
concept all_support_end = (... and supports_end<Ranges>);

struct size_maybe_checked {};

export template<bool check_equal_sizes, range... Ranges>
struct zip_impl {
private:
	static constexpr auto all_are_sized = (... and sized_range<Ranges>);

	static constexpr auto as_tuple(Ranges && ... ranges) {
		if constexpr (all_are_sized and !check_equal_sizes and sizeof...(Ranges) != 0) {
			auto const min = bounded::min(containers::size(ranges)...);
			return tv::tuple(containers::take(OPERATORS_FORWARD(ranges), min)...);
		} else {
			return tv::tuple<Ranges...>(OPERATORS_FORWARD(ranges)...);
		}
	}

	decltype(as_tuple(bounded::declval<Ranges>()...)) m_ranges;

	static constexpr auto end_impl(auto && m_ranges) {
		auto && [...ranges] = OPERATORS_FORWARD(m_ranges);
		if constexpr (all_are_sized) {
			return zip_iterator(::containers::end(OPERATORS_FORWARD(ranges))...);
		} else if constexpr (check_equal_sizes) {
			return zip_sentinel(containers::end(OPERATORS_FORWARD(ranges))...);
		} else {
			return zip_smallest_sentinel(containers::end(OPERATORS_FORWARD(ranges))...);
		}
	}

	constexpr explicit zip_impl(size_maybe_checked, Ranges && ... ranges):
		m_ranges(as_tuple(OPERATORS_FORWARD(ranges)...))
	{
	}
	static constexpr auto maybe_check_sizes(Ranges const & ... ranges) -> size_maybe_checked {
		if constexpr (all_are_sized and check_equal_sizes) {
			if (!all_are_equal(containers::size(ranges)...)) {
				throw std::runtime_error("Mismatched sizes in zip");
			}
		}
		return size_maybe_checked();
	}
public:
	constexpr explicit zip_impl(Ranges && ... ranges):
		zip_impl(maybe_check_sizes(ranges...), OPERATORS_FORWARD(ranges)...)
	{
	}

	constexpr auto begin() const & requires all_support_begin<Ranges const &...> {
		auto && [...ranges] = m_ranges;
		return zip_iterator(::containers::begin(OPERATORS_FORWARD(ranges))...);
	}
	constexpr auto begin() & requires all_support_begin<Ranges &...> {
		auto && [...ranges] = m_ranges;
		return zip_iterator(::containers::begin(OPERATORS_FORWARD(ranges))...);
	}
	constexpr auto begin() && requires all_support_begin<Ranges &&...> {
		auto && [...ranges] = std::move(m_ranges);
		return zip_iterator(::containers::begin(OPERATORS_FORWARD(ranges))...);
	}

	constexpr auto end() const & requires all_support_end<Ranges const &...> {
		return end_impl(m_ranges);
	}
	constexpr auto end() & requires all_support_end<Ranges &...> {
		return end_impl(m_ranges);
	}
	constexpr auto end() && requires all_support_end<Ranges &&...> {
		return end_impl(std::move(m_ranges));
	}

	constexpr auto size() const requires(all_are_sized) {
		if constexpr (sizeof...(Ranges) == 0) {
			return 0_bi;
		} else {
			return containers::size(m_ranges[0_bi]);
		}
	}
};

export template<range... Ranges>
struct zip : private zip_impl<true, Ranges...> {
	constexpr explicit zip(Ranges && ... ranges):
		zip_impl<true, Ranges...>(OPERATORS_FORWARD(ranges)...)
	{
	}

	using zip_impl<true, Ranges...>::begin;
	using zip_impl<true, Ranges...>::end;
	using zip_impl<true, Ranges...>::size;
};

template<typename... Ranges>
zip(Ranges && ...) -> zip<Ranges...>;

export template<range... Ranges>
struct zip_smallest : private zip_impl<false, Ranges...> {
	constexpr explicit zip_smallest(Ranges && ... ranges):
		zip_impl<false, Ranges...>(OPERATORS_FORWARD(ranges)...)
	{
	}

	using zip_impl<false, Ranges...>::begin;
	using zip_impl<false, Ranges...>::end;
	using zip_impl<false, Ranges...>::size;
};

template<typename... Ranges>
zip_smallest(Ranges && ...) -> zip_smallest<Ranges...>;

} // namespace containers
