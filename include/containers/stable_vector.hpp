// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/array/array.hpp>
#include <containers/append.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/index_type.hpp>
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <operators/bracket.hpp>

namespace containers {

// See https://probablydance.com/2013/05/13/4gb-per-vector/
// TODO: ensure proper exception safety
template<typename T, std::size_t capacity_>
struct stable_vector {
	static constexpr auto capacity() {
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
		range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	explicit stable_vector(Range && range):
		stable_vector()
	{
		::containers::append(*this, OPERATORS_FORWARD(range));
	}
	
	stable_vector(std::initializer_list<value_type> init):
		stable_vector()
	{
		::containers::append(*this, init);
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
	auto & operator=(stable_vector const & other) & {
		if (!m_storage) {
			*this = stable_vector();
		}
		assign(*this, other);
		return *this;
	}

	auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
	}

	friend auto swap(stable_vector & lhs, stable_vector & rhs) noexcept {
		std::swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	friend auto begin(stable_vector const & container) {
		return const_iterator(container.m_storage.get());
	}
	friend auto begin(stable_vector & container) {
		return iterator(container.m_storage.get());
	}
	friend auto end(stable_vector const & container) {
		return begin(container) + container.m_size;
	}
	friend auto end(stable_vector & container) {
		return begin(container) + container.m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	// Assumes that elements are already constructed in the spare capacity
	void append_from_capacity(auto const count) {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}
private:
	struct deleter {
		auto operator()(T * ptr) const {
			std::allocator<T>{}.deallocate(ptr, static_cast<std::ptrdiff_t>(capacity()));
		}
	};
	std::unique_ptr<T, deleter> m_storage;
	size_type m_size;

};

template<typename T, std::size_t capacity>
inline constexpr auto is_container<stable_vector<T, capacity>> = true;

}	// namespace containers