// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "test_assert.hpp"

#include <compare>

namespace bounded_test {

// Every constructor allocates memory. During constant evaluation, this forces
// the compiler to ensure the destructor is run exactly once for every instance
// of the object. Sanitizers should ensure the same during run time.
struct integer {
	constexpr integer(int x_):
		m_value(new int(x_))
	{
	}
	constexpr integer():
		integer(0)
	{
	}

	constexpr integer(integer && other):
		m_value(new int(other.value()))
	{
		// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
		#if !defined __GNUC__ or defined __clang__
			BOUNDED_TEST(this != &other);
		#endif
		other.m_moved_from = true;
	}
	constexpr integer(integer const & other):
		m_value(new int(other.value()))
	{
		// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
		#if !defined __GNUC__ or defined __clang__
			BOUNDED_TEST(this != &other);
		#endif
	}

	constexpr integer & operator=(integer && other) {
		*m_value = other.value();
		m_moved_from = false;
		other.m_moved_from = true;
		return *this;
	}
	constexpr integer & operator=(integer const & other) {
		*m_value = other.value();
		m_moved_from = false;
		return *this;
	}

	constexpr ~integer() {
		delete m_value;
	}

	constexpr int const & value() const {
		BOUNDED_TEST(!m_moved_from);
		return *m_value;
	}

	friend constexpr auto operator<=>(integer const & lhs, integer const & rhs) {
		return lhs.value() <=> rhs.value();
	}
	friend constexpr auto operator==(integer const & lhs, integer const & rhs) -> bool {
		return lhs.value() == rhs.value();
	}
	friend constexpr auto relocate(integer & x) noexcept {
		BOUNDED_TEST(!x.m_moved_from);
		return integer(x, tag());
	}
	friend constexpr auto to_radix_sort_key(integer const & x) {
		return x.value();
	}
private:
	struct tag{};
	constexpr integer(integer & other, tag) noexcept:
		m_value(other.m_value)
	{
	}

	int * m_value;
	bool m_moved_from = false;
};

} // namespace bounded