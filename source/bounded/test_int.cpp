// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include "../test_assert.hpp"

export module bounded.test_int;

import std_module;

namespace bounded_test {

enum class special { copyable, non_copyable };

// Every constructor allocates memory. During constant evaluation, this forces
// the compiler to ensure the destructor is run exactly once for every instance
// of the object. Sanitizers should ensure the same during run time.
template<special member>
struct integer_impl {
	constexpr integer_impl(int x_):
		m_value(new int(x_))
	{
	}
	constexpr integer_impl():
		integer_impl(0)
	{
	}

	constexpr integer_impl(integer_impl && other):
		m_value(new int(other.value()))
	{
		// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
		#if !defined __GNUC__ or defined __clang__
			BOUNDED_TEST(this != &other);
		#endif
		other.m_moved_from = true;
	}
	constexpr integer_impl(integer_impl const & other) requires(member == special::copyable):
		m_value(new int(other.value()))
	{
		// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
		#if !defined __GNUC__ or defined __clang__
			BOUNDED_TEST(this != &other);
		#endif
	}

	constexpr integer_impl & operator=(integer_impl && other) {
		*m_value = other.value();
		m_moved_from = false;
		other.m_moved_from = true;
		return *this;
	}
	constexpr integer_impl & operator=(integer_impl const & other) requires(member == special::copyable) {
		*m_value = other.value();
		m_moved_from = false;
		return *this;
	}

	constexpr ~integer_impl() {
		delete m_value;
	}

	constexpr int const & value() const {
		BOUNDED_TEST(!m_moved_from);
		return *m_value;
	}

	friend constexpr auto operator<=>(integer_impl const & lhs, integer_impl const & rhs) {
		return lhs.value() <=> rhs.value();
	}
	friend constexpr auto operator==(integer_impl const & lhs, integer_impl const & rhs) -> bool {
		return lhs.value() == rhs.value();
	}
	friend constexpr auto relocate(integer_impl & x) noexcept {
		BOUNDED_TEST(!x.m_moved_from);
		return integer_impl(x, tag());
	}
	friend constexpr auto to_radix_sort_key(integer_impl const & x) {
		return x.value();
	}
private:
	struct tag{};
	constexpr integer_impl(integer_impl & other, tag) noexcept:
		m_value(other.m_value)
	{
		other.m_moved_from = true;
	}

	int * m_value;
	bool m_moved_from = false;
};

export using integer = integer_impl<special::copyable>;
export using non_copyable_integer = integer_impl<special::non_copyable>;

export constexpr auto copy_value(non_copyable_integer const & x) -> non_copyable_integer {
	return non_copyable_integer(x.value());
}

} // namespace bounded_test