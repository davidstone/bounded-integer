// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/unique.hpp>

#include <containers/append.hpp>
#include <containers/erase.hpp>
#include <containers/repeat_n.hpp>
#include <containers/vector.hpp>

#include "../../test_assert.hpp"

namespace {

class checked_mover {
public:
	constexpr checked_mover(int value):
		m_value(value),
		m_moved(false)
	{
	}
		
	constexpr checked_mover(checked_mover const & other):
		m_value(other.m_value),
		m_moved(other.m_moved)
	{
		BOUNDED_TEST(!other.m_moved);
		BOUNDED_TEST(!other.m_destructed);
	}
	constexpr checked_mover(checked_mover && other) noexcept:
		m_value(other.m_value),
		m_moved(other.m_moved)
	{
		BOUNDED_TEST(this != &other);
		BOUNDED_TEST(!other.m_moved);
		BOUNDED_TEST(!other.m_destructed);
		other.m_moved = true;
	}
	constexpr checked_mover & operator=(checked_mover const & other) {
		BOUNDED_TEST(!other.m_moved);
		BOUNDED_TEST(!other.m_destructed);
		BOUNDED_TEST(!m_destructed);
		m_value = other.m_value;
		m_moved = other.m_moved;
		return *this;
	}
	constexpr checked_mover & operator=(checked_mover && other) noexcept {
		BOUNDED_TEST(this != &other);
		BOUNDED_TEST(!other.m_moved);
		BOUNDED_TEST(!other.m_destructed);
		BOUNDED_TEST(!m_destructed);
		m_value = other.m_value;
		m_moved = other.m_moved;
		other.m_moved = true;
		return *this;
	}
	constexpr ~checked_mover() {
		BOUNDED_TEST(!m_destructed);
		m_destructed = true;
	}
	
	
	friend constexpr auto operator<=>(checked_mover const & lhs, checked_mover const & rhs) {
		BOUNDED_TEST(!lhs.m_moved);
		BOUNDED_TEST(!rhs.m_moved);
		BOUNDED_TEST(!lhs.m_destructed);
		BOUNDED_TEST(!rhs.m_destructed);
		return lhs.m_value <=> rhs.m_value;
	}
	friend constexpr auto operator==(checked_mover const & lhs, checked_mover const & rhs) -> bool {
		BOUNDED_TEST(!lhs.m_moved);
		BOUNDED_TEST(!rhs.m_moved);
		BOUNDED_TEST(!lhs.m_destructed);
		BOUNDED_TEST(!rhs.m_destructed);
		return lhs.m_value== rhs.m_value;
	}
private:
	int m_value;
	bool m_moved;
	bool m_destructed = false;
};

template<typename Container>
constexpr void test_unique_copy_less(Container const & source, Container const & expected) {
	auto destination = Container(containers::repeat_n(containers::size(source), 0));
	auto const it = containers::unique_copy_less(begin(source), end(source), begin(destination));
	containers::erase(destination, it, end(destination));
	BOUNDED_TEST(destination == expected);
}

template<typename Container>
constexpr void test_unique_less(Container source, Container const & expected) {
	BOUNDED_TEST(std::is_sorted(begin(source), end(source)));
	BOUNDED_TEST(std::is_sorted(begin(expected), end(expected)));
	test_unique_copy_less(source, expected);
	test_unique_copy_less(std::move(source), expected);
}

template<typename Container>
constexpr void test_unique_merge_copy(Container const & lhs, Container const & rhs, Container const & expected) {
	auto result = Container(containers::repeat_n(typename Container::size_type(containers::size(lhs) + containers::size(rhs)), 0));
	auto const it = containers::unique_merge_copy(begin(lhs), end(lhs), begin(rhs), end(rhs), begin(result));
	containers::erase(result, it, end(result));

	BOUNDED_TEST(result == expected);
}

// `Container &&` instead of `Container` to avoid
// https://bugs.llvm.org/show_bug.cgi?id=47805 and
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97388

template<typename Container>
constexpr void test_unique_inplace_merge(Container && v, Container const & other, Container const & expected) {
	using iterator = typename Container::iterator;
	auto const midpoint = static_cast<typename std::iterator_traits<iterator>::difference_type>(containers::size(v));
	containers::append(v, other);
	auto const it = containers::unique_inplace_merge(begin(v), begin(v) + midpoint, end(v));
	containers::erase(v, it, end(v));

	BOUNDED_TEST(v == expected);
}

template<typename Container>
constexpr void test_unique_merge(Container && v, Container const & other, Container const & expected) {
	BOUNDED_TEST(std::is_sorted(begin(v), end(v)));
	BOUNDED_TEST(std::is_sorted(begin(other), end(other)));
	test_unique_merge_copy(v, other, expected);
	test_unique_inplace_merge(std::move(v), other, expected);
}

template<typename Container>
constexpr bool test_unique() {
	test_unique_less<Container>({ 1 }, { 1 });
	test_unique_less<Container>({ 1, 2 }, { 1, 2 });
	test_unique_less<Container>({ }, { });
	test_unique_less<Container>({ 1, 3, 5, 5, 5, 6, 10, 10 }, { 1, 3, 5, 6, 10 });
	test_unique_less<Container>({ 1, 1, 1, 1, 1, 1, 1, 1 }, { 1 });
	test_unique_merge<Container>({ 1, 2, 3, 5 }, { 7, 8, 9 }, { 1, 2, 3, 5, 7, 8, 9 });
	test_unique_merge<Container>({ 1, 3, 5, 7, 9 }, { 2, 2, 2, 3, 3, 6, 7 }, { 1, 2, 3, 5, 6, 7, 9 });
	test_unique_merge<Container>({ 2 }, { 1 }, { 1, 2 });
	test_unique_merge<Container>({ }, { 6 }, { 6 });
	test_unique_merge<Container>({ 4 }, { }, { 4 });
	test_unique_merge<Container>({ }, { }, { });
	test_unique_merge<Container>({ 1 }, { 1 }, { 1 });
	test_unique_merge<Container>({ 8 }, { 8, 8, 8, 8, 8 }, { 8 });
	// Ideally unique_inplace_merge would not assume the first range has no
	// duplicates, but that is my current use-case. I do not know how to remove
	// this limitation without making the algorithm less efficient.
	// test_unique_merge<Container>({ 8, 8 }, { 8, 8, 8, 8, 8 }, { 8 });
	return true;
}

static_assert(test_unique<containers::vector<checked_mover>>());

} // namespace