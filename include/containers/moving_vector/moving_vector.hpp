// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/moving_vector/forward_declaration.hpp>
#include <containers/moving_vector/moving_vector_iterator.hpp>
#include <containers/vector/vector.hpp>

#include <value_ptr/value_ptr.hpp>

#include <iterator>
#include <type_traits>

namespace containers {

template<typename T, typename Allocator>
class moving_vector {
public:
	using value_type = T;
private:
	using element_type = smart_pointer::value_ptr<value_type>;
	using container_type = vector<element_type, Allocator>;
public:
	using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<element_type>;
	using size_type = typename container_type::size_type;
	
	using moving_iterator = detail::basic_array_iterator<element_type, moving_vector>;
	
	using const_iterator = detail::moving_vector_iterator<value_type const, moving_vector>;
	using iterator = detail::moving_vector_iterator<value_type, moving_vector>;

	constexpr moving_vector() = default;

	constexpr explicit moving_vector(allocator_type const &) {}
	constexpr explicit moving_vector(size_type count, allocator_type const & = allocator_type{}) {
		for (auto const n : bounded::integer_range(count)) {
			static_cast<void>(n);
			emplace_back();
		}
	}
	constexpr moving_vector(size_type count, value_type const & value, allocator_type const & = allocator_type{}) {
		assign(*this, count, value);
	}
	template<typename InputIterator>
	constexpr moving_vector(InputIterator first, InputIterator last, allocator_type const & = allocator_type{}) {
		assign(*this, first, last);
	}

	constexpr moving_vector(std::initializer_list<value_type> init, allocator_type const & allocator = allocator_type{}):
		moving_vector(std::begin(init), std::end(init), allocator) {
	}
	
	moving_vector(moving_vector const & other) = default;
	constexpr moving_vector(moving_vector const & other, allocator_type const &):
		moving_vector(other) {
	}                                                                       

	moving_vector(moving_vector && other) = default;
	constexpr moving_vector(moving_vector && other, allocator_type const &):
		moving_vector(std::move(other)) {
	}
	
	auto operator=(moving_vector const &) & -> moving_vector & = default;
	constexpr auto operator=(moving_vector &&) & noexcept -> moving_vector & = default;

	constexpr auto begin() const noexcept {
		return const_iterator(moving_begin());
	}
	constexpr auto begin() noexcept {
		return iterator(moving_begin());
	}
	constexpr auto moving_begin() noexcept {
		return moving_iterator(m_container.data());
	}
	
	constexpr auto end() const noexcept {
		return const_iterator(moving_end());
	}
	constexpr auto end() noexcept {
		return iterator(moving_end());
	}
	constexpr auto moving_end() noexcept {
		return moving_begin() + size(m_container);
	}
	
	constexpr auto && operator[](index_type<moving_vector> const index) const noexcept {
		return *(begin() + index);
	}
	constexpr auto && operator[](index_type<moving_vector> const index) noexcept {
		return *(begin() + index);
	}
	
	
	constexpr auto capacity() const noexcept {
		return m_container.capacity();
	}
	constexpr auto reserve(size_type const requested_capacity) {
		return m_container.reserve(requested_capacity);
	}
	constexpr auto shink_to_fit() {
		m_container.shrink_to_fit();
	}
	

	template<typename... Args>
	constexpr auto emplace_back(Args && ... args) {
		m_container.emplace_back(smart_pointer::make_value<value_type>(std::forward<Args>(args)...));
	}
	
	template<typename... Args>
	constexpr auto emplace(const_iterator const position, Args && ... args) {
		return make_regular_iterator(m_container.emplace(make_base_iterator(position), smart_pointer::make_value<value_type>(std::forward<Args>(args)...)));
	}
	
	template<typename ForwardIterator, typename Sentinel>
	constexpr auto insert(const_iterator position, ForwardIterator first, Sentinel const last) {
		return make_regular_iterator(m_container.insert(make_base_iterator(position), first, last));
	}

	constexpr auto pop_back() {
		m_container.pop_back();
	}
	

	using const_moving_iterator = detail::basic_array_iterator<element_type const, moving_vector>;
private:
	// moving_begin and moving_end are private because it would allow
	// modifying value_type on a moving_vector const.
	constexpr auto moving_begin() const noexcept {
		return const_moving_iterator(m_container.data());
	}
	constexpr auto moving_end() const noexcept {
		return moving_begin() + size(m_container);
	}

	constexpr auto make_base_iterator(const_iterator const it) const {
		return m_container.begin() + (it - begin());
	}
	constexpr auto make_regular_iterator(typename container_type::const_iterator const it) {
		return begin() + (it - m_container.begin());
	}

	container_type m_container;
};

// For regular containers, the iterator you need to move elements around is just
// a regular iterator
template<typename Container>
constexpr auto moving_begin(Container && container) {
	return std::forward<Container>(container).begin();
}
template<typename T, typename Allocator>
constexpr auto moving_begin(moving_vector<T, Allocator> & container) {
	return container.moving_begin();
}
template<typename T, typename Allocator>
constexpr auto moving_begin(moving_vector<T, Allocator> const & container) {
	return container.moving_begin();
}
template<typename T, typename Allocator>
constexpr auto moving_begin(moving_vector<T, Allocator> && container) {
	return std::move(container).begin();
}
template<typename Container>
constexpr auto moving_end(Container && container) {
	return std::forward<Container>(container).end();
}
template<typename T, typename Allocator>
constexpr auto moving_end(moving_vector<T, Allocator> & container) {
	return container.moving_end();
}
template<typename T, typename Allocator>
constexpr auto moving_end(moving_vector<T, Allocator> const & container) {
	return container.moving_end();
}
template<typename T, typename Allocator>
constexpr auto moving_end(moving_vector<T, Allocator> && container) {
	return std::move(container).moving_end();
}

template<typename Iterator>
constexpr auto moving_to_standard_iterator(Iterator it) {
	return it;
}
template<typename T, typename Allocator>
constexpr auto moving_to_standard_iterator(detail::basic_array_iterator<smart_pointer::value_ptr<T>, moving_vector<T, Allocator>> it) {
	return static_cast<typename moving_vector<T, Allocator>::iterator>(it);
}

namespace detail {

template<typename T, typename Allocator, typename Iterator, BOUNDED_REQUIRES(
	std::is_same<Iterator, typename moving_vector<T, Allocator>::const_iterator>::value or
	std::is_same<Iterator, typename moving_vector<T, Allocator>::iterator>::value
)>
constexpr auto make_moving_iterator(moving_vector<T, Allocator> & container, Iterator const it) noexcept {
	return container.moving_begin() + (it - container.begin());
}
template<typename T, typename Allocator, typename Iterator, BOUNDED_REQUIRES(
	std::is_same<Iterator, typename moving_vector<T, Allocator>::moving_iterator>::value
)>
constexpr auto make_moving_iterator(moving_vector<T, Allocator> &, Iterator const it) noexcept {
	return it;
}

}	// namespace detail
}	// namespace containers
