// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.zip;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.is_empty;
import containers.is_iterator;
import containers.iter_difference_t;
import containers.range;
import containers.range_view;
import containers.size;

import bounded;
import numeric_traits;
import std_module;
import tv;

using namespace bounded::literal;

namespace containers {

template<typename... Iterators>
struct zip_iterator;

template<std::size_t... indexes>
constexpr auto dereference(auto const & tuple, std::index_sequence<indexes...>) {
	return tv::tie(*tuple[bounded::constant<indexes>]...);
}

template<typename... Iterators, std::size_t... indexes>
constexpr auto add(tv::tuple<Iterators...> const & tuple, auto const offset, std::index_sequence<indexes...>) -> zip_iterator<Iterators...> {
	return zip_iterator<Iterators...>((tuple[bounded::constant<indexes>] + offset)...);
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

template<typename... Iterators>
struct zip_iterator {
	using difference_type = typename zip_difference_type_impl<Iterators...>::type;

	constexpr zip_iterator() = default;
	constexpr explicit zip_iterator(Iterators... its) requires(sizeof...(Iterators) > 0):
		m_its(std::move(its)...)
	{
	}

	friend constexpr auto operator*(zip_iterator const & it) {
		return dereference(it.m_its, indexes());
	}
	friend constexpr auto operator+(zip_iterator, bounded::constant_t<1>) -> zip_iterator requires(sizeof...(Iterators) == 0) {
		std::unreachable();
	}
	friend constexpr auto operator+(zip_iterator const & it, difference_type const offset) -> zip_iterator {
		return add(it.m_its, offset, indexes());
	}

	friend auto operator<=>(zip_iterator const &, zip_iterator const &) = default;
private:
	using indexes = std::make_index_sequence<sizeof...(Iterators)>;
	[[no_unique_address]] tv::tuple<Iterators...> m_its;
};

constexpr auto all_are_equal(auto const size, auto const ... sizes) -> bool {
	return (... and (size == sizes));
}

constexpr auto all_are_equal() -> bool {
	return true;
}

constexpr auto get_first_size(auto const r, auto...) {
	return containers::size(r);
}

constexpr auto get_first_size() {
	return 0_bi;
}

export constexpr auto zip(range auto && ... rs) {
	// TODO: Support unsized ranges
	if (!all_are_equal(containers::size(rs)...)) {
		throw std::runtime_error("Mismatched sizes in zip");
	}
	return containers::range_view(
		zip_iterator(containers::begin(rs)...),
		get_first_size(rs...)
	);
}

} // namespace containers

static_assert(containers::iterator<containers::zip_iterator<>>);

static_assert(containers::is_empty(containers::zip()));

constexpr auto a1 = containers::array({1, 2});
constexpr auto a2 = containers::array({3, 4});
constexpr auto expected = containers::array({tv::tuple(1, 3), tv::tuple(2, 4)});
static_assert(containers::equal(containers::zip(a1), a1));
static_assert(containers::equal(containers::zip(a1, a2), expected));