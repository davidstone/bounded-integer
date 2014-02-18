// Applies any other policy with dynamic bounds
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_POLICY_DYNAMIC_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_DYNAMIC_POLICY_HPP_

#include "is_overflow_policy.hpp"

#include "../arithmetic_operators.hpp"
#include "../class.hpp"
#include "../comparison_operators.hpp"
#include "../enable_if.hpp"
#include "../make_bounded.hpp"
#include "../minmax.hpp"

#include <cstdint>

namespace bounded_integer {

// Currently does not work with a stateful base policy due to a bug in gcc that
// causes a crash.
template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy>
class dynamic_policy {
public:
	using overflow_policy_tag = void;
	using underlying_type = bounded_integer<static_minimum, static_maximum, overflow_policy>;

	constexpr dynamic_policy() noexcept:
		m_min(make_bounded<static_minimum>()),
		m_max(make_bounded<static_maximum>()) {
	}
	constexpr dynamic_policy(underlying_type minimum, underlying_type maximum) noexcept:
		m_min(std::move(minimum)),
		m_max(std::move(maximum)) {
	}
	constexpr dynamic_policy(dynamic_policy const &) noexcept = default;
	constexpr dynamic_policy(dynamic_policy &&) noexcept = default;
	constexpr dynamic_policy(dynamic_policy const volatile & other) noexcept:
		m_min(other.m_min),
		m_max(other.m_max) {
	}
	constexpr dynamic_policy(dynamic_policy volatile && other) noexcept:
		m_min(std::move(other.m_min)),
		m_max(std::move(other.m_max)) {
	}

	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	constexpr dynamic_policy(T &&) noexcept:
		dynamic_policy() {
	}

	dynamic_policy & operator=(dynamic_policy const & other) noexcept {
		m_min.unchecked_assignment(other.m_min);
		m_max.unchecked_assignment(other.m_max);
		return *this;
	}
	dynamic_policy & operator=(dynamic_policy && other) noexcept {
		m_min.unchecked_assignment(std::move(other.m_min));
		m_max.unchecked_assignment(std::move(other.m_max));
		return *this;
	}

	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	dynamic_policy & operator=(T && other) noexcept {
		return *this = dynamic_policy(std::forward<T>(other));
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	dynamic_policy volatile & operator=(T && other) volatile noexcept {
		return *this = dynamic_policy(std::forward<T>(other));
	}


	template<typename T, typename Minimum, typename Maximum>
	constexpr intmax_t assignment(T && value, Minimum && minimum, Maximum && maximum) const
		noexcept(noexcept(overflow_policy{}.assignment(
			std::forward<T>(value),
			::bounded_integer::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			::bounded_integer::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return overflow_policy{}.assignment(
			std::forward<T>(value),
			::bounded_integer::max(std::forward<Minimum>(minimum), m_min),
			::bounded_integer::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr intmax_t assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(overflow_policy{}.assignment(
			std::forward<T>(value),
			::bounded_integer::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			::bounded_integer::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return overflow_policy{}.assignment(
			std::forward<T>(value),
			::bounded_integer::max(std::forward<Minimum>(minimum), m_min),
			::bounded_integer::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	
	constexpr underlying_type const & min() const noexcept {
		return m_min;
	}
	constexpr underlying_type const volatile & min() const volatile noexcept {
		return m_min;
	}
	constexpr underlying_type const & max() const noexcept {
		return m_max;
	}
	constexpr underlying_type const volatile & max() const volatile noexcept {
		return m_max;
	}
	
	static constexpr bool is_modulo = overflow_policy::is_modulo;
	static constexpr bool overflow_is_error = overflow_policy::overflow_is_error;
private:
	underlying_type m_min;
	underlying_type m_max;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_DYNAMIC_POLICY_HPP_
