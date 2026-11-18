// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/conditional.hpp>

export module bounded.tombstone;

import bounded.arithmetic.operators;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.conditional_function;
import bounded.declval;
import bounded.integer;
import bounded.literal;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {
using namespace bounded::literal;

// Users should specialize `bounded::tombstone` for their types. The values
// should be accessed through `bounded::tombstone_traits`.

// An alternative design would be to implement this in terms of
// std::array<std::byte, sizeof(T)>. That approach is more correct because it
// does not claim that there is a T object somewhere, but it would not work with
// constexpr. This means all of the users of tombstone_traits would have to have
// a separate code path guarded by `if consteval` to avoid the small object
// optimization somehow.

template<typename>
constexpr auto is_constant = false;

template<auto value>
constexpr auto is_constant<integer<value, value>> = true;

template<typename T>
concept constant_integer = is_constant<T>;

export template<typename T>
struct tombstone {
	// `index` is a constant in the range `[0, max result of index(value)]`.
	static auto make(auto index) noexcept -> T = delete;
	// Returns -1_bi if there is an object present
	static constexpr auto index(T const &) noexcept -> constant_t<-1> {
		return -1_bi;
	}
};

template<bounded_integer T> requires (!constant_integer<T>)
struct tombstone<T> {
private:
	using underlying = typename T::underlying_type;
	static constexpr auto underlying_min = constant<numeric_traits::min_value<underlying>>;
	static constexpr auto underlying_max = constant<numeric_traits::max_value<underlying>>;
	static constexpr auto spare_below = numeric_traits::min_value<T> - underlying_min;
	static constexpr auto spare_above = underlying_max - numeric_traits::max_value<T>;
	using index_t = bounded::integer<
		-1,
		normalize<spare_below + spare_above - 1_bi>
	>;
public:
	static constexpr auto make(constant_integer auto const index) noexcept -> T {
		auto const value = conditional_function<index < spare_below>(
			index + underlying_min,
			index - spare_below + numeric_traits::max_value<T> + 1_bi
		);
		return T(underlying(value), unchecked);
	}
	static constexpr auto index(T const & value) noexcept -> index_t {
		auto const bounded_value = integer(value.m_value);
		auto const result =
			BOUNDED_CONDITIONAL(bounded_value < numeric_traits::min_value<T>, bounded_value - underlying_min,
			BOUNDED_CONDITIONAL(bounded_value > numeric_traits::max_value<T>, bounded_value + spare_below - numeric_traits::max_value<T> - 1_bi,
			-1_bi
		));
		return bounded::assume_in_range<index_t>(result);
	}
};

template<typename T>
constexpr auto max_tombstone_index =
	numeric_traits::max_value<decltype(tombstone<T>::index(declval<T const &>()))>;

template<typename T>
concept make_accepts_min_and_max = requires {
	{ tombstone<T>::make(0_bi) } -> std::same_as<T>;
	{ tombstone<T>::make(max_tombstone_index<T>) } -> std::same_as<T>;
};

template<typename Index, typename T>
concept tombstone_index =
	constant_integer<Index> and
	0_bi <= Index() and Index() <= max_tombstone_index<T>;

export template<typename T>
struct tombstone_traits {
	using index_t = decltype(tombstone<T>::index(declval<T>()));

	static_assert(numeric_traits::min_value<index_t> == -1_bi);
	
	static constexpr auto spare_representations = numeric_traits::max_value<index_t> + 1_bi;

	static_assert(spare_representations == 0_bi or make_accepts_min_and_max<T>);
	
	static constexpr auto make(tombstone_index<T> auto const index) noexcept -> T {
		static_assert(noexcept(tombstone<T>::make(index)));
		return tombstone<T>::make(index);
	}
	// Returns -1_bi if there is an object present
	static constexpr auto index(T const & value) noexcept -> index_t {
		static_assert(noexcept(tombstone<T>::index(value)));
		return tombstone<T>::index(value);
	}
};

export struct tombstone_tag {};

export template<auto>
struct tombstone_member;

template<typename Outer, typename Inner, Inner Outer::* pointer>
struct tombstone_member<pointer> {
	static constexpr auto make(auto const index) noexcept -> Outer {
		return Outer(
			tombstone_tag(),
			[=] noexcept { return tombstone<Inner>::make(index); }
		);
	}
	static constexpr auto index(Outer const & value) noexcept {
		return tombstone<Inner>::index(value.*pointer);
	}
};

} // namespace bounded