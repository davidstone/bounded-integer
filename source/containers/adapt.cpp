// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.adapt;

import containers.begin_end;
import containers.iter_value_t;
import containers.iterator_adapter;
import containers.reference_wrapper;

export import containers.common_iterator_functions;

import bounded;
import std_module;

namespace containers {

template<typename Range, typename Traits>
concept sized_adapted_range = requires(Traits const traits, Range const range) {
	::containers::unwrap(traits).get_size(range);
};

template<typename Range, typename Traits>
concept statically_sized_adapted_range = sized_adapted_range<Range, Traits> and requires(Traits const traits) {
	std::remove_reference_t<decltype(::containers::unwrap(traits))>::template get_size<Range>();
};

export template<typename Range, typename Traits>
struct adapt {
	constexpr adapt(Range && range, Traits traits):
		m_range(OPERATORS_FORWARD(range)),
		m_traits(std::move(traits))
	{
	}
	
	constexpr auto begin() const & {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_begin(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto begin() & {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_begin(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto begin() && {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_begin(std::move(*this).m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() const & {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_end(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() & {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_end(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() && {
		return containers::adapt_iterator(
			::containers::unwrap(m_traits).get_end(std::move(*this).m_range),
			adapt_iterator_traits(m_traits)
		);
	}

	constexpr auto size() const requires sized_adapted_range<Range, Traits> {
		return ::containers::unwrap(m_traits).get_size(m_range);
	}
	static constexpr auto size() requires statically_sized_adapted_range<Range, Traits> {
		return std::remove_reference_t<decltype(::containers::unwrap(m_traits))>::template get_size<Range>();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto base() const & -> Range const &{
		return m_range;
	}
	constexpr auto base() && -> Range && {
		return std::move(m_range);
	}

private:
	using adapt_iterator_traits = std::conditional_t<
		std::is_empty_v<Traits> and std::is_copy_assignable_v<Traits>,
		Traits,
		decltype(std::reference_wrapper(bounded::declval<Traits const &>()))
	>;

	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Traits m_traits;
};

template<typename Range, typename Traits>
adapt(Range &&, Traits) -> adapt<Range, Traits>;

}	// namespace containers
