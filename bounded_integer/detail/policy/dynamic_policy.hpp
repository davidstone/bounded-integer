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

#pragma once

#include "is_overflow_policy.hpp"

#include "../class.hpp"
#include "../comparison.hpp"
#include "../enable_if.hpp"
#include "../make.hpp"
#include "../minmax.hpp"
#include "../arithmetic/operators.hpp"

#include <cstdint>

namespace bounded {

template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
struct dynamic_policy;

template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
struct dynamic_max_policy;

template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage = storage_type::fast>
struct dynamic_min_policy;

namespace detail {

// Copying an overflow policy should copy its dynamic min / max, if the source
// overflow policy has one. If we do not have a dynamic min / max, then the
// default values should be the same as the static min / max.

template<typename T>
struct has_dynamic_max : std::false_type {};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct has_dynamic_max<dynamic_policy<minimum, maximum, overflow_policy, storage>> : std::true_type {};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct has_dynamic_max<dynamic_max_policy<minimum, maximum, overflow_policy, storage>> : std::true_type {};


template<typename T>
struct has_dynamic_min : std::false_type {};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct has_dynamic_min<dynamic_policy<minimum, maximum, overflow_policy, storage>> : std::true_type {};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct has_dynamic_min<dynamic_min_policy<minimum, maximum, overflow_policy, storage>> : std::true_type {};


template<
	intmax_t minimum,
	typename OverflowPolicy,
	enable_if_t<!has_dynamic_min<std::decay_t<OverflowPolicy>>::value> = clang_dummy
>
constexpr auto get_minimum(OverflowPolicy const &) noexcept {
	static_assert(
		is_overflow_policy<OverflowPolicy>::value,
		"Can only be used with an overflow_policy."
	);
	return make<minimum>();
}

template<
	intmax_t,
	typename OverflowPolicy,
	enable_if_t<has_dynamic_min<std::decay_t<OverflowPolicy>>::value> = clang_dummy
>
constexpr decltype(auto) get_minimum(OverflowPolicy const & policy) noexcept {
	return policy.min();
}


template<
	intmax_t maximum,
	typename OverflowPolicy,
	enable_if_t<!has_dynamic_max<std::decay_t<OverflowPolicy>>::value> = clang_dummy
>
constexpr auto get_maximum(OverflowPolicy const &) noexcept {
	static_assert(
		is_overflow_policy<OverflowPolicy>::value,
		"Can only be used with an overflow_policy."
	);
	return make<maximum>();
}

template<
	intmax_t,
	typename OverflowPolicy,
	enable_if_t<has_dynamic_max<std::decay_t<OverflowPolicy>>::value> = clang_dummy
>
constexpr decltype(auto) get_maximum(OverflowPolicy const & policy) noexcept {
	return policy.max();
}

}	// namespace detail


template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage>
struct dynamic_policy : private overflow_policy {
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

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_policy(OverflowPolicy && policy) noexcept:
		dynamic_policy(
			detail::get_minimum<static_minimum>(std::forward<OverflowPolicy>(policy)),
			detail::get_maximum<static_maximum>(std::forward<OverflowPolicy>(policy)),
			std::forward<OverflowPolicy>(policy)
		) {
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
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), std::declval<underlying_type &>()),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), m_min),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), m_max)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), std::declval<underlying_type &>()),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), m_min),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), m_max)
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



template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage>
struct dynamic_max_policy : private overflow_policy {
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

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_max_policy(OverflowPolicy && policy) noexcept:
		dynamic_max_policy(
			detail::get_maximum<static_maximum>(std::forward<OverflowPolicy>(policy)),
			std::forward<OverflowPolicy>(policy)
		) {
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
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy const &>(*this).assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), m_max)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), std::declval<underlying_type &>())
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			std::forward<Minimum>(minimum),
			::bounded::min(bounded::make(std::forward<Maximum>(maximum)), m_max)
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



template<intmax_t static_minimum, intmax_t static_maximum, typename overflow_policy, storage_type storage>
struct dynamic_min_policy : private overflow_policy {
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

	template<typename OverflowPolicy, enable_if_t<is_overflow_policy<OverflowPolicy>::value> = clang_dummy>
	constexpr dynamic_min_policy(OverflowPolicy && policy) noexcept:
		dynamic_min_policy(
			detail::get_minimum<static_minimum>(std::forward<OverflowPolicy>(policy)),
			std::forward<OverflowPolicy>(policy)
		) {
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
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), std::declval<underlying_type &>()),
			std::forward<Maximum>(maximum)
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), m_min),
			std::forward<Maximum>(maximum)
		);
	}
	template<typename T, typename Minimum, typename Maximum>
	constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) const volatile
		noexcept(noexcept(std::declval<overflow_policy>().assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), std::declval<underlying_type &>()),
			std::forward<Maximum>(maximum)
		))) {
		return static_cast<overflow_policy volatile const &>(*this).assignment(
			std::forward<T>(value),
			::bounded::max(bounded::make(std::forward<Minimum>(minimum)), m_min),
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
