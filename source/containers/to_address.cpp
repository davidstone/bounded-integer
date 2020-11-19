// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/to_address.hpp>

namespace {

static_assert(containers::to_address(nullptr) == nullptr);
static_assert(containers::to_address(static_cast<void *>(nullptr)) == nullptr);

struct my_iterator {
	constexpr explicit my_iterator(void * pointer):
		m_pointer(pointer)
	{
	}

	constexpr auto to_address() const {
		return m_pointer;
	}
private:
	void * m_pointer;
};

static_assert(containers::to_address(my_iterator(nullptr)) == nullptr);

} // namespace