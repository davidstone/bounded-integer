// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array.hpp>
#include <containers/emplace_back.hpp>
#include <containers/index_type.hpp>
#include <containers/is_container.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/assert.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename Container>
struct dynamic_resizable_array : private Container {
	using typename Container::value_type;
	using typename Container::size_type;
	using typename Container::const_iterator;
	using typename Container::iterator;

	constexpr dynamic_resizable_array() {}
	
	template<typename Range> requires(
		range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit dynamic_resizable_array(Range && source) {
		append(*this, BOUNDED_FORWARD(source));
	}
	
	constexpr dynamic_resizable_array(std::initializer_list<value_type> init) {
		append(*this, init);
	}

	constexpr dynamic_resizable_array(dynamic_resizable_array const & other) {
		append(*this, other);
	}

	constexpr dynamic_resizable_array(dynamic_resizable_array && other) noexcept {
		this->move_assign_to_empty(std::move(other));
	}

	constexpr auto & operator=(dynamic_resizable_array const & other) & {
		assign(*this, other);
		return *this;
	}
	constexpr auto & operator=(dynamic_resizable_array && other) & noexcept {
		::containers::detail::destroy_range(*this);
		this->move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
	}

	~dynamic_resizable_array() {
		::containers::detail::destroy_range(*this);
	}


	friend constexpr auto begin(dynamic_resizable_array const & container) {
		return begin(static_cast<Container const &>(container));
	}
	friend constexpr auto begin(dynamic_resizable_array & container) {
		return begin(static_cast<Container &>(container));
	}
	friend constexpr auto end(dynamic_resizable_array const & container) {
		return end(static_cast<Container const &>(container));
	}
	friend constexpr auto end(dynamic_resizable_array & container) {
		return end(static_cast<Container &>(container));
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(dynamic_resizable_array)
	
	using Container::capacity;
	auto reserve(size_type const requested_capacity) {
		if (requested_capacity > capacity()) {
			this->relocate(requested_capacity);
		}
	}
	auto shrink_to_fit() {
		if (size(*this) != capacity()) {
			this->relocate(size(*this));
		}
	}

	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		this->set_size(size(*this) + count);
	}
};

template<typename Range>
dynamic_resizable_array(Range &&) -> dynamic_resizable_array<std::decay_t<typename std::decay_t<Range>::value_type>>;


}	// namespace detail

template<typename Container>
inline constexpr auto is_container<detail::dynamic_resizable_array<Container>> = true;

}	// namespace containers
