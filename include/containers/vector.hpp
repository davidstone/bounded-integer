// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array.hpp>
#include <containers/dynamic_resizable_array.hpp>
#include <containers/index_type.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename T>
struct vector_base {
	using value_type = T;
	using size_type = typename dynamic_array_data<value_type>::size_type;

	using const_iterator = contiguous_iterator<value_type const, bounded::detail::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::detail::builtin_max_value<size_type>>;
	using raw_container = dynamic_array<trivial_storage<value_type>>;

	constexpr vector_base() = default;

	constexpr vector_base(vector_base && other) noexcept:
		m_container(std::move(other.m_container)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}

	constexpr auto move_assign_to_empty(vector_base && other) & {
		m_container = std::move(other.m_container);
		m_size = std::exchange(other.m_size, 0_bi);
	}

	friend constexpr auto begin(vector_base const & container) {
		return const_iterator(::containers::detail::static_or_reinterpret_cast<value_type const *>(data(container.m_container)));
	}
	friend constexpr auto begin(vector_base & container) {
		return iterator(::containers::detail::static_or_reinterpret_cast<value_type *>(data(container.m_container)));
	}
	friend constexpr auto end(vector_base const & container) {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(vector_base & container) {
		return begin(container) + container.m_size;
	}

	constexpr auto capacity() const {
		return ::containers::size(m_container);
	}

	template<typename Size>
	auto make_storage(Size const new_capacity) {
		return raw_container(repeat_default_n<typename raw_container::value_type>(new_capacity));
	}
	auto relocate_preallocated(raw_container temp) {
		m_container = std::move(temp);
		// m_size remains the same
	}

	template<typename Size>
	auto relocate(Size const requested_capacity) {
		auto temp = make_storage(requested_capacity);
		containers::uninitialized_move_destroy(
			*this,
			::containers::detail::static_or_reinterpret_cast<value_type *>(data(temp))
		);
		relocate_preallocated(std::move(temp));
	}
	
	template<typename Size>
	auto set_size(Size const s) {
		m_size = s;
	}
private:
	raw_container m_container;
	size_type m_size = 0_bi;
};

}	// namespace detail

template<typename T>
inline constexpr auto is_container<detail::vector_base<T>> = true;

template<typename T>
using vector = detail::dynamic_resizable_array<detail::vector_base<T>>;

}	// namespace containers
