// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/is_iterator.hpp>
#include <containers/operator_bracket.hpp>
#include <containers/repeat_n.hpp>
#include <containers/uninitialized_storage.hpp>

#include <bounded/detail/forward.hpp>

#include <iterator>
#include <memory>

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

template<typename T>
struct dynamic_array_data {
	using size_type = bounded::integer<0, maximum_array_size<T>>;
	
	constexpr dynamic_array_data() noexcept = default;

	template<typename Size>
	constexpr dynamic_array_data(T * const pointer_, Size const size_) noexcept:
		pointer(pointer_),
		size(static_cast<size_type>(size_))
	{
	}

	T * pointer = nullptr;
	size_type size = 0_bi;
};

template<typename T>
constexpr auto begin(dynamic_array_data<T> const container) noexcept {
	return container.pointer;
}
template<typename T>
constexpr auto end(dynamic_array_data<T> const container) noexcept {
	return begin(container) + container.size;
}


template<typename T, typename Size>
constexpr auto make_storage(Size const size) {
	return dynamic_array_data(
		std::allocator<T>{}.allocate(static_cast<std::size_t>(size)),
		size
	);
}

template<typename T>
auto deallocate_storage(dynamic_array_data<T> const data) noexcept {
	std::allocator<T>{}.deallocate(
		data.pointer,
		static_cast<std::size_t>(data.size)
	);
}



template<typename T>
constexpr auto cleanup(dynamic_array_data<T> const data) noexcept {
	detail::destroy_range(begin(data), end(data));
	deallocate_storage(data);
}



template<
	typename T,
	typename ForwardIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)
>
auto dynamic_array_initializer(ForwardIterator first, Sentinel const last) {
	auto const data = make_storage<T>(::containers::distance(first, last));
	try {
		containers::uninitialized_copy(first, last, begin(data));
	} catch(...) {
		deallocate_storage(data);
		throw;
	}
	return data;
}


template<
	typename T,
	typename Size,
	BOUNDED_REQUIRES(std::is_convertible_v<Size, typename dynamic_array_data<T>::size_type>)
>
auto dynamic_array_initializer(Size const size) {
	auto const data = make_storage<T>(size);
	try {
		containers::uninitialized_default_construct(begin(data), end(data));
	} catch(...) {
		deallocate_storage(data);
		throw;
	}
	return data;
}
	

}	// namespace detail

template<typename T>
struct dynamic_array {
	using value_type = T;
	using size_type = typename detail::dynamic_array_data<value_type>::size_type;

	using const_iterator = contiguous_iterator<value_type const, size_type::max().value()>;
	using iterator = contiguous_iterator<value_type, size_type::max().value()>;
	
	constexpr dynamic_array() = default;

	template<typename ForwardIterator, typename Sentinel, BOUNDED_REQUIRES(is_iterator_sentinel<ForwardIterator, Sentinel>)>
	constexpr dynamic_array(ForwardIterator first, Sentinel const last):
		m_data(::containers::detail::dynamic_array_initializer<value_type>(first, last))
	{
	}
	
	constexpr dynamic_array(std::initializer_list<value_type> init):
		dynamic_array(begin(init), end(init))
	{
	}

	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	explicit constexpr dynamic_array(Count const count):
		m_data(::containers::detail::dynamic_array_initializer<value_type>(count))
	{
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible_v<Count, size_type>)>
	constexpr dynamic_array(Count const count, value_type const & value) {
		auto const range = repeat_n(count, value);
		m_data = ::containers::detail::dynamic_array_initializer<value_type>(begin(range), end(range));
	}
	
	constexpr dynamic_array(dynamic_array const & other):
		dynamic_array(begin(other), end(other))
	{
	}
	
	constexpr dynamic_array(dynamic_array && other) noexcept:
		m_data(std::exchange(other.m_data, {}))
	{
	}
	
	~dynamic_array() noexcept {
		detail::cleanup(m_data);
	}

	constexpr auto & operator=(dynamic_array const & other) & {
		assign(*this, begin(other), end(other));
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		detail::cleanup(m_data);
		m_data = std::exchange(other.m_data, {});
		return *this;
	}
	
	friend constexpr auto begin(dynamic_array const & container) noexcept {
		return const_iterator(container.m_data.pointer);
	}
	friend constexpr auto begin(dynamic_array & container) noexcept {
		return iterator(container.m_data.pointer);
	}
	
	friend constexpr auto end(dynamic_array const & container) noexcept {
		return begin(container) + container.m_data.size;
	}
	friend constexpr auto end(dynamic_array & container) noexcept {
		return begin(container) + container.m_data.size;
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(dynamic_array)
	
private:
	detail::dynamic_array_data<value_type> m_data = {};
};


template<typename T>
constexpr auto is_container<dynamic_array<T>> = true;


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
