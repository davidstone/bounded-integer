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
import containers.range_view;
import containers.size;
import containers.sized_range;

import bounded;
import numeric_traits;
import std_module;
import tv;

using namespace bounded::literal;

namespace containers {

template<typename... Iterators>
struct zip_iterator;

template<typename... Ts>
constexpr auto ref_or_value_tuple(Ts && ... args) {
	return tv::tuple<Ts...>(OPERATORS_FORWARD(args)...);
}

template<std::size_t... indexes>
constexpr auto dereference(auto const & tuple, std::index_sequence<indexes...>) {
	return ref_or_value_tuple(*tuple[bounded::constant<indexes>]...);
}

template<typename... Iterators, std::size_t... indexes>
constexpr auto add(tv::tuple<Iterators...> tuple, auto const offset, std::index_sequence<indexes...>) -> zip_iterator<Iterators...> {
	return zip_iterator<Iterators...>((std::move(tuple)[bounded::constant<indexes>] + offset)...);
}

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

template<typename... Iterators>
struct zip_iterator {
	using difference_type = typename zip_difference_type_impl<Iterators...>::type;

	constexpr zip_iterator() = default;
	constexpr explicit zip_iterator(Iterators... its) requires(sizeof...(Iterators) > 0):
		m_its(std::move(its)...)
	{
	}

	friend constexpr auto operator*(zip_iterator const & it) {
		return dereference(it.m_its, indexes);
	}
	friend constexpr auto operator+(zip_iterator it, bounded::constant_t<1>) -> zip_iterator {
		if constexpr (sizeof...(Iterators) == 0) {
			std::unreachable();
		} else {
			return add(std::move(it).m_its, 1_bi, indexes);
		}
	}
	friend constexpr auto operator+(zip_iterator it, bounded::convertible_to<difference_type> auto const offset) -> zip_iterator {
		return add(std::move(it).m_its, offset, indexes);
	}

	friend auto operator<=>(zip_iterator const &, zip_iterator const &) = default;
private:
	template<typename... Sentinels>
	friend struct zip_sentinel;
	static constexpr auto indexes = std::make_index_sequence<sizeof...(Iterators)>();
	[[no_unique_address]] tv::tuple<Iterators...> m_its;
};

template<std::size_t... indexes>
constexpr auto all(auto const & lhs, auto const & rhs, std::index_sequence<indexes...>, auto const predicate) {
	return (... and predicate(lhs[bounded::constant<indexes>], rhs[bounded::constant<indexes>]));
}

template<typename... Sentinels>
struct zip_sentinel {
	constexpr explicit zip_sentinel(Sentinels... sentinels):
		m_sentinels(std::move(sentinels)...)
	{
	}
	template<typename... Iterators> requires(sizeof...(Iterators) == sizeof...(Sentinels))
	friend constexpr auto operator==(zip_iterator<Iterators...> const & lhs, zip_sentinel const rhs) -> bool {
		constexpr auto indexes = std::make_index_sequence<sizeof...(Sentinels)>();
		if (all(lhs.m_its, rhs.m_sentinels, indexes, std::not_equal_to())) {
			return false;
		} else if (all(lhs.m_its, rhs.m_sentinels, indexes, std::equal_to())) {
			return true;
		} else {
			throw std::runtime_error("Mismatched sizes for inputs to zip");
		}
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

template<std::size_t... indexes>
constexpr auto make_zip_iterator(auto && ranges, std::index_sequence<indexes...>, auto get) {
	return zip_iterator(
		get(OPERATORS_FORWARD(ranges)[bounded::constant<indexes>])...
	);
}

// https://github.com/llvm/llvm-project/issues/59513
struct use_begin {
	static constexpr auto operator()(auto && r) {
		return containers::begin(OPERATORS_FORWARD(r));
	}
};
struct use_end {
	static constexpr auto operator()(auto && r) {
		return containers::end(OPERATORS_FORWARD(r));
	}
};

template<std::size_t... indexes>
constexpr auto make_zip_sentinel(auto && ranges, std::index_sequence<indexes...>) {
	return zip_sentinel(
		containers::end(OPERATORS_FORWARD(ranges)[bounded::constant<indexes>])...
	);
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

export template<range... Ranges>
struct zip {
private:
	static constexpr auto all_are_sized = (... and sized_range<Ranges>);
	static constexpr auto indexes = std::make_index_sequence<sizeof...(Ranges)>();
	tv::tuple<Ranges...> m_ranges;

	static constexpr auto end_impl(auto && ranges) {
		if constexpr (all_are_sized) {
			return ::containers::make_zip_iterator(OPERATORS_FORWARD(ranges), indexes, use_end());
		} else {
			return ::containers::make_zip_sentinel(OPERATORS_FORWARD(ranges), indexes);
		}
	}

	struct size_maybe_checked {};
	constexpr explicit zip(size_maybe_checked, Ranges && ... ranges):
		m_ranges(OPERATORS_FORWARD(ranges)...)
	{
	}
	static constexpr auto maybe_check_sizes(Ranges const & ... ranges) -> size_maybe_checked {
		if constexpr (all_are_sized) {
			if (!all_are_equal(containers::size(ranges)...)) {
				throw std::runtime_error("Mismatched sizes in zip");
			}
		}
		return size_maybe_checked();
	}
public:
	constexpr explicit zip(Ranges && ... ranges):
		zip(maybe_check_sizes(ranges...), OPERATORS_FORWARD(ranges)...)
	{
	}

	constexpr auto begin() const & requires all_support_begin<Ranges const &...> {
		return make_zip_iterator(m_ranges, indexes, use_begin());
	}
	constexpr auto begin() & requires all_support_begin<Ranges &...> {
		return make_zip_iterator(m_ranges, indexes, use_begin());
	}
	constexpr auto begin() && requires all_support_begin<Ranges &&...> {
		return make_zip_iterator(std::move(m_ranges), indexes, use_begin());
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

template<typename... Ranges>
zip(Ranges && ...) -> zip<Ranges...>;

} // namespace containers
