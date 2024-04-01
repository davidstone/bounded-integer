// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.to_address;

import std_module;

namespace containers {

template<typename T>
concept pointer_like = std::is_pointer_v<T> or std::is_null_pointer_v<T>;

template<typename T>
concept has_member_to_address = requires(T const x) {
	x.to_address();
};

template<typename T>
concept has_pointer_traits = requires(T const x) { std::pointer_traits<T>::to_address(x); };

template<typename T>
concept has_member_arrow = requires(T const x) { x.operator->(); };

export template<typename T>
concept to_addressable =
	pointer_like<T> or
	has_member_to_address<T> or
	has_pointer_traits<T> or
	has_member_arrow<T>;

export template<to_addressable Iterator>
constexpr auto to_address(Iterator const it) {
	if constexpr (pointer_like<Iterator>) {
		return it;
	} else if constexpr (has_member_to_address<Iterator>) {
		return it.to_address();
	} else if constexpr (has_pointer_traits<Iterator>) {
		return std::pointer_traits<Iterator>::to_address(it);
	} else {
		return it.operator->();
	}
}

} // namespace containers

static_assert(!containers::to_addressable<int>);

static_assert(containers::to_address(nullptr) == nullptr);
static_assert(containers::to_address(static_cast<void *>(nullptr)) == nullptr);
static_assert(containers::to_address(std::string_view().begin()) == nullptr);

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