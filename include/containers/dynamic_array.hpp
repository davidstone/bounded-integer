// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/assign.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/common_functions.hpp>
#include <containers/compare_container.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/is_iterator.hpp>
#include <containers/maximum_array_size.hpp>
#include <containers/size.hpp>
#include <containers/uninitialized_storage.hpp>

#include <operators/forward.hpp>

#include <operators/bracket.hpp>

#include <iterator>
#include <memory>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename T>
struct dynamic_array_data {
	using size_type = bounded::integer<0, maximum_array_size<T>>;
	
	constexpr dynamic_array_data() = default;

	constexpr dynamic_array_data(T * const pointer_, auto const size_):
		pointer(pointer_),
		size(static_cast<size_type>(size_))
	{
	}

	T * pointer = nullptr;
	size_type size = 0_bi;
};


template<typename T>
constexpr auto make_storage(auto const size) {
	return dynamic_array_data(
		std::allocator<T>{}.allocate(static_cast<std::size_t>(size)),
		size
	);
}

template<typename T>
constexpr auto deallocate_storage(dynamic_array_data<T> const data) {
	std::allocator<T>{}.deallocate(
		data.pointer,
		static_cast<std::size_t>(data.size)
	);
}



template<typename T>
constexpr auto cleanup(dynamic_array_data<T> const data) {
	::containers::detail::destroy_range(data.pointer, data.pointer + data.size);
	deallocate_storage(data);
}



template<typename T>
constexpr auto dynamic_array_initializer(range auto && init) {
	auto const data = make_storage<T>(::containers::detail::linear_size(init));
	try {
		containers::uninitialized_copy(OPERATORS_FORWARD(init), data.pointer);
	} catch(...) {
		deallocate_storage(data);
		throw;
	}
	return data;
}


}	// namespace detail

template<typename T>
struct dynamic_array;

template<typename T>
constexpr void clear(dynamic_array<T> & value) {
	value = {};
}

template<typename T, range Range> requires(!std::is_array_v<Range>)
constexpr auto assign(dynamic_array<T> & container, Range && range) {
	auto const difference = detail::linear_size(range);
	if (difference == containers::size(container)) {
		::containers::copy(containers::begin(OPERATORS_FORWARD(range)), containers::end(OPERATORS_FORWARD(range)), containers::begin(container));
	} else {
		containers::clear(container);
		container = dynamic_array<T>(OPERATORS_FORWARD(range));
	}
}

template<typename T>
struct dynamic_array : private lexicographical_comparison::base {
	using value_type = T;
	using size_type = typename detail::dynamic_array_data<value_type>::size_type;

	using const_iterator = contiguous_iterator<value_type const, bounded::detail::builtin_max_value<size_type>>;
	using iterator = contiguous_iterator<value_type, bounded::detail::builtin_max_value<size_type>>;
	
	constexpr dynamic_array() = default;

	template<range Range> requires(!std::is_array_v<std::remove_cv_t<std::remove_reference_t<Range>>>)
	constexpr explicit dynamic_array(Range && range):
		m_data(::containers::detail::dynamic_array_initializer<value_type>(OPERATORS_FORWARD(range)))
	{
	}

	template<std::size_t init_size>
	constexpr dynamic_array(c_array<value_type, init_size> && init):
		dynamic_array(range_view(std::move(init)))
	{
	}

	constexpr dynamic_array(dynamic_array const & other):
		dynamic_array(range_view(other))
	{
	}
	
	constexpr dynamic_array(dynamic_array && other) noexcept:
		m_data(std::exchange(other.m_data, {}))
	{
	}
	
	constexpr ~dynamic_array() {
		detail::cleanup(m_data);
	}

	constexpr auto & operator=(dynamic_array const & other) & {
		containers::assign(*this, other);
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		detail::cleanup(m_data);
		m_data = other.m_data;
		other.m_data = {};
		return *this;
	}
	
	constexpr auto begin() const & {
		return const_iterator(m_data.pointer);
	}
	constexpr auto begin() & {
		return iterator(m_data.pointer);
	}
	constexpr auto begin() && {
		return ::containers::move_iterator(begin());
	}

	constexpr auto size() const {
		return m_data.size;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
	
private:
	detail::dynamic_array_data<value_type> m_data = {};
};

} // namespace containers
