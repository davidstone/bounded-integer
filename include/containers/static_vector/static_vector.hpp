// std::vector-like interface around a bounded::array
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <containers/array/array.hpp>
#include <containers/array/iterator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/repeat_n.hpp>
#include <containers/static_vector/forward_declaration.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded_integer/integer_range.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace containers {

// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct static_vector {
	using value_type = T;
	using size_type = bounded::integer<0, capacity_>;
	using const_iterator = detail::basic_array_iterator<value_type const, static_vector>;
	using iterator = detail::basic_array_iterator<value_type, static_vector>;
	
	static_vector() = default;
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	explicit static_vector(Count const count) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count)
	) {}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	static_vector(Count const count, value_type const & value) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(count_constructor{}, count, value)
	) {}
	
	template<typename InputIterator, typename Sentinel>
	static_vector(InputIterator first, Sentinel const last) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(emplace_back(*first))) {
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}
	
	static_vector(static_vector const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(other.begin(), other.end())
	) {}
	static_vector(static_vector && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()))
	) {}

	static_vector(std::initializer_list<value_type> init) BOUNDED_NOEXCEPT_INITIALIZATION(
		static_vector(init.begin(), init.end())
	) {}

	auto & operator=(static_vector const & other) & noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
		assign(*this, other.begin(), other.end());
		return *this;
	}
	auto & operator=(static_vector && other) & noexcept(std::is_nothrow_move_assignable<value_type>::value) {
		assign(*this, std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
		return *this;
	}
	auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<static_vector &>(), init.begin(), init.end()))) {
		assign(*this, init.begin(), init.end());
		return *this;
	}

	~static_vector() {
		clear(*this);
	}


	auto begin() const noexcept {
		return const_iterator(data());
	}
	auto begin() noexcept {
		return iterator(data());
	}
	auto end() const noexcept {
		return begin() + m_size;
	}
	auto end() noexcept {
		return begin() + m_size;
	}


	auto && operator[](index_type<static_vector> index) const noexcept {
		return *(begin() + index);
	}
	auto && operator[](index_type<static_vector> index) noexcept {
		return *(begin() + index);
	}
	
	auto data() const noexcept {
		return reinterpret_cast<value_type const *>(m_container.data());
	}
	auto data() noexcept {
		return reinterpret_cast<value_type *>(m_container.data());
	}

	static constexpr auto capacity() noexcept {
		return bounded::constant<capacity_>;
	}

	template<typename... Args>
	auto emplace_back(Args && ... args) {
		assert(size(*this) != capacity());
		new(m_container.data() + size(*this)) value_type(std::forward<Args>(args)...);
		++m_size;
	}
	
	template<typename... Args>
	auto emplace(const_iterator const position_, Args && ... args) {
		auto const position = detail::make_mutable_iterator(*this, position_);
		if (position == end()) {
			emplace_back(std::forward<Args>(args)...);
			return position;
		}
		emplace_back(std::move(back(*this)));
		std::move(position, bounded::prev(end()), bounded::next(position));
		auto const pointer = std::addressof(*position);
		pointer->~value_type();
		::new(static_cast<void *>(pointer)) value_type(std::forward<Args>(args)...);
		return position;
	}
	template<typename InputIterator, typename Sentinel>
	auto insert(const_iterator const position_, InputIterator first, Sentinel last) {
		auto const position = detail::make_mutable_iterator(*this, position_);
		if (position == end()) {
			for (; first != last; ++first) {
				emplace_back(*first);
			}
			return position;
		}
		// TODO: This will likely need std::uninitialized_copy and std::move
		return position;
	}


	void pop_back() {
		back(*this).~value_type();
		--m_size;
	}
	
private:
	enum class count_constructor{};
	template<typename Count, typename... MaybeInitializer>
	explicit static_vector(count_constructor, Count const count, MaybeInitializer && ... args) noexcept(std::is_nothrow_constructible<value_type, MaybeInitializer && ...>::value) {
		static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
		for (auto const n : bounded::integer_range(count)) {
			static_cast<void>(n);
			emplace_back(std::forward<MaybeInitializer>(args)...);
		}
	}
	
	array<uninitialized_storage<value_type>, capacity_> m_container = {{}};
	size_type m_size = bounded::constant<0>;
};

}	// namespace containers
