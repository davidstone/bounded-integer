// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/concatenate.hpp>
#include <containers/range_view.hpp>
#include <containers/small_buffer_optimized_vector.hpp>

namespace containers {

// Unlike std::basic_string, there is no null terminator.
template<typename CharT, typename Allocator = allocator<CharT>>
struct basic_string : private small_buffer_optimized_vector<CharT, 1, Allocator> {
private:
	using base = small_buffer_optimized_vector<CharT, 1, Allocator>;
public:
	using typename base::value_type;
	using typename base::allocator_type;
	using typename base::size_type;
	using typename base::const_iterator;
	using typename base::iterator;
	
	using base::get_allocator;

	using base::base;
	
	constexpr basic_string(std::basic_string_view<CharT> const sv, allocator_type allocator_ = allocator_type()) BOUNDED_NOEXCEPT_INITIALIZATION(
		base(begin(sv), end(sv), std::move(allocator_))
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
	
	constexpr operator std::basic_string_view<CharT>() const noexcept {
		return {data(*this), size(*this)};
	}
};

template<typename CharT, typename Allocator>
constexpr auto is_container<basic_string<CharT, Allocator>> = true;


namespace detail {

template<typename CharT>
struct c_string_sentinel_t {
};

template<typename CharT>
constexpr auto compare(CharT const * first, c_string_sentinel_t<CharT>) noexcept {
	return *first == '\0' ? bounded::strong_ordering_equal : bounded::strong_ordering_less;
}

template<typename CharT>
constexpr auto c_string_sentinel = c_string_sentinel_t<CharT>{};

} // namespace detail

template<typename CharT, typename Allocator>
constexpr auto compare(basic_string<CharT, Allocator> const & lhs, CharT const * const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), rhs, detail::c_string_sentinel<CharT>)
)
template<typename CharT, typename Allocator>
constexpr auto compare(CharT const * const lhs, basic_string<CharT, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(lhs, detail::c_string_sentinel<CharT>, begin(rhs), end(rhs))
)

template<typename CharT, typename Allocator>
constexpr auto compare(basic_string<CharT, Allocator> const & lhs, std::basic_string_view<CharT> const rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), begin(rhs), end(rhs))
)
template<typename CharT, typename Allocator>
constexpr auto compare(std::basic_string_view<CharT> const lhs, basic_string<CharT, Allocator> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	::containers::compare(begin(lhs), end(lhs), begin(rhs), end(rhs))
)


template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> const & lhs, basic_string<CharT, Allocator> const & rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}
template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> && lhs, basic_string<CharT, Allocator> const & rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}
template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> const & lhs, basic_string<CharT, Allocator> && rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}
template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> && lhs, basic_string<CharT, Allocator> && rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}


template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> const & lhs, std::basic_string_view<CharT> const rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}
template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> && lhs, std::basic_string_view<CharT> const rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(std::move(lhs), rhs);
}
template<typename CharT, typename Allocator>
auto operator+(std::basic_string_view<CharT> const lhs, basic_string<CharT, Allocator> const & rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, rhs);
}
template<typename CharT, typename Allocator>
auto operator+(std::basic_string_view<CharT> const lhs, basic_string<CharT, Allocator> && rhs) {
	return ::containers::concatenate<basic_string<CharT, Allocator>>(lhs, std::move(rhs));
}


template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> const & lhs, CharT const * const rhs) {
	return lhs + std::basic_string_view(rhs);
}
template<typename CharT, typename Allocator>
auto operator+(basic_string<CharT, Allocator> && lhs, CharT const * const rhs) {
	return std::move(lhs) + std::basic_string_view(rhs);
}
template<typename CharT, typename Allocator>
auto operator+(CharT const * const lhs, basic_string<CharT, Allocator> const & rhs) {
	return std::basic_string_view(lhs) + rhs;
}
template<typename CharT, typename Allocator>
auto operator+(CharT const * const lhs, basic_string<CharT, Allocator> && rhs) {
	return std::basic_string_view(lhs) + std::move(rhs);
}


using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;


// TODO: Add in a null-terminated string class

}	// namespace containers
