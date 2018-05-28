// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/dynamic_array/dynamic_array.hpp>
#include <containers/index_type.hpp>
#include <containers/is_container.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/integer_range.hpp>

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
	using typename Container::allocator_type;
	using typename Container::size_type;
	using typename Container::const_iterator;
	using typename Container::iterator;

	using Container::get_allocator;
	
	constexpr dynamic_resizable_array() noexcept {}
	
	constexpr explicit dynamic_resizable_array(allocator_type allocator_) BOUNDED_NOEXCEPT_INITIALIZATION(
		Container(std::move(allocator_))
	) {
	}
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr explicit dynamic_resizable_array(Count const count, allocator_type allocator_ = allocator_type()):
		Container(std::move(allocator_))
	{
		if (count > capacity()) {
			this->relocate(count);
		}
		::containers::uninitialized_default_construct(begin(*this), begin(*this) + count, get_allocator());
		this->set_size(count);
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr dynamic_resizable_array(Count const count, value_type const & value, allocator_type allocator_ = allocator_type()):
		Container(std::move(allocator_))
	{
		auto const repeat = detail::repeat_n(count, value);
		assert(size(repeat) == count);
		assign(*this, begin(repeat), end(repeat));
	}
	
	template<typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
	constexpr dynamic_resizable_array(ForwardIterator first, Sentinel const last, allocator_type allocator_ = allocator_type()):
		Container(std::move(allocator_))
	{
		assign(*this, first, last);
	}
	
	constexpr dynamic_resizable_array(std::initializer_list<value_type> init, allocator_type allocator_ = allocator_type()) BOUNDED_NOEXCEPT_INITIALIZATION(
		dynamic_resizable_array(begin(init), end(init), std::move(allocator_))
	) {}

	constexpr dynamic_resizable_array(dynamic_resizable_array const & other, allocator_type allocator_) BOUNDED_NOEXCEPT_INITIALIZATION(
		dynamic_resizable_array(begin(other), end(other), std::move(allocator_))
	) {}
	constexpr dynamic_resizable_array(dynamic_resizable_array const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		dynamic_resizable_array(begin(other), end(other), other.get_allocator())
	) {}

	constexpr dynamic_resizable_array(dynamic_resizable_array && other, allocator_type allocator_) noexcept:
		Container(std::move(allocator_))
	{
		this->move_assign_to_empty(std::move(other));
	}
	constexpr dynamic_resizable_array(dynamic_resizable_array && other) noexcept:
		dynamic_resizable_array(std::move(other), std::move(other).get_allocator())
	{
	}

	constexpr auto & operator=(dynamic_resizable_array const & other) & noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(dynamic_resizable_array && other) & noexcept {
		::containers::detail::destroy(get_allocator(), begin(*this), end(*this));
		this->move_assign_to_empty(std::move(other));
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, begin(init), end(init));
		return *this;
	}

	~dynamic_resizable_array() {
		::containers::detail::destroy(get_allocator(), begin(*this), end(*this));
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

	template<typename... Args>
	decltype(auto) emplace_back(Args && ... args) {
		if (size(*this) < capacity()) {
			::containers::detail::construct(get_allocator(), data(*this) + size(*this), std::forward<Args>(args)...);
		} else {
			auto temp = this->make_storage(new_capacity());
			::containers::detail::construct(get_allocator(), data(temp) + capacity(), std::forward<Args>(args)...);
			::containers::uninitialized_move_destroy(begin(*this), end(*this), data(temp), get_allocator());
			this->relocate_preallocated(std::move(temp));
		}
		add_size(1_bi);
		return back(*this);
	}
	
	// TODO: Remove duplication between emplace and insert
	template<typename... Args>
	auto emplace(const_iterator const position, Args && ... args) {
		check_iterator_validity(position);
		auto const offset = position - begin(*this);
		if (position == end(*this)) {
			emplace_back(std::forward<Args>(args)...);
		} else if (size(*this) != capacity()) {
			detail::emplace_in_middle_no_reallocation(*this, position, get_allocator(), std::forward<Args>(args)...);
		} else {
			// There is a reallocation required, so just put everything in the
			// correct place to begin with
			auto temp = this->make_storage(new_capacity());
			// First construct the new element because the arguments to
			// construct it may reference an old element. We cannot move
			// elements it references before constructing it
			auto && allocator_ = get_allocator();
			::containers::detail::construct(allocator_, data(temp) + offset, std::forward<Args>(args)...);
			auto const mutable_position = begin(*this) + offset;
			auto const pointer = ::containers::uninitialized_move_destroy(begin(*this), mutable_position, data(temp), allocator_);
			assert(data(temp) + offset == pointer);
			::containers::uninitialized_move_destroy(mutable_position, end(*this), std::next(pointer), allocator_);
			this->relocate_preallocated(std::move(temp));
			add_size(1_bi);
		}
		return begin(*this) + offset;
	}

	// TODO: Check if the range lies within the container
	// TODO: remove duplication between this and static_vector insert
	template<typename ForwardIterator, typename Sentinel>
	auto insert(const_iterator const position, ForwardIterator first, Sentinel last) {
		check_iterator_validity(position);
		if (position == end(*this)) {
			return append(*this, first, last);
		}

		auto const range_size = bounded::throw_policy<std::out_of_range>{}.assignment(
			::containers::distance(first, last),
			0_bi,
			dynamic_resizable_array::size_type::max()
		);
		if (range_size <= capacity()) {
			auto const mutable_position = detail::put_in_middle_no_reallocation(*this, position, first, last, range_size, get_allocator());
			
			add_size(range_size);
			return mutable_position;
		}
		
		// There is a reallocation required, so just put everything in the
		// correct place to begin with
		auto temp = this->make_storage(new_capacity());
		auto const offset = position - begin(*this);
		// First construct the new element because it may reference an old
		// element, and we do not want to move elements it references
		::containers::uninitialized_copy(first, last, data(temp) + offset, get_allocator());
		auto const mutable_position = begin(*this) + offset;
		auto const pointer = ::containers::uninitialized_move_destroy(begin(*this), mutable_position, data(temp), get_allocator());
		assert(data(temp) + offset == pointer);
		::containers::uninitialized_move_destroy(mutable_position, end(*this), pointer + range_size, get_allocator());
		this->relocate_preallocated(std::move(temp));
		add_size(range_size);
		return mutable_position;
	}


	void pop_back() {
		assert(!empty(*this));
		::containers::detail::destroy(get_allocator(), std::addressof(back(*this)));
		add_size(-1_bi);
	}
	
private:
	template<typename Size>
	auto add_size(Size const s) BOUNDED_NOEXCEPT_GCC_BUG(
		this->set_size(size(*this) + s)
	)
	constexpr auto check_iterator_validity(const_iterator it) {
		assert(it >= begin(*this));
		assert(it <= end(*this));
		static_cast<void>(it);
	}
	constexpr auto new_capacity() const {
		return static_cast<size_type>(bounded::max(1_bi, capacity() * 2_bi));
	}
};


}	// namespace detail

template<typename Container>
struct is_container_c<detail::dynamic_resizable_array<Container>> : std::true_type {};

}	// namespace containers
