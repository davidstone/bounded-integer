// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/is_iterator.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/array/iterator.hpp>

#include <iterator>

namespace containers {
namespace detail {

// TODO: support larger array sizes
template<typename T>
constexpr auto maximum_array_size = static_cast<std::intmax_t>(
	bounded::min(
		std::numeric_limits<std::ptrdiff_t>::max() / static_cast<std::ptrdiff_t>(sizeof(T)),
		(1LL << 31LL) - 1
	)
);


template<typename T, typename Allocator>
using rebound_allocator = typename allocator_traits<Allocator>::template rebind_alloc<T>;


// Because this does not have an allocator, it cannot free its own memory
template<typename T>
struct dynamic_array_without_allocator {
	using value_type = T;
	using size_type = bounded::integer<0, detail::maximum_array_size<T>>;

	using const_iterator = detail::basic_array_iterator<value_type const, dynamic_array_without_allocator>;
	using iterator = detail::basic_array_iterator<value_type, dynamic_array_without_allocator>;
	
	constexpr dynamic_array_without_allocator() = default;

	template<typename ForwardIterator, typename Sentinel, typename Allocator, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
	dynamic_array_without_allocator(ForwardIterator first, Sentinel const last, Allocator & allocator):
		m_size(::containers::distance(first, last)),
		m_data(make_storage(allocator, m_size))
	{
		try {
			::containers::uninitialized_copy(first, last, m_data, allocator);
		} catch(...) {
			deallocate_storage(allocator);
			throw;
		}
	}
	
	template<typename Count, typename Allocator, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	dynamic_array_without_allocator(Count const count, Allocator & allocator):
		m_size(count),
		m_data(make_storage(allocator, m_size))
	{
		try {
			::containers::uninitialized_default_construct(begin(), end(), allocator);
		} catch(...) {
			deallocate_storage(allocator);
			throw;
		}
	}
	
	template<typename Count, typename Allocator, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr dynamic_array_without_allocator(Count const count, value_type const & value, Allocator & allocator) {
		auto const range = ::containers::detail::repeat_n(count, value);
		*this = dynamic_array_without_allocator(range.begin(), range.end(), allocator);
	}
	
	constexpr dynamic_array_without_allocator(dynamic_array_without_allocator && other) noexcept:
		m_size(std::move(other.m_size)),
		m_data(std::move(other.m_data))
	{
		other.m_size = bounded::constant<0>;
		other.m_data = nullptr;
	}
	
	template<typename Allocator>
	constexpr auto move_assign(dynamic_array_without_allocator && other, Allocator & allocator) & noexcept {
		deallocate_storage(allocator);
		m_size = std::move(other.m_size);
		m_data = std::move(other.m_data);
		other.m_size = bounded::constant<0>;
		other.m_data = nullptr;
	}
	
	constexpr auto data() const noexcept {
		return reinterpret_cast<value_type const *>(m_data);
	}
	constexpr auto data() noexcept {
		return reinterpret_cast<value_type *>(m_data);
	}
	
	constexpr auto begin() const noexcept {
		return const_iterator(data());
	}
	constexpr auto begin() noexcept {
		return iterator(data());
	}
	constexpr auto end() const noexcept {
		return begin() + m_size;
	}
	constexpr auto end() noexcept {
		return begin() + m_size;
	}


	constexpr auto && operator[](index_type<dynamic_array_without_allocator> const index) const noexcept {
		return *(begin() + index);
	}
	constexpr auto && operator[](index_type<dynamic_array_without_allocator> const index) noexcept {
		return *(begin() + index);
	}
	
	// No point changing the size; these are used right before destruction
	template<typename Allocator>
	auto cleanup(Allocator && allocator) noexcept {
		::containers::detail::destroy(allocator, begin(), end());
		deallocate_storage(allocator);
	}
	
private:
	using underlying_storage = uninitialized_storage<value_type>;

	template<typename Allocator, typename Size>
	static constexpr auto make_storage(Allocator & allocator, Size const size) {
		using allocator_type = rebound_allocator<underlying_storage, Allocator>;
		auto a = static_cast<allocator_type>(allocator);
		return allocator_traits<allocator_type>::allocate(a, static_cast<std::size_t>(size));
	}

	template<typename Allocator>
	auto deallocate_storage(Allocator && allocator) noexcept {
		using allocator_type = rebound_allocator<underlying_storage, Allocator>;
		auto a = static_cast<allocator_type>(allocator);
		allocator_traits<allocator_type>::deallocate(a, m_data, static_cast<std::size_t>(m_size));
	}

	size_type m_size = bounded::constant<0>;
	underlying_storage * m_data = nullptr;
};

template<typename T, typename ForwardIterator, typename Sentinel, typename Allocator, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
auto assign(dynamic_array_without_allocator<T> & container, ForwardIterator first, Sentinel const last, Allocator & allocator) {
	auto const difference = ::containers::distance(first, last);
	if (difference == size(container)) {
		::containers::copy(first, last, container.begin());
	} else {
		clear(container);
		container = dynamic_array_without_allocator<T>(first, last, allocator);
	}
}

}	// namespace detail

// TODO: Handle allocator transfer in copies / moves
template<typename T, typename Allocator = allocator<T>>
struct dynamic_array : private detail::rebound_allocator<T, Allocator> {
private:
	using base_container = detail::dynamic_array_without_allocator<T>;
public:
	using value_type = typename base_container::value_type;
	using size_type = typename base_container::size_type;

	using allocator_type = detail::rebound_allocator<T, Allocator>;
	static_assert(std::is_empty<allocator_type>::value, "Stateful allocators not yet supported.");

	using const_iterator = detail::basic_array_iterator<value_type const, dynamic_array>;
	using iterator = detail::basic_array_iterator<value_type, dynamic_array>;
	
	constexpr auto && get_allocator() const & noexcept {
		return static_cast<allocator_type const &>(*this);
	}
	constexpr auto && get_allocator() & noexcept {
		return static_cast<allocator_type &>(*this);
	}
	constexpr auto && get_allocator() && noexcept {
		return static_cast<allocator_type &&>(*this);
	}
	
	constexpr dynamic_array() = default;

	constexpr explicit dynamic_array(allocator_type allocator):
		allocator_type(std::move(allocator))
	{
	}
	
	template<typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
	constexpr dynamic_array(ForwardIterator first, Sentinel const last, allocator_type allocator = allocator_type{}):
		allocator_type(std::move(allocator)),
		m_base(first, last, get_allocator())
	{
	}
	
	constexpr dynamic_array(std::initializer_list<value_type> init, allocator_type allocator = allocator_type{}):
		dynamic_array(init.begin(), init.end(), std::move(allocator))
	{
	}

	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	explicit constexpr dynamic_array(Count const count, allocator_type allocator = allocator_type{}):
		allocator_type(std::move(allocator)),
		m_base(count, get_allocator())
	{
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	constexpr dynamic_array(Count const count, value_type const & value, allocator_type allocator = allocator_type{}):
		allocator_type(std::move(allocator)),
		m_base(count, value, get_allocator())
	{
	}
	
	constexpr dynamic_array(dynamic_array const & other, allocator_type allocator):
		dynamic_array(other.begin(), other.end(), std::move(allocator))
	{
	}
	constexpr dynamic_array(dynamic_array const & other):
		dynamic_array(other, other.get_allocator())
	{
	}
	
	constexpr dynamic_array(dynamic_array && other, allocator_type allocator) noexcept:
		allocator_type(std::move(allocator)),
		m_base(std::move(other).m_base)
	{
	}
	constexpr dynamic_array(dynamic_array && other) noexcept:
		dynamic_array(std::move(other), std::move(other).get_allocator())
	{
	}
	
	~dynamic_array() noexcept {
		m_base.cleanup(get_allocator());
	}

	constexpr auto & operator=(dynamic_array const & other) & {
		assign(*this, other.begin(), other.end());
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		m_base.move_assign(std::move(other.m_base), get_allocator());
		return *this;
	}
	
	constexpr auto data() const noexcept {
		return m_base.data();
	}
	constexpr auto data() noexcept {
		return m_base.data();
	}
	
	constexpr auto begin() const noexcept {
		return const_iterator(data());
	}
	constexpr auto begin() noexcept {
		return iterator(data());
	}
	constexpr auto end() const noexcept {
		return begin() + size(m_base);
	}
	constexpr auto end() noexcept {
		return begin() + size(m_base);
	}

	constexpr auto && operator[](index_type<dynamic_array> const index) const noexcept {
		return *(begin() + index);
	}
	constexpr auto && operator[](index_type<dynamic_array> const index) noexcept {
		return *(begin() + index);
	}
	
private:
	base_container m_base;
};


template<typename T, typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
auto assign(dynamic_array<T> & container, ForwardIterator first, Sentinel const last) {
	auto const difference = ::containers::distance(first, last);
	if (difference == size(container)) {
		::containers::copy(first, last, container.begin());
	} else {
		clear(container);
		container = dynamic_array<T>(first, last);
	}
}

namespace detail {

template<typename T, typename Size, typename... MaybeInitializer>
auto resize(common_resize_tag, dynamic_array<T> & container, Size const count, MaybeInitializer && ... args) {
	static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
	if (size(container) != count) {
		// Technically, the clear is unnecessary, but calling it decreases peak
		// memory use.
		clear(container);
		container = dynamic_array<T>(count, std::forward<MaybeInitializer>(args)...);
	}
}

}	// namespace detail

}	// namespace containers
