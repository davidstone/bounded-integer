// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/index_type.hpp>
#include <containers/insert_emplace_impl.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/repeat_n.hpp>

#include <bounded/assert.hpp>
#include <bounded/detail/forward.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {


}	// namespace detail

// TODO: ensure proper exception safety
template<typename Storage>
struct fixed_capacity_vector {
private:
	Storage m_storage;
	enum class count_constructor{};
public:
	static constexpr auto capacity() noexcept {
		return Storage::capacity();
	}

	using value_type = typename Storage::value_type;
	using size_type = typename Storage::size_type;
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(capacity())>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(capacity())>;

	constexpr fixed_capacity_vector() = default;
	
	template<typename Count> requires std::is_convertible_v<Count, size_type>
	constexpr explicit fixed_capacity_vector(Count const count) BOUNDED_NOEXCEPT_INITIALIZATION(
		fixed_capacity_vector(count_constructor{}, count)
	) {}
	template<typename Count> requires std::is_convertible_v<Count, size_type>
	constexpr fixed_capacity_vector(Count const count, value_type const & value) BOUNDED_NOEXCEPT_INITIALIZATION(
		fixed_capacity_vector(count_constructor{}, count, value)
	) {}
	
	template<typename InputIterator, typename Sentinel> requires is_iterator_sentinel<InputIterator, Sentinel>
	constexpr fixed_capacity_vector(InputIterator first, Sentinel const last) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(std::declval<fixed_capacity_vector &>().emplace_back(*first))) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	
	constexpr fixed_capacity_vector(fixed_capacity_vector const & other) = default;
	constexpr fixed_capacity_vector(fixed_capacity_vector && other) = default;

	constexpr fixed_capacity_vector(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_INITIALIZATION(
		fixed_capacity_vector(begin(init), end(init))
	) {
	}
	
	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit fixed_capacity_vector(Range && range) BOUNDED_NOEXCEPT_INITIALIZATION(
		fixed_capacity_vector(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)))
	) {
	}

	constexpr auto & operator=(fixed_capacity_vector const & other) & noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(fixed_capacity_vector && other) & noexcept(
		std::is_swappable_v<Storage> ? std::is_nothrow_swappable_v<Storage> : std::is_nothrow_move_assignable_v<value_type>
	) {
		if constexpr (std::is_swappable_v<Storage>) {
			using std::swap;
			swap(m_storage, other.m_storage);
		} else {
			assign(*this, begin(std::move(other)), end(std::move(other)));
		}
		return *this;
	}
	constexpr auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<fixed_capacity_vector &>(), containers::begin(init), containers::end(init)))) {
		assign(*this, begin(init), end(init));
		return *this;
	}


	friend constexpr auto begin(fixed_capacity_vector const & container) noexcept {
		return const_iterator(begin(container.m_storage));
	}
	friend constexpr auto begin(fixed_capacity_vector & container) noexcept {
		return iterator(begin(container.m_storage));
	}
	friend constexpr auto end(fixed_capacity_vector const & container) noexcept {
		return const_iterator(end(container.m_storage));
	}
	friend constexpr auto end(fixed_capacity_vector & container) noexcept {
		return iterator(end(container.m_storage));
	}


	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(fixed_capacity_vector)
	
	// Assumes that elements are already constructed in the spare capacity
	template<typename Integer>
	constexpr void append_from_capacity(Integer const count) noexcept {
		m_storage.size += count;
	}
	
	template<typename... Args>
	constexpr auto & emplace_back(Args && ... args) {
		BOUNDED_ASSERT(size(*this) != capacity());
		bounded::construct(*(data(*this) + size(*this)), BOUNDED_FORWARD(args)...);
		append_from_capacity(1_bi);
		return back(*this);
	}
	
	template<typename... Args>
	constexpr auto emplace(const_iterator const position, Args && ... args) {
		auto relocating_emplace = []{
			BOUNDED_ASSERT_OR_ASSUME(false);
		};
		return detail::emplace_impl(*this, position, relocating_emplace, BOUNDED_FORWARD(args)...);
	}
	template<typename Range = std::initializer_list<value_type>>
	constexpr auto insert(const_iterator const position, Range && range) {
		return detail::insert_impl(*this, position, BOUNDED_FORWARD(range), [](auto) {
			BOUNDED_ASSERT_OR_ASSUME(false);
			return iterator{};
		});
	}
	
	constexpr auto pop_back() {
		bounded::destroy(back(*this));
		--m_storage.size;
	}


private:
	template<typename Count, typename... MaybeInitializer>
	constexpr explicit fixed_capacity_vector(count_constructor, Count const count, MaybeInitializer && ... args) noexcept(std::is_nothrow_constructible_v<value_type, MaybeInitializer && ...>) {
		static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
		for (auto const n [[maybe_unused]] : integer_range(count)) {
			emplace_back(BOUNDED_FORWARD(args)...);
		}
	}
};

template<typename Storage>
constexpr auto is_container<fixed_capacity_vector<Storage>> = true;

}	// namespace containers
