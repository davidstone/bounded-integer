// Does nothing. Overflow is undefined behavior
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

#ifndef BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_

#include <cstdint>

namespace bounded_integer {

class null_policy {
public:
	constexpr null_policy() noexcept {}
	constexpr null_policy(null_policy const &) noexcept {}
	constexpr null_policy(null_policy &&) noexcept {}
	constexpr null_policy(null_policy const volatile &) noexcept {}
	constexpr null_policy(null_policy volatile &&) noexcept {}
	null_policy & operator=(null_policy const &) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy &&) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy const volatile &) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy volatile &&) noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy const &) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy &&) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy const volatile &) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy volatile &&) volatile noexcept {
		return *this;
	}
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// http://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const value) const noexcept {
		return (minimum < value and value < maximum) ? value : error_out_of_range(value);
	}
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const value) const volatile noexcept {
		return (minimum < value and value < maximum) ? value : error_out_of_range(value);
	}

	// It might actually be true! This should be considered undefined
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
private:
	template<typename T>
	static T error_out_of_range(T const value) noexcept {
		return value;
	}
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
