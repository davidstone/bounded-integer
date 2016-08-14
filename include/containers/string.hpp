// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/equal.hpp>
#include <containers/algorithms/lexicographical_compare.hpp>
#include <containers/small_buffer_optimized_vector.hpp>

namespace containers {

// Unlike std::basic_string, there is no null terminator.
template<typename T, typename Allocator = allocator<T>>
struct basic_string : private small_buffer_optimized_vector<T, 1, Allocator> {
private:
	using base = small_buffer_optimized_vector<T, 1, Allocator>;
public:
	using typename base::value_type;
	using typename base::allocator_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;
	
	using base::get_allocator;

	using base::base;
	constexpr basic_string(T const * const c_string, allocator_type allocator = allocator_type()) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(c_string, c_string + std::strlen(c_string), std::move(allocator))
	) {
	}
	
	// Taking this as a template parameter allows us to grab the array size as
	// part of the type system if we are passing in a string literal or some
	// other array that has not yet decayed. This prevents some cases of
	// undefined behavior where we index past the end of the provided array.
	template<typename CString, typename Size, BOUNDED_REQUIRES(
		(std::is_same<std::decay_t<CString>, T const *>::value or std::is_same<std::decay_t<CString>, T *>::value) and
		std::numeric_limits<Size>::is_integer
	)>
	constexpr basic_string(CString const & str, Size const size, allocator_type allocator = allocator_type()) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(str, str + size, std::move(allocator))
	) {
	}
	
	using base::operator=;
	
	friend constexpr auto begin(basic_string const & container) noexcept {
		return begin(static_cast<base const &>(container));
	}
	friend constexpr auto begin(basic_string & container) noexcept {
		return begin(static_cast<base &>(container));
	}
	friend constexpr auto end(basic_string const & container) noexcept {
		return end(static_cast<base const &>(container));
	}
	friend constexpr auto end(basic_string & container) noexcept {
		return end(static_cast<base &>(container));
	}

	using base::operator[];
	
	using base::capacity;
	using base::reserve;
	using base::shrink_to_fit;
	
	using base::emplace_back;
	using base::emplace;
	using base::insert;
	using base::pop_back;
};



// TODO: Write a c_string end sentinel for a one-pass algorithm

template<typename T, typename Allocator>
constexpr auto operator==(basic_string<T, Allocator> const & lhs, T const * const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::equal(begin(lhs), end(lhs), rhs, rhs + std::strlen(rhs))
)
template<typename T, typename Allocator>
constexpr auto operator==(T const * const lhs, basic_string<T, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	rhs == lhs
)


template<typename T, typename Allocator>
constexpr auto operator<(basic_string<T, Allocator> const & lhs, T const * const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare(begin(lhs), end(lhs), rhs, rhs + std::strlen(rhs))
)
template<typename T, typename Allocator>
constexpr auto operator<(T const * const lhs, basic_string<T, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare(lhs, lhs + std::strlen(lhs), begin(rhs), end(rhs))
)



// TODO: Write a concatenate function that accepts an arbitrary number of ranges
template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> const & lhs, basic_string<T, Allocator> const & rhs) {
	basic_string<T, Allocator> result;
	result.reserve(size(lhs) + size(rhs));
	append(result, begin(lhs), end(lhs));
	append(result, begin(rhs), end(rhs));
	return result;
}
template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> && lhs, basic_string<T, Allocator> const & rhs) {
	append(lhs, begin(rhs), end(rhs));
	return std::move(lhs);
}
template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> const & lhs, basic_string<T, Allocator> && rhs) {
	rhs.insert(begin(rhs), begin(lhs), end(lhs));
	return std::move(rhs);
}
template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> && lhs, basic_string<T, Allocator> && rhs) {
	return (size(lhs) + size(rhs) <= lhs.capacity()) ?
		std::move(lhs) + detail::add_const(rhs) :
		detail::add_const(lhs) + std::move(rhs);
}



template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> const & lhs, T const * const rhs) {
	auto const rhs_size = std::strlen(rhs);
	basic_string<T, Allocator> result;
	result.reserve(size(lhs) + rhs_size);
	append(result, begin(lhs), end(lhs));
	append(result, rhs, rhs + rhs_size);
	return result;
}
template<typename T, typename Allocator>
auto operator+(basic_string<T, Allocator> && lhs, T const * const rhs) {
	append(lhs, rhs, rhs + std::strlen(rhs));
	return std::move(lhs);
}
template<typename T, typename Allocator>
auto operator+(T const * const lhs, basic_string<T, Allocator> const & rhs) {
	auto const lhs_size = std::strlen(lhs);
	basic_string<T, Allocator> result;
	result.reserve(lhs_size + size(lhs));
	append(result, lhs, lhs + lhs_size);
	append(result, begin(rhs), end(rhs));
	return result;
}
template<typename T, typename Allocator>
auto operator+(T const * const lhs, basic_string<T, Allocator> && rhs) {
	rhs.insert(begin(rhs), lhs, lhs + std::strlen(rhs));
	return std::move(rhs);
}



using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;


// TODO: Add in a null-terminated string class

}	// namespace containers
