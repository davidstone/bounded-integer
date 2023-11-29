// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.adapt;

import containers.adapt_iterator;
import containers.begin_end;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.iter_value_t;
import containers.range;
import containers.reference_or_value;

export import containers.common_iterator_functions;

import bounded;
import std_module;

namespace containers {

template<typename Range, typename Traits>
concept sized_adapted_range = requires(Traits const traits, Range const range) {
	traits.get_size(range);
};

template<typename Range, typename Traits>
concept statically_sized_adapted_range = sized_adapted_range<Range, Traits> and requires(Traits const traits) {
	std::remove_reference_t<decltype(traits)>::template get_size<Range>();
};

template<typename Traits, typename Range>
concept has_iterator_traits = requires(Traits const traits) {
	traits.iterator_traits(bounded::declval<Range>());
};

template<typename Traits>
constexpr auto get_iterator_traits(Traits const & range_traits, auto & r) -> decltype(auto) {
	if constexpr (requires { range_traits.iterator_traits(r); }) {
		return range_traits.iterator_traits(r);
	} else {
		return reference_or_value<Traits>(range_traits);
	}
}

export template<range Range, typename Traits>
struct adapt {
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Traits m_traits;

	constexpr auto iterator_traits() const -> decltype(auto) {
		return get_iterator_traits(m_traits, m_range);
	}
	constexpr auto iterator_traits() -> decltype(auto) {
		return get_iterator_traits(m_traits, m_range);
	}
public:
	constexpr adapt(Range && range, Traits traits):
		m_range(OPERATORS_FORWARD(range)),
		m_traits(std::move(traits))
	{
	}
	
	constexpr auto begin() const & -> iterator auto requires range<Range const &> {
		return containers::adapt_iterator(
			m_traits.get_begin(m_range),
			iterator_traits()
		);
	}
	constexpr auto begin() & -> iterator auto requires range<Range &> {
		return containers::adapt_iterator(
			m_traits.get_begin(m_range),
			iterator_traits()
		);
	}
	constexpr auto begin() && -> iterator auto requires range<Range &&> {
		auto traits = iterator_traits();
		return containers::adapt_iterator(
			m_traits.get_begin(std::move(*this).m_range),
			std::move(traits)
		);
	}
	constexpr auto end() const & requires range<Range const &> {
		return containers::adapt_iterator_or_sentinel(
			m_traits.get_end(m_range),
			iterator_traits()
		);
	}
	constexpr auto end() & requires range<Range &> {
		return containers::adapt_iterator_or_sentinel(
			m_traits.get_end(m_range),
			iterator_traits()
		);
	}
	constexpr auto end() && requires range<Range &&> {
		auto traits = iterator_traits();
		return containers::adapt_iterator_or_sentinel(
			m_traits.get_end(std::move(*this).m_range),
			std::move(traits)
		);
	}

	constexpr auto size() const requires sized_adapted_range<Range, Traits> {
		return m_traits.get_size(m_range);
	}
	static constexpr auto size() requires statically_sized_adapted_range<Range, Traits> {
		return std::remove_reference_t<decltype(m_traits)>::template get_size<Range>();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto base() const & -> Range const & {
		return m_range;
	}
	constexpr auto base() && -> Range && {
		return std::move(m_range);
	}
};

template<typename Range, typename Traits>
adapt(Range &&, Traits) -> adapt<Range, Traits>;

}	// namespace containers
