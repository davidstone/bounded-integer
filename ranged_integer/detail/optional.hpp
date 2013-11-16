// Specialization for boost::optional
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

#ifndef RANGED_INTEGER_DETAIL_OPTIONAL_HPP_
#define RANGED_INTEGER_DETAIL_OPTIONAL_HPP_

#include "class.hpp"
#include "enable_if.hpp"

#include <cstdint>
#include <limits>

#include <boost/assert.hpp>
#include <boost/optional.hpp>

enum class none_t {};
constexpr none_t none = none_t{};

namespace detail {

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class policy>
class compressed_optional {
public:
	using value_type = ranged_integer<minimum, maximum, policy>;

	constexpr compressed_optional() noexcept:
		m_value(uninitialized_value(), non_check) {
	}
	constexpr compressed_optional(boost::none_t) noexcept:
		compressed_optional{} {
	}

	template<typename integer, enable_if_t<
		detail::is_implicitly_constructible_from<minimum, maximum, integer>()
	> = clang_dummy>
	constexpr compressed_optional(integer && other) noexcept:
		m_value(std::forward<integer>(other)) {
	}
	template<typename integer, enable_if_t<
		detail::is_explicitly_constructible_from<minimum, maximum, policy<minimum, maximum>, integer>()
	> = clang_dummy>
	constexpr explicit compressed_optional(integer && other):
		m_value(std::forward<integer>(other)) {
	}

	constexpr compressed_optional(bool condition, value_type const & value) noexcept:
		m_value(condition ? value : value_type(uninitialized_value())) {
	}
	constexpr compressed_optional(compressed_optional const &) noexcept = default;
	template<intmax_t other_min, intmax_t other_max, template<intmax_t, intmax_t> class other_policy>
	constexpr explicit compressed_optional(compressed_optional<other_min, other_max, other_policy> const & other):
		m_value(other.is_initialized() ? *other : uninitialized_value()) {
	}
	template<typename U>
	constexpr explicit compressed_optional(boost::optional<U> const & other):
		m_value(other.is_initialized() ? *other : uninitialized_value()) {
	}

	// TODO: in_place_factory overloads?
	
	compressed_optional & operator=(compressed_optional const &) noexcept = default;
	compressed_optional & operator=(boost::none_t) noexcept {
		*this = compressed_optional(boost::none);
		return *this;
	}
	compressed_optional & operator=(value_type const & value) noexcept {
		m_value = value;
		return *this;
	}
	template<intmax_t other_min, intmax_t other_max, template<intmax_t, intmax_t> class other_policy>
	compressed_optional & operator=(compressed_optional<other_min, other_max, other_policy> const & other) {
		*this = compressed_optional(other);
		return *this;
	}
	template<typename U>
	compressed_optional & operator=(boost::optional<U> const & other) {
		*this = compressed_optional(other);
		return *this;
	}

	// TODO: in_place_factory overloads?
	
	value_type const & get() const {
		BOOST_ASSERT(is_initialized());
		return m_value;
	}
	value_type & get() noexcept {
		BOOST_ASSERT(is_initialized());
		return m_value;
	}
	value_type const & operator*() const {
		BOOST_ASSERT(is_initialized());
		return get();
	}
	value_type & operator*() {
		BOOST_ASSERT(is_initialized());
		return get();
	}
	
	constexpr value_type const * get_ptr() const noexcept {
		return is_initialized() ? &m_value : nullptr;
	}
	value_type * get_ptr() noexcept {
		return is_initialized() ? &m_value : nullptr;
	}
	value_type const * operator->() const {
		BOOST_ASSERT(is_initialized());
		return get_ptr();
	}
	value_type * operator->() {
		BOOST_ASSERT(is_initialized());
		return get_ptr();
	}
	
	constexpr value_type const & get_value_or(value_type const & default_value) const noexcept {
		return is_initialized() ? m_value : default_value;
	}
	
	constexpr explicit operator bool() const noexcept {
		return is_initialized();
	}
	constexpr bool operator!() const noexcept {
		return !is_initialized();
	}
private:
	constexpr bool is_initialized() const noexcept {
		return m_value != uninitialized_value();
	}
	using underlying_type = typename value_type::underlying_type;
	static constexpr underlying_type uninitialized_value() {
		return static_cast<underlying_type>(
			(minimum > std::numeric_limits<underlying_type>::min()) ? minimum - 1 :
			(maximum < std::numeric_limits<underlying_type>::max()) ? maximum + 1 :
			throw std::logic_error("Attempted to use compressed_optional when the default version should have been used.")
		);
	}
	value_type m_value;
};

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator==(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs == *rhs :
		(static_cast<bool>(lhs) == static_cast<bool>(rhs));
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator!=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs < *rhs :
		(static_cast<bool>(lhs) < static_cast<bool>(rhs));
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}



template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator==(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return static_cast<bool>(lhs) and *lhs == rhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator==(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs == lhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator!=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator!=(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs == lhs);
}


template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !static_cast<bool>(lhs) ? true : *lhs == rhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !static_cast<bool>(rhs) ? false : lhs == *rhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator<=(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}


template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>=(compressed_optional<lhs_min, lhs_max, lhs_policy> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
bool operator>=(ranged_integer<lhs_min, lhs_max, lhs_policy> const & lhs, compressed_optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}




template<intmax_t minimum, intmax_t maximum>
class has_extra_space {
private:
	static constexpr bool uses_space = minimum != maximum;
	using underlying_type = underlying_t<minimum, maximum>;
	static constexpr intmax_t underlying_min = std::numeric_limits<underlying_type>::min();
	static constexpr intmax_t underlying_max = std::numeric_limits<underlying_type>::max();
	static constexpr bool smaller_than_underlying_type = underlying_min < minimum or underlying_max > maximum;
public:
	static constexpr bool value = uses_space and smaller_than_underlying_type;
};

template<
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class policy,
	bool use_compressed_version
>
class optional_c;
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class policy>
class optional_c<minimum, maximum, policy, true> {
public:
	using type = compressed_optional<minimum, maximum, policy>;
};
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class policy>
class optional_c<minimum, maximum, policy, false> {
public:
	using type = boost::optional<ranged_integer<minimum, maximum, policy>>;
};

template<typename T>
class optional_general {
public:
	using type = boost::optional<T>;
};
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class policy>
class optional_general<ranged_integer<minimum, maximum, policy>> {
public:
	using type = typename optional_c<minimum, maximum, policy, has_extra_space<minimum, maximum>::value>::type;
};

}	// namespace detail

template<typename T>
using optional = typename detail::optional_general<T>::type;

template<typename T>
optional<typename std::remove_reference<T>::type> make_optional(T && value) noexcept {
	return optional<T>(std::forward<T>(value));
}

#endif	// RANGED_INTEGER_DETAIL_OPTIONAL_HPP_
