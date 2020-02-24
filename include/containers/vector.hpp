// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/dynamic_array.hpp>
#include <containers/index_type.hpp>
#include <containers/is_container.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <operators/forward.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T>
struct vector {
	using value_type = T;
	using size_type = typename detail::dynamic_array_data<value_type>::size_type;

	using const_iterator = contiguous_iterator<value_type const, bounded::detail::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::detail::builtin_max_value<size_type>>;

	constexpr vector() = default;

	template<range Range> requires(
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit vector(Range && source) {
		append(*this, OPERATORS_FORWARD(source));
	}
	
	constexpr vector(std::initializer_list<value_type> init) {
		append(*this, init);
	}

	constexpr vector(vector && other) noexcept:
		m_container(std::move(other.m_container)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}

	constexpr vector(vector const & other) {
		append(*this, other);
	}

	constexpr auto & operator=(vector && other) & noexcept {
		::containers::detail::destroy_range(*this);
		m_container = std::move(other.m_container);
		m_size = std::exchange(other.m_size, 0_bi);
		return *this;
	}
	constexpr auto & operator=(vector const & other) & {
		assign(*this, other);
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
	}

	constexpr ~vector() {
		::containers::detail::destroy_range(*this);
	}

	friend constexpr auto begin(vector const & container) {
		return const_iterator(::containers::detail::static_or_reinterpret_cast<value_type const *>(data(container.m_container)));
	}
	friend constexpr auto begin(vector & container) {
		return iterator(::containers::detail::static_or_reinterpret_cast<value_type *>(data(container.m_container)));
	}
	friend constexpr auto end(vector const & container) {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(vector & container) {
		return begin(container) + container.m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	constexpr auto capacity() const {
		return ::containers::size(m_container);
	}
	constexpr auto reserve(size_type const requested_capacity) {
		if (requested_capacity <= capacity()) {
			return;
		}
		auto temp = make_storage(requested_capacity);
		containers::uninitialized_move_destroy(
			*this,
			temp.data()
		);
		m_container = std::move(temp);
		// m_size remains the same
	}

	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		m_size = size(*this) + count;
	}

private:
	constexpr auto make_storage(auto const new_capacity) {
		return raw_container(repeat_default_n<typename raw_container::value_type>(new_capacity));
	}
	
	using raw_container = dynamic_array<trivial_storage<value_type>>;
	raw_container m_container;
	size_type m_size = 0_bi;
};

template<typename Range>
vector(Range &&) -> vector<std::decay_t<typename std::decay_t<Range>::value_type>>;

template<typename T>
inline constexpr auto is_container<vector<T>> = true;

}	// namespace containers
