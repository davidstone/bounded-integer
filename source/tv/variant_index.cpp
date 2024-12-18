// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.variant_index;

import tv.get_index;

import bounded;
import std_module;

namespace tv {

struct non_constructible {
	non_constructible() = delete;
};

template<std::size_t size>
struct variant_selector_c {
	using type = bounded::integer<0, bounded::normalize<size - 1U>>;
};

template<>
struct variant_selector_c<0> {
	using type = non_constructible;
};

template<std::size_t size>
using variant_selector = typename variant_selector_c<size>::type;

export template<typename... Ts>
struct variant_index {
	using integer_index = variant_selector<sizeof...(Ts)>;
	constexpr explicit variant_index(integer_index const index):
		m_index(index)
	{
	}
	constexpr auto integer() const -> integer_index {
		return m_index;
	}
	friend constexpr auto operator==(variant_index, variant_index) -> bool = default;
	friend constexpr auto operator==(variant_index const lhs, integer_index const rhs) -> bool {
		return lhs.integer() == rhs;
	}
	template<typename T> requires (... or std::same_as<T, Ts>)
	friend constexpr auto operator==(variant_index const lhs, bounded::type_t<T> const rhs) -> bool {
		return [=]<std::size_t... indexes>(std::index_sequence<indexes...>) {
			bool matched = false;
			(..., (bounded::constant<indexes> == lhs.integer() ?
				void(matched = (get_type(bounded::constant<indexes>, bounded::type<Ts>...) == rhs)) :
				void()
			));
			return matched;
		}(std::make_index_sequence<sizeof...(Ts)>());
	}
private:
	[[no_unique_address]] integer_index m_index;

	constexpr explicit variant_index(bounded::tombstone_tag, auto const make):
		m_index(make())
	{
	}
	friend bounded::tombstone<variant_index>;
	friend bounded::tombstone_member<&variant_index::m_index>;
};

} // namespace tv

template<typename... Ts>
struct bounded::tombstone<tv::variant_index<Ts...>> : bounded::tombstone_member<&tv::variant_index<Ts...>::m_index> {
};

using namespace bounded::literal;

static_assert(!bounded::constructible_from<tv::variant_index<>, bounded::constant_t<0>>);

static_assert(bounded::constructible_from<tv::variant_index<int>, bounded::constant_t<0>>);
static_assert(!bounded::constructible_from<tv::variant_index<int>, bounded::constant_t<1>>);

static_assert(bounded::constructible_from<tv::variant_index<int, long>, bounded::constant_t<0>>);
static_assert(bounded::constructible_from<tv::variant_index<int, long>, bounded::constant_t<1>>);
static_assert(!bounded::constructible_from<tv::variant_index<int, long>, bounded::constant_t<2>>);

static_assert(bounded::constructible_from<tv::variant_index<int, int>, bounded::constant_t<0>>);
static_assert(bounded::constructible_from<tv::variant_index<int, int>, bounded::constant_t<1>>);
static_assert(!bounded::constructible_from<tv::variant_index<int, int>, bounded::constant_t<2>>);

static_assert(tv::variant_index<int>(0_bi).integer() == 0_bi);

static_assert(tv::variant_index<int, long>(0_bi).integer() == 0_bi);
static_assert(tv::variant_index<int, long>(1_bi).integer() == 1_bi);
static_assert(tv::variant_index<int, int>(0_bi).integer() == 0_bi);
static_assert(tv::variant_index<int, int>(1_bi).integer() == 1_bi);

static_assert(tv::variant_index<int>(0_bi) == tv::variant_index<int>(0_bi));
static_assert(tv::variant_index<int>(0_bi) == 0_bi);
static_assert(tv::variant_index<int>(0_bi) == bounded::type<int>);

static_assert(tv::variant_index<int, long>(0_bi) == tv::variant_index<int, long>(0_bi));
static_assert(tv::variant_index<int, long>(0_bi) != tv::variant_index<int, long>(1_bi));
static_assert(tv::variant_index<int, long>(1_bi) == tv::variant_index<int, long>(1_bi));
static_assert(tv::variant_index<int, long>(0_bi) == 0_bi);
static_assert(tv::variant_index<int, long>(1_bi) == 1_bi);
static_assert(tv::variant_index<int, long>(0_bi) == bounded::type<int>);
static_assert(tv::variant_index<int, long>(0_bi) != bounded::type<long>);
static_assert(tv::variant_index<int, long>(1_bi) == bounded::type<long>);
static_assert(tv::variant_index<int, long>(1_bi) != bounded::type<int>);

static_assert(tv::variant_index<int, int>(0_bi) == tv::variant_index<int, int>(0_bi));
static_assert(tv::variant_index<int, int>(0_bi) != tv::variant_index<int, int>(1_bi));
static_assert(tv::variant_index<int, int>(1_bi) == tv::variant_index<int, int>(1_bi));
static_assert(tv::variant_index<int, int>(0_bi) == 0_bi);
static_assert(tv::variant_index<int, int>(1_bi) == 1_bi);
static_assert(tv::variant_index<int, int>(0_bi) == bounded::type<int>);
static_assert(tv::variant_index<int, int>(1_bi) == bounded::type<int>);
