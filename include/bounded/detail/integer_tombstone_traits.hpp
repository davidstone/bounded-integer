// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/conditional.hpp>
#include <bounded/tombstone_traits.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {

template<bounded_integer T>
struct tombstone_traits<T> {
private:
	using underlying = typename T::underlying_type;
	static constexpr auto underlying_min = constant<numeric_traits::min_value<underlying>>;
	static constexpr auto underlying_max = constant<numeric_traits::max_value<underlying>>;
	static constexpr auto spare_below = numeric_traits::min_value<T> - underlying_min;
	static constexpr auto spare_above = underlying_max - numeric_traits::max_value<T>;
public:
	static constexpr auto spare_representations = BOUNDED_CONDITIONAL(std::is_empty_v<T>,
		constant<0>,
		spare_below + spare_above
	);

	template<typename Index> requires(Index() < spare_representations)
	static constexpr auto make(Index const index) noexcept {
		auto const value = BOUNDED_CONDITIONAL(index < spare_below,
			index + underlying_min,
			index - spare_below + numeric_traits::max_value<T> + constant<1>
		);
		return T(underlying(value), unchecked);
	}
	static constexpr auto index(T const & value) {
		auto const bounded_value = integer(value.m_value);
		return
			BOUNDED_CONDITIONAL(bounded_value < numeric_traits::min_value<T>, bounded_value - underlying_min,
			BOUNDED_CONDITIONAL(bounded_value > numeric_traits::max_value<T>, bounded_value + spare_below - numeric_traits::max_value<T> - constant<1>,
			constant<-1>
		));
	}
};

} // namespace bounded