// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/iterator_adapter.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/reference_wrapper.hpp>

#include <bounded/integer.hpp>

#include <type_traits>
#include <utility>

namespace containers {

template<typename Range, typename Traits>
struct adapt {
	constexpr adapt(Range && range, Traits && traits) noexcept(std::is_nothrow_move_constructible_v<Range> and std::is_nothrow_move_constructible_v<Traits>):
		m_range(BOUNDED_FORWARD(range)),
		m_traits(BOUNDED_FORWARD(traits))
	{
	}
	
	friend constexpr auto begin(adapt const & adapted) {
		return containers::adapt_iterator(begin(adapted.m_range), reference_wrapper(adapted.m_traits));
	}
	friend constexpr auto begin(adapt & adapted) {
		return containers::adapt_iterator(begin(adapted.m_range), reference_wrapper(adapted.m_traits));
	}
	friend constexpr auto begin(adapt && adapted) {
		return containers::adapt_iterator(begin(std::move(adapted).m_range), reference_wrapper(adapted.m_traits));
	}
	friend constexpr auto end(adapt const & adapted) {
		return containers::adapt_iterator(end(adapted.m_range), reference_wrapper(adapted.m_traits));
	}
	friend constexpr auto end(adapt & adapted) {
		return containers::adapt_iterator(end(adapted.m_range), reference_wrapper(adapted.m_traits));
	}
	friend constexpr auto end(adapt && adapted) {
		return containers::adapt_iterator(end(std::move(adapted).m_range), reference_wrapper(adapted.m_traits));
	}
	
	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(adapt)
	
private:
	Range m_range;
	Traits m_traits;
};

template<typename Range, typename Traits>
adapt(Range &&, Traits &&) -> adapt<Range, Traits>;

}	// namespace containers
