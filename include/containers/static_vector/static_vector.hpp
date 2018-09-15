// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/index_type.hpp>
#include <containers/insert_emplace_impl.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>
#include <containers/array/array.hpp>

#include <bounded/detail/forward.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

// literal types cannot have destructors. If T is trivially destructible, we
// can skip calling their destructors, which could allow static_vector<T, n> to
// be a literal type.
//
// This should also improve debug-mode performance.

template<typename T, std::size_t capacity, bool trivial = std::is_trivially_destructible_v<T>>
struct static_vector_data;

template<typename T, std::size_t capacity>
struct static_vector_data<T, capacity, true> {
	array<trivial_storage<T>, capacity> m_container = {{}};
	bounded::integer<0, bounded::detail::normalize<capacity>> m_size = 0_bi;
};

template<typename T, std::size_t capacity>
struct static_vector_data<T, capacity, false> : static_vector_data<T, capacity, true> {
	~static_vector_data() {
		auto const first = begin(this->m_container);
		auto const last = first + this->m_size;
		detail::destroy_range(first, last);
	}
};


}	// namespace detail

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector : private detail::static_vector_data<T, capacity_> {
private:
	enum class count_constructor{};
public:
	using value_type = T;
	using size_type = bounded::integer<0, bounded::detail::normalize<capacity_>>;
	using const_iterator = contiguous_iterator<value_type const, capacity_>;
	using iterator = contiguous_iterator<value_type, capacity_>;

	constexpr static_vector() = default;
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	constexpr explicit static_vector(Count const count) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count)
	) {}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	constexpr static_vector(Count const count, value_type const & value) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count, value)
	) {}
	
	template<typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator, Sentinel>)>
	constexpr static_vector(InputIterator first, Sentinel const last) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(std::declval<static_vector &>().emplace_back(*first))) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	
	constexpr static_vector(static_vector const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(other), end(other))
	) {}
	constexpr static_vector(static_vector && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(std::move(other)), end(std::move(other)))
	) {}

	constexpr static_vector(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(begin(init), end(init))
	) {}

	constexpr auto & operator=(static_vector const & other) & noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable_v<value_type>) {
		assign(*this, begin(std::move(other)), end(std::move(other)));
		return *this;
	}
	constexpr auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<static_vector &>(), containers::begin(init), containers::end(init)))) {
		assign(*this, begin(init), end(init));
		return *this;
	}


	friend constexpr auto begin(static_vector const & container) noexcept {
		return const_iterator(detail::static_or_reinterpret_cast<value_type const *>(data(container.m_container)));
	}
	friend constexpr auto begin(static_vector & container) noexcept {
		return iterator(detail::static_or_reinterpret_cast<value_type *>(data(container.m_container)));
	}
	friend constexpr auto end(static_vector const & container) noexcept {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(static_vector & container) noexcept {
		return begin(container) + container.m_size;
	}


	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(static_vector)
	
	static constexpr auto capacity() noexcept {
		return bounded::constant<capacity_>;
	}
	
	// Assumes that elements are already constructed in the spare capacity
	template<typename Integer>
	constexpr void append_from_capacity(Integer const count) noexcept {
		this->m_size += count;
	}
	
	template<typename... Args>
	constexpr auto & emplace_back(Args && ... args) {
		assert(size(*this) != capacity());
		bounded::construct(*(data(*this) + size(*this)), BOUNDED_FORWARD(args)...);
		append_from_capacity(1_bi);
		return back(*this);
	}
	
	template<typename... Args>
	constexpr auto emplace(const_iterator const position, Args && ... args) {
		auto relocating_emplace = []{
			assert(false);
		};
		return detail::emplace_impl(*this, position, relocating_emplace, BOUNDED_FORWARD(args)...);
	}
	template<typename Range = std::initializer_list<value_type>>
	constexpr auto insert(const_iterator const position, Range && range) {
		return detail::insert_impl(*this, position, BOUNDED_FORWARD(range), [](auto) {
			assert(false);
			return iterator{};
		});
	}
	
	constexpr auto pop_back() {
		assert(!empty(*this));
		bounded::destroy(back(*this));
		--this->m_size;
	}


private:
	template<typename Count, typename... MaybeInitializer>
	constexpr explicit static_vector(count_constructor, Count const count, MaybeInitializer && ... args) noexcept(std::is_nothrow_constructible_v<value_type, MaybeInitializer && ...>) {
		static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
		for (auto const n : integer_range(count)) {
			static_cast<void>(n);
			emplace_back(BOUNDED_FORWARD(args)...);
		}
	}
};

template<typename Size, typename T>
static_vector(Size const size, T const & value) -> static_vector<T, static_cast<std::size_t>(std::numeric_limits<Size>::max())>;


template<typename T, std::size_t capacity>
constexpr auto is_container<static_vector<T, capacity>> = true;

}	// namespace containers
