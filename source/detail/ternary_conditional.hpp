// This fakes a ternary conditional operator overload (evalutes both arguments)
// Copyright (C) 2013 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef RANGED_INTEGER_TERNARY_CONDITIONAL_HPP_
#define RANGED_INTEGER_TERNARY_CONDITIONAL_HPP_

#include "common_type.hpp"
#include <utility>

namespace detail {

template<typename LHS, typename RHS, typename Compare, typename result_type = typename std::common_type<LHS, RHS>::type>
constexpr result_type ternary_conditional(Compare && compare, LHS && lhs, RHS && rhs) {
	return std::forward<Compare>(compare)(lhs, rhs) ?
		static_cast<result_type>(std::forward<LHS>(lhs)) :
		static_cast<result_type>(std::forward<RHS>(rhs));
}


class less {
public:
	template<typename LHS, typename RHS>
	constexpr typename std::common_type<LHS, RHS>::type operator()(LHS && lhs, RHS && rhs) {
		return ternary_conditional(compare{}, lhs, rhs);
	}
private:
	class compare {
	public:
		template<typename LHS, typename RHS>
		constexpr bool operator()(LHS const & lhs, RHS const & rhs) {
			return lhs < rhs;
		}
	};
};
class greater {
public:
	template<typename LHS, typename RHS>
	constexpr typename std::common_type<LHS, RHS>::type operator()(LHS && lhs, RHS && rhs) {
		return ternary_conditional(compare{}, lhs, rhs);
	}
private:
	class compare {
	public:
		template<typename LHS, typename RHS>
		constexpr bool operator()(LHS const & lhs, RHS const & rhs) {
			return lhs > rhs;
		}
	};
};
}	// namespace detail

#endif	// RANGED_INTEGER_TERNARY_CONDITIONAL_HPP_
