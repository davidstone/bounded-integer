// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.to_address;

import std_module;

namespace containers {

template<typename T>
concept pointer_like = std::is_pointer_v<T> or std::is_null_pointer_v<T>;

export template<typename T>
concept to_addressable = pointer_like<T> or requires(T const x) { x.to_address(); };

export template<to_addressable Iterator>
constexpr auto to_address(Iterator const it) {
	if constexpr (pointer_like<Iterator>) {
		return it;
	} else {
		return it.to_address();
	}
}

} // namespace containers

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