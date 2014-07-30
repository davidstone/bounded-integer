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

#include "../class.hpp"
#include "../enable_if.hpp"
#include "../make.hpp"
#include "../minmax.hpp"
#include "../operators/arithmetic.hpp"
#include "../operators/comparison.hpp"

#include <cstdint>

namespace bounded {

template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
class dynamic_policy : private overflow_policy {
public:
	using overflow_policy_tag = void;
	using underlying_type = integer<static_minimum, static_maximum, overflow_policy, storage>;

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_policy(underlying_type minimum, underlying_type maximum, OverflowPolicy && policy) noexcept:
		overflow_policy(std::forward<OverflowPolicy>(policy)),
		m_min(std::move(minimum)),
		m_max(std::move(maximum)) {
	}
	constexpr dynamic_policy(underlying_type minimum, underlying_type maximum) noexcept:
		m_min(std::move(minimum)),
		m_max(std::move(maximum)) {
	}
	constexpr dynamic_policy() noexcept:
		dynamic_policy(make<static_minimum>(), make<static_maximum>()) {
	}
	constexpr dynamic_policy(dynamic_policy const &) noexcept = default;
	constexpr dynamic_policy(dynamic_policy &&) noexcept = default;
	constexpr dynamic_policy(dynamic_policy const volatile & other) noexcept:
		dynamic_policy(other.m_min, other.m_max, static_cast<overflow_policy const volatile &>(other)) {
	}
	constexpr dynamic_policy(dynamic_policy volatile && other) noexcept:
		dynamic_policy(std::move(other.m_min), std::move(other.m_max), static_cast<overflow_policy volatile &&>(other)) {
	}

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_policy(OverflowPolicy && policy) noexcept:
		dynamic_policy(make<static_minimum>(), make<static_maximum>(), std::forward<OverflowPolicy>(policy)) {
	}

	auto && operator=(dynamic_policy const & other) noexcept {
		m_min.unchecked_assignment(other.m_min);
		m_max.unchecked_assignment(other.m_max);
		return *this;
	}
	auto && operator=(dynamic_policy && other) noexcept {
		m_min.unchecked_assignment(std::move(other.m_min));
		m_max.unchecked_assignment(std::move(other.m_max));
		return *this;
	}

	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) noexcept {
		return *this = dynamic_policy(std::forward<T>(other));
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) volatile noexcept {
		return *this = dynamic_policy(std::forward<T>(other));
	}


	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			::bounded::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), m_min),
			::bounded::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			::bounded::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), m_min),
			::bounded::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	
	constexpr auto && min() const & noexcept {
		return m_min;
	}
	constexpr auto && min() const volatile & noexcept {
		return m_min;
	}
	auto && min() & noexcept {
		return m_min;
	}
	auto && min() volatile & noexcept {
		return m_min;
	}
	auto && min() && noexcept {
		return std::move(m_min);
	}

	constexpr auto && max() const & noexcept {
		return m_max;
	}
	constexpr auto && max() const volatile & noexcept {
		return m_max;
	}
	auto && max() & noexcept {
		return m_max;
	}
	auto && max() volatile & noexcept {
		return m_max;
	}
	auto && max() && noexcept {
		return std::move(m_max);
	}
	
	static constexpr bool is_modulo = overflow_policy::is_modulo;
	static constexpr bool overflow_is_error = overflow_policy::overflow_is_error;

private:
	underlying_type m_min;
	underlying_type m_max;
};



template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
class dynamic_max_policy : private overflow_policy {
public:
	using overflow_policy_tag = void;
	using underlying_type = integer<static_minimum, static_maximum, overflow_policy, storage>;

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_max_policy(underlying_type maximum, OverflowPolicy && policy) noexcept:
		overflow_policy(std::forward<OverflowPolicy>(policy)),
		m_max(std::move(maximum)) {
	}
	constexpr dynamic_max_policy(underlying_type maximum) noexcept:
		m_max(std::move(maximum)) {
	}
	constexpr dynamic_max_policy() noexcept:
		dynamic_max_policy(make<static_maximum>()) {
	}
	constexpr dynamic_max_policy(dynamic_max_policy const &) noexcept = default;
	constexpr dynamic_max_policy(dynamic_max_policy &&) noexcept = default;
	constexpr dynamic_max_policy(dynamic_max_policy const volatile & other) noexcept:
		dynamic_max_policy(other.m_max) {
	}
	constexpr dynamic_max_policy(dynamic_max_policy volatile && other) noexcept:
		dynamic_max_policy(std::move(other.m_max)) {
	}

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_max_policy(OverflowPolicy && policy) noexcept:
		dynamic_max_policy(make<static_maximum>(), std::forward<OverflowPolicy>(policy)) {
	}

	auto && operator=(dynamic_max_policy const & other) noexcept {
		m_max.unchecked_assignment(other.m_max);
		return *this;
	}
	auto && operator=(dynamic_max_policy && other) noexcept {
		m_max.unchecked_assignment(std::move(other.m_max));
		return *this;
	}

	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) noexcept {
		return *this = dynamic_max_policy(std::forward<T>(other));
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) volatile noexcept {
		return *this = dynamic_max_policy(std::forward<T>(other));
	}


	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy const &>(*this).assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(std::forward<Maximum>(maximum), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(std::forward<Maximum>(maximum), m_max)
		);
	}
	
	static constexpr auto min() noexcept {
		return make<static_minimum>();
	}

	constexpr auto && max() const & noexcept {
		return m_max;
	}
	constexpr auto && max() const volatile & noexcept {
		return m_max;
	}
	auto && max() & noexcept {
		return m_max;
	}
	auto && max() volatile & noexcept {
		return m_max;
	}
	auto && max() && noexcept {
		return std::move(m_max);
	}
	
	static constexpr bool is_modulo = overflow_policy::is_modulo;
	static constexpr bool overflow_is_error = overflow_policy::overflow_is_error;
private:
	underlying_type m_max;
};



template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
class dynamic_min_policy : private overflow_policy {
public:
	using overflow_policy_tag = void;
	using underlying_type = integer<static_minimum, static_maximum, overflow_policy, storage>;

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_min_policy(underlying_type minimum, OverflowPolicy && policy) noexcept:
		overflow_policy(std::forward<OverflowPolicy>(policy)),
		m_min(std::move(minimum)) {
	}
	constexpr dynamic_min_policy(underlying_type minimum) noexcept:
		m_min(std::move(minimum)) {
	}
	constexpr dynamic_min_policy() noexcept:
		dynamic_min_policy(make<static_minimum>()) {
	}
	constexpr dynamic_min_policy(dynamic_min_policy const &) noexcept = default;
	constexpr dynamic_min_policy(dynamic_min_policy &&) noexcept = default;
	constexpr dynamic_min_policy(dynamic_min_policy const volatile & other) noexcept:
		dynamic_min_policy(other.m_min) {
	}
	constexpr dynamic_min_policy(dynamic_min_policy volatile && other) noexcept:
		dynamic_min_policy(std::move(other.m_min)) {
	}

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_min_policy(OverflowPolicy && policy) noexcept:
		dynamic_min_policy(make<static_minimum>(), std::forward<OverflowPolicy>(policy)) {
	}

	auto && operator=(dynamic_min_policy const & other) noexcept {
		m_min.unchecked_assignment(other.m_min);
		return *this;
	}
	auto && operator=(dynamic_min_policy && other) noexcept {
		m_min.unchecked_assignment(std::move(other.m_min));
		return *this;
	}

	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) noexcept {
		return *this = dynamic_min_policy(std::forward<T>(other));
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto && operator=(T && other) volatile noexcept {
		return *this = dynamic_min_policy(std::forward<T>(other));
	}


	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			std::forward<Maximum>(maximum)
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), m_min),
			std::forward<Maximum>(maximum)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), std::declval<underlying_type &>()),
			std::forward<Maximum>(maximum)
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(std::forward<Minimum>(minimum), m_min),
			std::forward<Maximum>(maximum)
		);
	}
	
	constexpr auto && min() const & noexcept {
		return m_min;
	}
	constexpr auto && min() const volatile & noexcept {
		return m_min;
	}
	auto && min() & noexcept {
		return m_min;
	}
	auto && min() volatile & noexcept {
		return m_min;
	}
	auto && min() && noexcept {
		return std::move(m_min);
	}

	static constexpr auto max() noexcept {
		return make<static_maximum>();
	}
	
	static constexpr bool is_modulo = overflow_policy::is_modulo;
	static constexpr bool overflow_is_error = overflow_policy::overflow_is_error;
private:
	underlying_type m_min;
};

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_DYNAMIC_POLICY_HPP_
