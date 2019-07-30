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
#include <containers/insert_emplace_impl.hpp>
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

	constexpr dynamic_resizable_array() noexcept {}
	
	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit dynamic_resizable_array(Range && range) {
		assign(*this, begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)));
	}
	
	constexpr dynamic_resizable_array(std::initializer_list<value_type> init) {
		assign(*this, begin(init), end(init));
	}

	constexpr dynamic_resizable_array(dynamic_resizable_array const & other) {
		assign(*this, begin(other), end(other));
	}

	constexpr dynamic_resizable_array(dynamic_resizable_array && other) noexcept {
		this->move_assign_to_empty(std::move(other));
	}

	constexpr auto & operator=(dynamic_resizable_array const & other) & noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(dynamic_resizable_array && other) & noexcept {
		detail::destroy_range(*this);
		this->move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, begin(init), end(init));
		return *this;
	}

	~dynamic_resizable_array() {
		detail::destroy_range(*this);
	}


	friend constexpr auto begin(dynamic_resizable_array const & container) noexcept {
		static_assert(noexcept(begin(static_cast<Container const &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<Container const &>(container));
	}
	friend constexpr auto begin(dynamic_resizable_array & container) noexcept {
		static_assert(noexcept(begin(static_cast<Container &>(container))), "This function assumes begin is noexcept");
		return begin(static_cast<Container &>(container));
	}
	friend constexpr auto end(dynamic_resizable_array const & container) noexcept {
		static_assert(noexcept(end(static_cast<Container const &>(container))), "This function assumes end is noexcept");
		return end(static_cast<Container const &>(container));
	}
	friend constexpr auto end(dynamic_resizable_array & container) noexcept {
		static_assert(noexcept(end(static_cast<Container &>(container))), "This function assumes end is noexcept");
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
	template<typename Integer>
	constexpr void append_from_capacity(Integer const count) noexcept {
		this->set_size(size(*this) + count);
	}

	template<typename... Args>
	auto emplace(const_iterator const position, Args && ... args) {
		auto relocating_emplace = [&]{
			insert_or_emplace_with_reallocation(position, 1_bi, [&](auto const ptr) {
				bounded::construct(*ptr, BOUNDED_FORWARD(args)...);
			});
		};
		return ::containers::detail::emplace_impl(*this, position, relocating_emplace, BOUNDED_FORWARD(args)...);
	}

	// TODO: Check if the range lies within the container
	template<typename Range = std::initializer_list<value_type>>
	auto insert(const_iterator const position, Range && range) {
		// This looks like a double move, but when insert_impl actually moves
		// from range, it does not call this function
		return detail::insert_impl(*this, position, BOUNDED_FORWARD(range), [&](auto const range_size) {
			return insert_or_emplace_with_reallocation(position, range_size, [&](auto const ptr) {
				::containers::uninitialized_copy(BOUNDED_FORWARD(range), ptr);
			});
		});
	}

private:
	template<typename Integer, typename Function>
	auto insert_or_emplace_with_reallocation(const_iterator position, Integer number_of_elements, Function construct) {
		// There is a reallocation required, so just put everything in the
		// correct place to begin with
		auto temp = this->make_storage(::containers::detail::reallocation_size(*this, number_of_elements));
		// First construct the new element because the arguments to
		// construct it may reference an old element. We cannot move
		// elements it references before constructing it
		auto const offset = position - begin(*this);
		construct(detail::static_or_reinterpret_cast<value_type *>(data(temp)) + offset);
		auto const mutable_position = begin(*this) + offset;
		auto const temp_begin = detail::static_or_reinterpret_cast<value_type *>(data(temp));
		auto const pointer = containers::uninitialized_move_destroy(begin(*this), mutable_position, temp_begin);
		BOUNDED_ASSERT(temp_begin + offset == pointer);
		::containers::uninitialized_move_destroy(mutable_position, end(*this), pointer + number_of_elements);
		this->relocate_preallocated(std::move(temp));
		append_from_capacity(number_of_elements);
		return mutable_position;
	}
};

template<typename Range>
dynamic_resizable_array(Range &&) -> dynamic_resizable_array<std::decay_t<typename std::decay_t<Range>::value_type>>;


}	// namespace detail

template<typename Container>
inline constexpr auto is_container<detail::dynamic_resizable_array<Container>> = true;

}	// namespace containers
