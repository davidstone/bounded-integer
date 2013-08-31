// Compile-time min and max of an arbitrary number of values
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

#ifndef RANGED_INTEGER_MINMAX_HPP_
#define RANGED_INTEGER_MINMAX_HPP_

namespace detail {

template<typename Predicate, typename Integer>
constexpr Integer minmax(Predicate const & predicate, Integer && integer) {
	return integer;
}
template<typename Predicate, typename Integer, typename... Integers>
constexpr auto minmax(Predicate const & predicate, Integer && integer, Integers && ... integers) -> decltype(predicate(integer, minmax(predicate, std::forward<Integers>(integers)...))) {
	return predicate(integer, minmax(predicate, std::forward<Integers>(integers)...));
}

class less {
public:
	template<typename T1, typename T2>
	constexpr auto operator()(T1 && t1, T2 && t2) -> decltype((t1 < t2) ? t1 : t2) {
		return (t1 < t2) ? t1 : t2;
	}
};
class greater {
public:
	template<typename T1, typename T2>
	constexpr auto operator()(T1 && t1, T2 && t2) -> decltype((t1 < t2) ? t1 : t2) {
		return (t1 > t2) ? t1 : t2;
	}
};
}	// namespace detail

template<typename... Integers>
constexpr auto min(Integers && ... integers) -> decltype(detail::minmax(detail::less{}, std::forward<Integers>(integers)...)) {
	return detail::minmax(detail::less{}, std::forward<Integers>(integers)...);
}

template<typename... Integers>
constexpr auto max(Integers && ... integers) -> decltype(detail::minmax(detail::greater{}, std::forward<Integers>(integers)...)) {
	return detail::minmax(detail::greater{}, std::forward<Integers>(integers)...);
}

#endif	// RANGED_INTEGER_MINMAX_HPP_
