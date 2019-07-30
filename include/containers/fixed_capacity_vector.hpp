// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/emplace_back.hpp>
#include <containers/index_type.hpp>
#include <containers/insert_emplace_impl.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/pop_back.hpp>
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
public:
	static constexpr auto capacity() noexcept {
		return Storage::capacity();
	}

	using value_type = typename Storage::value_type;
	using size_type = typename Storage::size_type;
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(capacity())>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(capacity())>;

	constexpr fixed_capacity_vector() = default;
	
	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit fixed_capacity_vector(Range && range) {
		for (decltype(auto) value : BOUNDED_FORWARD(range)) {
			::containers::emplace_back(*this, BOUNDED_FORWARD(value));
		}
	}
	
	constexpr fixed_capacity_vector(std::initializer_list<value_type> init) {
		for (auto const & value : init) {
			::containers::emplace_back(*this, value);
		}
	}
	
	fixed_capacity_vector(fixed_capacity_vector const & other) = default;
	fixed_capacity_vector(fixed_capacity_vector && other) = default;
	fixed_capacity_vector & operator=(fixed_capacity_vector && other) = default;
	fixed_capacity_vector & operator=(fixed_capacity_vector const & other) = default;

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
		BOUNDED_ASSERT(count + m_storage.size <= capacity());
		m_storage.size += count;
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
};

template<typename Storage>
inline constexpr auto is_container<fixed_capacity_vector<Storage>> = true;

}	// namespace containers
