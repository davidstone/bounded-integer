// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/allocator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/is_iterator.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>
#include <containers/array/iterator.hpp>

#include <bounded/detail/forward.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

// TODO: support larger array sizes
template<typename T>
constexpr auto maximum_array_size = static_cast<std::intmax_t>(
	bounded::min(
		bounded::constant<std::numeric_limits<std::ptrdiff_t>::max()> / bounded::size_of<T>,
		(1_bi << 31_bi) - 1_bi
	)
);

template<typename T, typename Allocator>
using rebound_allocator = typename allocator_traits<Allocator>::template rebind_alloc<T>;


template<typename T>
struct dynamic_array_data_t {
	using size_type = bounded::integer<0, maximum_array_size<T>>;
	
	T * pointer = nullptr;
	size_type size = 0_bi;
};

template<typename T>
constexpr auto begin(dynamic_array_data_t<T> const container) noexcept {
	return container.pointer;
}
template<typename T>
constexpr auto end(dynamic_array_data_t<T> const container) noexcept {
	return begin(container) + container.size;
}


template<typename T, typename Size>
constexpr auto dynamic_array_data(T * const pointer, Size const size) noexcept {
	return dynamic_array_data_t<T>{
		pointer,
		static_cast<typename dynamic_array_data_t<T>::size_type>(size)
	};
}

template<typename T, typename Allocator, typename Size>
constexpr auto make_storage(Allocator & allocator_, Size const size) {
	return dynamic_array_data(
		allocator_traits<Allocator>::allocate(allocator_, static_cast<std::size_t>(size)),
		size
	);
}

template<typename Allocator, typename T, typename Size>
auto deallocate_storage(Allocator & allocator_, T * pointer, Size const size) noexcept {
	allocator_traits<Allocator>::deallocate(
		allocator_,
		reinterpret_cast<typename Allocator::value_type *>(pointer),
		static_cast<std::size_t>(size)
	);
}

template<typename Allocator, typename T>
auto deallocate_storage(Allocator & allocator_, dynamic_array_data_t<T> const data) noexcept {
	return deallocate_storage(allocator_, data.pointer, data.size);
}



template<typename Allocator, typename T>
constexpr auto cleanup(Allocator & allocator_, dynamic_array_data_t<T> const data) noexcept {
	::containers::detail::destroy(allocator_, begin(data), end(data));
	deallocate_storage(allocator_, data);
}



template<
	typename ForwardIterator,
	typename Sentinel,
	typename Allocator,
	BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)
>
auto dynamic_array_initializer(ForwardIterator first, Sentinel const last, Allocator & allocator_) {
	using value_type = typename Allocator::value_type;
	auto const data = make_storage<value_type>(allocator_, ::containers::distance(first, last));
	try {
		::containers::uninitialized_copy(first, last, begin(data), allocator_);
	} catch(...) {
		deallocate_storage(allocator_, data);
		throw;
	}
	return data;
}


template<
	typename Size,
	typename Allocator,
	BOUNDED_REQUIRES(std::is_convertible<Size, typename dynamic_array_data_t<typename Allocator::value_type>::size_type>::value)
>
auto dynamic_array_initializer(Size const size, Allocator & allocator_) {
	using value_type = typename Allocator::value_type;
	auto const data = make_storage<value_type>(allocator_, size);
	try {
		::containers::uninitialized_default_construct(begin(data), end(data), allocator_);
	} catch(...) {
		deallocate_storage(allocator_, data);
		throw;
	}
	return data;
}
	

}	// namespace detail

// TODO: Handle allocator transfer in copies / moves
template<typename T, typename Allocator = allocator<T>>
struct dynamic_array : private detail::rebound_allocator<T, Allocator> {
	using value_type = T;
	using size_type = typename detail::dynamic_array_data_t<value_type>::size_type;

	using allocator_type = detail::rebound_allocator<value_type, Allocator>;
	static_assert(std::is_empty_v<allocator_type>, "Stateful allocators not yet supported.");

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

	constexpr explicit dynamic_array(allocator_type allocator_):
		allocator_type(std::move(allocator_))
	{
	}
	
	template<typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
	constexpr dynamic_array(ForwardIterator first, Sentinel const last, allocator_type allocator_ = allocator_type{}):
		allocator_type(std::move(allocator_)),
		m_data(::containers::detail::dynamic_array_initializer(first, last, get_allocator()))
	{
	}
	
	constexpr dynamic_array(std::initializer_list<value_type> init, allocator_type allocator_ = allocator_type{}):
		dynamic_array(begin(init), end(init), std::move(allocator_))
	{
	}

	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	explicit constexpr dynamic_array(Count const count, allocator_type allocator_ = allocator_type{}):
		allocator_type(std::move(allocator_)),
		m_data(::containers::detail::dynamic_array_initializer(count, get_allocator()))
	{
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	constexpr dynamic_array(Count const count, value_type const & value, allocator_type allocator_ = allocator_type{}):
		allocator_type(std::move(allocator_))
	{
		auto const range = ::containers::detail::repeat_n(count, value);
		m_data = ::containers::detail::dynamic_array_initializer(begin(range), end(range), get_allocator());
	}
	
	constexpr dynamic_array(dynamic_array const & other, allocator_type allocator_):
		dynamic_array(begin(other), end(other), std::move(allocator_))
	{
	}
	constexpr dynamic_array(dynamic_array const & other):
		dynamic_array(other, other.get_allocator())
	{
	}
	
	constexpr dynamic_array(dynamic_array && other, allocator_type allocator_) noexcept:
		allocator_type(std::move(allocator_)),
		m_data(std::exchange(other.m_data, {}))
	{
	}
	constexpr dynamic_array(dynamic_array && other) noexcept:
		dynamic_array(std::move(other), std::move(other).get_allocator())
	{
	}
	
	~dynamic_array() noexcept {
		::containers::detail::cleanup(get_allocator(), m_data);
	}

	constexpr auto & operator=(dynamic_array const & other) & {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		::containers::detail::cleanup(get_allocator(), m_data);
		m_data = std::exchange(other.m_data, {});
		return *this;
	}
	
	friend constexpr auto begin(dynamic_array const & container) noexcept {
		return const_iterator(container.m_data.pointer, detail::iterator_constructor);
	}
	friend constexpr auto begin(dynamic_array & container) noexcept {
		return iterator(container.m_data.pointer, detail::iterator_constructor);
	}
	
	friend constexpr auto end(dynamic_array const & container) noexcept {
		return begin(container) + container.m_data.size;
	}
	friend constexpr auto end(dynamic_array & container) noexcept {
		return begin(container) + container.m_data.size;
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(dynamic_array)
	
private:
	detail::dynamic_array_data_t<value_type> m_data = {};
};


template<typename T, typename Allocator>
constexpr auto is_container<dynamic_array<T, Allocator>> = true;


template<typename T, typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
auto assign(dynamic_array<T> & container, ForwardIterator first, Sentinel const last) {
	auto const difference = ::containers::distance(first, last);
	if (difference == size(container)) {
		::containers::copy(first, last, begin(container));
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
		container = dynamic_array<T>(count, BOUNDED_FORWARD(args)...);
	}
}

}	// namespace detail
}	// namespace containers
