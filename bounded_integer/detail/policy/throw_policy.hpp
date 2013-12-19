// Throws an exception on overflow
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

#ifndef BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_

#include "../string.hpp"
#include <cstdint>
#include <stdexcept>

namespace bounded_integer {

class throw_policy {
public:
	constexpr throw_policy() noexcept {}
	constexpr throw_policy(throw_policy const &) noexcept {}
	constexpr throw_policy(throw_policy &&) noexcept {}
	constexpr throw_policy(throw_policy const volatile &) noexcept {}
	constexpr throw_policy(throw_policy volatile &&) noexcept {}
	throw_policy & operator=(throw_policy const &) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy &&) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy const volatile &) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy volatile &&) noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy const &) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy &&) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy const volatile &) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy volatile &&) volatile noexcept {
		return *this;
	}
	// The optimizer should be able to simplify this to remove dead checks.
	template<intmax_t minimum, intmax_t maximum, typename T>
	constexpr intmax_t assignment(T const value) const {
		return (minimum <= value and value <= maximum) ?
			static_cast<intmax_t>(value) :
			throw std::range_error("Got a value of " + to_string(+value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
	template<intmax_t minimum, intmax_t maximum, typename T>
	constexpr intmax_t assignment(T const value) const volatile {
		return (minimum <= value and value <= maximum) ?
			static_cast<intmax_t>(value) :
			throw std::range_error("Got a value of " + to_string(+value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
	
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
