// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform_iterator.hpp>

#include <bounded/integer.hpp>

#include <concepts>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Base>
struct move_iterator_sentinel {
	constexpr explicit move_iterator_sentinel(Base it):
		m_it(std::move(it))
	{
	}
	constexpr auto const & base() const {
		return m_it;
	}
	friend constexpr auto operator<=>(move_iterator_sentinel const & lhs, move_iterator_sentinel const & rhs);
	template<iterator Iterator> requires requires(Iterator it, Base base) { it.base() <=> base; }
	friend constexpr auto operator<=>(Iterator const & lhs, move_iterator_sentinel const & rhs) {
		return lhs.base() <=> rhs.base();
	}
	template<iterator Iterator> requires requires(Iterator it, Base base) { it.base() == base; }
	friend constexpr auto operator==(Iterator const & lhs, move_iterator_sentinel const & rhs) {
		return lhs.base() == rhs.base();
	}
private:
	Base m_it;
};

} // namespace detail

template<typename Iterator>
constexpr auto move_iterator(Iterator it_) {
	if constexpr (iterator<Iterator>) {
		return ::containers::transform_iterator_dereference(std::move(it_), [](Iterator const & it) -> decltype(auto) {
			using base_result = decltype(*it);
			using result = std::conditional_t<
				std::is_reference_v<base_result>,
				std::remove_reference_t<base_result> &&,
				base_result
			>;
			return static_cast<result>(*it);
		});
	} else {
		return detail::move_iterator_sentinel{std::move(it_)};
	}
}

}	// namespace containers
