// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#undef NDEBUG

#include <bounded/detail/comparison.hpp>

#include <cassert>

namespace bounded_test {

BOUNDED_COMPARISON

// Add in some asserts and manually make special member functions non-trivial
struct string_view {
	constexpr string_view(char const * value_) noexcept:
		value(value_) {
	}
	constexpr string_view(string_view const & other) noexcept:
		value(other.value)
	{
		assert(not other.moved_from);
	}
	constexpr auto & operator=(string_view const & other) noexcept {
		assert(not other.moved_from);
		value = other.value;
		moved_from = false;
		return *this;
	}
	constexpr string_view(string_view && other) noexcept:
		value(other.value)
	{
		assert(not other.moved_from);
		other.moved_from = true;
	}
	constexpr auto & operator=(string_view && other) noexcept {
		assert(not other.moved_from);
		value = other.value;
		moved_from = false;
		other.moved_from = true;
		return *this;
	}
	
	friend constexpr auto compare(string_view lhs, string_view rhs) noexcept {
		assert(not lhs.moved_from and not rhs.moved_from);
		while (true) {
			if (auto const cmp = bounded::compare(*lhs.value, *rhs.value); cmp != 0 or *lhs.value == '\0') {
				return cmp;
			}
			++lhs.value;
			++rhs.value;
		}
	}
private:
	char const * value;
	bool moved_from = false;
};

}	// namespace bounded_test