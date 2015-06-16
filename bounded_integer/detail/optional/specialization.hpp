// optional specialization for bounded::integer
// Copyright (C) 2015 David Stone
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

#include "optional.hpp"

#include "../class.hpp"
#include "../enable_if.hpp"
#include "../numeric_limits.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace bounded {
namespace detail {

template<typename T>
constexpr auto underlying_min = basic_numeric_limits<typename T::underlying_type>::min();
template<typename T>
constexpr auto underlying_max = basic_numeric_limits<typename T::underlying_type>::max();

template<typename T>
constexpr auto has_extra_space = underlying_min<T> < basic_numeric_limits<T>::min() or basic_numeric_limits<T>::max() < underlying_max<T>;

template<typename T>
struct integer_optional_storage_c {
	constexpr integer_optional_storage_c() noexcept:
		m_value(uninitialized_value(), non_check) {
	}
	template<typename... Args>
	constexpr explicit integer_optional_storage_c(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_value(std::forward<Args>(args)...) {
	}

	// TODO: handle stateful policies (not set in assignment operator)
	template<typename... Args>
	constexpr void emplace(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value) {
		m_value = T(std::forward<Args>(args)...);
	}
	
	constexpr void reset() noexcept {
		m_value.unchecked_assignment(uninitialized_value());
	}

	
	constexpr auto is_initialized() const noexcept {
		return m_value.value() != uninitialized_value();
	}
	constexpr auto && value() const & noexcept {
		return m_value;
	}
	constexpr auto && value() & noexcept {
		return m_value;
	}
	constexpr auto && value() && noexcept {
		return std::move(m_value);
	}
private:
	static constexpr auto uninitialized_value() noexcept {
		constexpr auto minimum = basic_numeric_limits<T>::min();
		constexpr auto maximum = basic_numeric_limits<T>::max();
		return static_cast<typename T::underlying_type>(minimum > underlying_min<T> ? minimum - 1 : maximum + 1);
	}
	T m_value;
};

template<typename T>
using integer_optional_storage = std::conditional_t<has_extra_space<T>, integer_optional_storage_c<T>, default_optional_storage<T>>; 

}	// namespace detail

template<intmax_t minimum, intmax_t maximum, typename overflow, storage_type storage>
struct optional_storage<integer<minimum, maximum, overflow, storage>> : detail::integer_optional_storage<integer<minimum, maximum, overflow, storage>> {
private:
	using base = detail::integer_optional_storage<integer<minimum, maximum, overflow, storage>>;
public:
	using base::base;
};

}	// namespace bounded
