// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/array/array.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/emplace_back.hpp>
#include <containers/index_type.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct stable_vector {
	static constexpr auto capacity() noexcept {
		return bounded::constant<bounded::detail::normalize<capacity_>>;
	}

	using value_type = T;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<static_cast<std::ptrdiff_t>(capacity_)>
	>;
	using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(capacity_)>;
	using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(capacity_)>;

	// Allocates the full capacity
	stable_vector():
		m_storage(std::allocator<T>{}.allocate(static_cast<std::ptrdiff_t>(capacity()))),
		m_size(0_bi)
	{
	}

	template<typename Range> requires(
		is_range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	explicit stable_vector(Range && range):
		stable_vector()
	{
		for (decltype(auto) value : BOUNDED_FORWARD(range)) {
			::containers::emplace_back(*this, BOUNDED_FORWARD(value));
		}
	}
	
	stable_vector(std::initializer_list<value_type> init):
		stable_vector()
	{
		for (auto const & value : init) {
			::containers::emplace_back(*this, value);
		}
	}
	
	// TODO: Support trivial relocatability
	stable_vector(stable_vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}
	stable_vector(stable_vector const & other):
		stable_vector()
	{
		std::uninitialized_copy(begin(other), end(other), begin(*this));
		m_size = other.m_size;
	}

	~stable_vector() {
		detail::destroy_range(*this);
	}

	auto & operator=(stable_vector && other) & noexcept {
		swap(*this, other);
		return *this;
	}
	auto & operator=(stable_vector const & other) & noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
		if (!m_storage) {
			*this = stable_vector();
		}
		assign(*this, begin(other), end(other));
		return *this;
	}

	auto & operator=(std::initializer_list<value_type> init) & noexcept(noexcept(assign(std::declval<stable_vector &>(), containers::begin(init), containers::end(init)))) {
		assign(*this, begin(init), end(init));
		return *this;
	}

	friend auto swap(stable_vector & lhs, stable_vector & rhs) noexcept {
		std::swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	friend auto begin(stable_vector const & container) noexcept {
		return const_iterator(container.m_storage.get());
	}
	friend auto begin(stable_vector & container) noexcept {
		return iterator(container.m_storage.get());
	}
	friend auto end(stable_vector const & container) noexcept {
		return begin(container) + container.m_size;
	}
	friend auto end(stable_vector & container) noexcept {
		return begin(container) + container.m_size;
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(stable_vector)
	
	// Assumes that elements are already constructed in the spare capacity
	template<typename Integer>
	void append_from_capacity(Integer const count) noexcept {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}
private:
	struct deleter {
		auto operator()(T * ptr) const noexcept {
			std::allocator<T>{}.deallocate(ptr, static_cast<std::ptrdiff_t>(capacity()));
		}
	};
	std::unique_ptr<T, deleter> m_storage;
	size_type m_size;

};

template<typename T, std::size_t capacity>
inline constexpr auto is_container<stable_vector<T, capacity>> = true;

}	// namespace containers