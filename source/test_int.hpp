// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

// This allocates memory to force the compiler to ensure the destructor is run
// exactly once for every instance of the object
struct test_int {
	constexpr test_int(int x_):
		m_value(new int(x_))
	{
	}
	constexpr test_int(test_int const & other):
		m_value(new int(*other.m_value))
	{
	}
	constexpr test_int & operator=(test_int const & other) {
		*m_value = *other.m_value;
		return *this;
	}
	constexpr ~test_int() {
		delete m_value;
	}

	friend constexpr auto operator==(test_int const & lhs, test_int const & rhs) -> bool {
		return *lhs.m_value == *rhs.m_value;
	}
	friend constexpr auto move_destroy(test_int && value) noexcept {
		return test_int(std::move(value), tag());
	}

	constexpr auto value() const {
		return *m_value;
	}

private:
	struct tag{};
	constexpr test_int(test_int && other, tag) noexcept:
		m_value(other.m_value)
	{
	}

	int * m_value;
};

} // namespace bounded