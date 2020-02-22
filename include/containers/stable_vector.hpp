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
	constexpr stable_vector():
		m_storage(std::allocator<T>{}.allocate(static_cast<std::ptrdiff_t>(capacity()))),
		m_size(0_bi)
	{
	}

	template<typename Range> requires(
		range<Range> and
		!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>
	)
	constexpr explicit stable_vector(Range && range):
		stable_vector()
	{
		::containers::append(*this, OPERATORS_FORWARD(range));
	}
	
	constexpr stable_vector(std::initializer_list<value_type> init):
		stable_vector()
	{
		::containers::append(*this, init);
	}
	
	// TODO: Support trivial relocatability
	constexpr stable_vector(stable_vector && other) noexcept:
		m_storage(std::move(other.m_storage)),
		m_size(std::exchange(other.m_size, 0_bi))
	{
	}
	constexpr stable_vector(stable_vector const & other):
		stable_vector()
	{
		::containers::uninitialized_copy(begin(other), end(other), begin(*this));
		m_size = other.m_size;
	}

	constexpr ~stable_vector() {
		detail::destroy_range(*this);
	}

	constexpr auto & operator=(stable_vector && other) & noexcept {
		swap(*this, other);
		return *this;
	}
	constexpr auto & operator=(stable_vector const & other) & {
		if (!m_storage) {
			*this = stable_vector();
		}
		assign(*this, other);
		return *this;
	}

	constexpr auto & operator=(std::initializer_list<value_type> init) & {
		assign(*this, init);
		return *this;
	}

	friend constexpr auto swap(stable_vector & lhs, stable_vector & rhs) noexcept {
		swap(lhs.m_storage, rhs.m_storage);
		std::swap(lhs.m_size, rhs.m_size);
	}

	friend constexpr auto begin(stable_vector const & container) {
		return const_iterator(container.m_storage.get());
	}
	friend constexpr auto begin(stable_vector & container) {
		return iterator(container.m_storage.get());
	}
	friend constexpr auto end(stable_vector const & container) {
		return begin(container) + container.m_size;
	}
	friend constexpr auto end(stable_vector & container) {
		return begin(container) + container.m_size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
	// Assumes that elements are already constructed in the spare capacity
	constexpr void append_from_capacity(auto const count) {
		BOUNDED_ASSERT(count + m_size <= capacity());
		m_size += count;
	}
private:
	struct deleter {
		constexpr auto operator()(T * ptr) const {
			std::allocator<T>{}.deallocate(ptr, static_cast<std::ptrdiff_t>(capacity()));
		}
	};
	#if 0
	// TODO: When std::unique_ptr is constexpr, implement as
	std::unique_ptr<T, deleter> m_storage;
	#endif
	struct unique_ptr {
		constexpr explicit unique_ptr(T * ptr):
			m_ptr(ptr)
		{
		}
		constexpr unique_ptr(unique_ptr && other):
			m_ptr(other.release())
		{
		}
		constexpr unique_ptr & operator=(unique_ptr && other) & {
			m_ptr = other.release();
		}
		constexpr ~unique_ptr() {
			deleter()(m_ptr);
		}
		friend constexpr void swap(unique_ptr & lhs, unique_ptr & rhs) {
			std::swap(lhs.m_ptr, rhs.m_ptr);
		}

		constexpr auto get() const {
			return m_ptr;
		}
		constexpr auto release() {
			return std::exchange(m_ptr, nullptr);
		}

		constexpr operator bool() const {
			return m_ptr;
		}
	private:
		T * m_ptr;
	};
	unique_ptr m_storage;
	size_type m_size;

};

template<typename T, std::size_t capacity>
inline constexpr auto is_container<stable_vector<T, capacity>> = true;

}	// namespace containers