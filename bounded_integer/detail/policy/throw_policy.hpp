// Throws an exception on overflow
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

#ifndef BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_

#include "is_overflow_policy.hpp"
#include "../enable_if.hpp"
#include "../string.hpp"
#include <cstdint>
#include <stdexcept>

namespace bounded_integer {

class throw_policy {
public:
	using overflow_policy_tag = void;
	constexpr throw_policy() noexcept {}
	constexpr throw_policy(throw_policy const &) noexcept = default;
	constexpr throw_policy(throw_policy &&) noexcept = default;
	template<typename T, enable_if_t<std::is_same<typename std::decay<T>::type, throw_policy>::value> = clang_dummy>
	constexpr throw_policy(T &&) noexcept {
	}
	template<typename T, enable_if_t<
		is_overflow_policy<T>::value and
		!std::is_same<typename std::decay<T>::type, throw_policy>::value
	> = clang_dummy>
	constexpr explicit throw_policy(T &&) noexcept {
	}

	throw_policy & operator=(throw_policy const &) noexcept = default;
	throw_policy & operator=(throw_policy &&) noexcept = default;
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	throw_policy & operator=(T &&) noexcept {
		return *this;
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	throw_policy volatile & operator=(T &&) volatile noexcept {
		return *this;
	}

	// The optimizer should be able to simplify this to remove dead checks.
	template<typename T, typename Minimum, typename Maximum>
	static constexpr intmax_t assignment(T && value, Minimum && minimum, Maximum && maximum) {
		return (minimum <= value and value <= maximum) ?
			static_cast<intmax_t>(value) :
			throw std::range_error("Got a value of " + to_string(+value) + " but expected a value in the range [" + to_string(+minimum) + ", " + to_string(+maximum) + "]");
	}
	
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
