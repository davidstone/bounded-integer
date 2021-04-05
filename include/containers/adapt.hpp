// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_functions.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/reference_wrapper.hpp>
#include <containers/value_type.hpp>

#include <bounded/integer.hpp>

#include <operators/bracket.hpp>

#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename Range, typename Traits>
concept sized_adapted_range = requires(Traits const traits, Range const range) { containers::unwrap(traits).get_size(range); };

template<typename Range, typename Traits>
concept statically_sized_adapted_range = sized_adapted_range<Range, Traits> and requires(Traits const traits) {
	std::remove_reference_t<decltype(containers::unwrap(traits))>::template get_size<Range>();
};

} // namespace detail

template<typename Range, typename Traits>
struct adapt {
private:
	using adapt_iterator_traits = std::conditional_t<
		std::is_empty_v<Traits> and std::is_copy_assignable_v<Traits>,
		Traits,
		decltype(reference_wrapper(std::declval<Traits const &>()))
	>;

	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Traits m_traits;

public:
	using iterator = decltype(containers::adapt_iterator(
		containers::unwrap(std::declval<adapt_iterator_traits &>()).get_begin(std::declval<Range &>()),
		std::declval<adapt_iterator_traits>()
	));
	using const_iterator = decltype(containers::adapt_iterator(
		containers::unwrap(std::declval<adapt_iterator_traits &>()).get_begin(std::declval<Range const &>()),
		std::declval<adapt_iterator_traits>()
	));
	using value_type = iter_value_t<iterator>;
	using size_type = bounded::integer<
		0,
		bounded::normalize<static_cast<std::uintmax_t>(bounded::max_value<typename std::iterator_traits<iterator>::difference_type>)>
	>;
	
	constexpr adapt(Range && range, Traits traits):
		m_range(OPERATORS_FORWARD(range)),
		m_traits(std::move(traits))
	{
	}
	
	constexpr auto begin() const & {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_begin(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto begin() & {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_begin(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto begin() && {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_begin(std::move(*this).m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() const & {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_end(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() & {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_end(m_range),
			adapt_iterator_traits(m_traits)
		);
	}
	constexpr auto end() && {
		return containers::adapt_iterator(
			containers::unwrap(m_traits).get_end(std::move(*this).m_range),
			adapt_iterator_traits(m_traits)
		);
	}

	constexpr auto size() const requires detail::sized_adapted_range<Range, Traits> {
		return containers::unwrap(m_traits).get_size(m_range);
	}
	static constexpr auto size() requires detail::statically_sized_adapted_range<Range, Traits> {
		return std::remove_reference_t<decltype(containers::unwrap(m_traits))>::template get_size<Range>();
	}
	
	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
};

template<typename Range, typename Traits>
adapt(Range &&, Traits) -> adapt<Range, Traits>;

}	// namespace containers
