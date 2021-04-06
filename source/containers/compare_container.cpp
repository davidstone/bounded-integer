// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/compare_container.hpp>

#include <containers/vector.hpp>

namespace {

constexpr auto max_size = 3;

struct equal_only : containers::range_equality::base {
	using value_type = int;

	template<typename... Args>
	constexpr explicit equal_only(Args... init):
		m_data{init...},
		m_size(sizeof...(Args))
	{
		static_assert(sizeof...(Args) <= max_size);
	}
	constexpr auto begin() const {
		return static_cast<int const *>(m_data);
	}
	constexpr auto size() const {
		return m_size;
	}

private:
	int m_data[max_size];
	int m_size;
};

static_assert(equal_only() == equal_only());
static_assert(equal_only() != equal_only(1));

static_assert(equal_only(1) == equal_only(1));
static_assert(equal_only(1) != equal_only(2));
static_assert(equal_only(1) != equal_only(1, 2));
static_assert(equal_only(1) != equal_only(2, 1));
static_assert(equal_only(1) != equal_only(1, 1));

static_assert(equal_only(1, 2) == equal_only(1, 2));
static_assert(equal_only(1, 2) != equal_only(2, 2));
static_assert(equal_only(1, 2) != equal_only(2, 1));
static_assert(equal_only(1, 2) != equal_only(1, 2, 2));
static_assert(equal_only(1, 2) != equal_only(3, 1, 2));


struct lexicographical : containers::lexicographical_comparison::base {
	using value_type = int;

	template<typename... Args>
	constexpr explicit lexicographical(Args... init):
		m_data{init...},
		m_size(sizeof...(Args))
	{
		static_assert(sizeof...(Args) <= max_size);
	}
	constexpr auto begin() const {
		return static_cast<int const *>(m_data);
	}
	constexpr auto size() const {
		return m_size;
	}

private:
	int m_data[max_size];
	int m_size;
};

static_assert(lexicographical() == lexicographical());
static_assert(lexicographical() <= lexicographical());
static_assert(lexicographical() >= lexicographical());

static_assert(lexicographical() != lexicographical(1));
static_assert(lexicographical() < lexicographical(1));


static_assert(lexicographical(1) == lexicographical(1));
static_assert(lexicographical(1) <= lexicographical(1));
static_assert(lexicographical(1) >= lexicographical(1));

static_assert(lexicographical(1) != lexicographical(2));
static_assert(lexicographical(1) < lexicographical(2));

static_assert(lexicographical(1) != lexicographical(0));
static_assert(lexicographical(1) > lexicographical(0));

static_assert(lexicographical(1) != lexicographical(1, 2));
static_assert(lexicographical(1) < lexicographical(1, 2));

static_assert(lexicographical(1) != lexicographical(2, 1));
static_assert(lexicographical(1) < lexicographical(2, 1));

static_assert(lexicographical(1) != lexicographical(1, 1));
static_assert(lexicographical(1) < lexicographical(1, 1));

static_assert(lexicographical(1) != lexicographical(0, 1));
static_assert(lexicographical(1) > lexicographical(0, 1));


static_assert(lexicographical(1, 2) == lexicographical(1, 2));
static_assert(lexicographical(1, 2) <= lexicographical(1, 2));
static_assert(lexicographical(1, 2) >= lexicographical(1, 2));

static_assert(lexicographical(1, 2) != lexicographical(2, 2));
static_assert(lexicographical(1, 2) < lexicographical(2, 2));

static_assert(lexicographical(1, 2) != lexicographical(2, 1));
static_assert(lexicographical(1, 2) < lexicographical(2, 1));

static_assert(lexicographical(1, 2) != lexicographical(0, 1));
static_assert(lexicographical(1, 2) > lexicographical(0, 1));

static_assert(lexicographical(1, 2) != lexicographical(1, 2, 2));
static_assert(lexicographical(1, 2) < lexicographical(1, 2, 2));

static_assert(lexicographical(1, 2) != lexicographical(0, 1, 2));
static_assert(lexicographical(1, 2) > lexicographical(0, 1, 2));

static_assert(lexicographical(1, 2) != lexicographical(3, 1, 2));
static_assert(lexicographical(1, 2) < lexicographical(3, 1, 2));


struct shortlex : containers::shortlex_comparison::base {
	using value_type = int;

	template<typename... Args>
	constexpr explicit shortlex(Args... init):
		m_data{init...},
		m_size(sizeof...(Args))
	{
		static_assert(sizeof...(Args) <= max_size);
	}
	constexpr auto begin() const {
		return static_cast<int const *>(m_data);
	}
	constexpr auto size() const {
		return m_size;
	}

private:
	int m_data[max_size];
	int m_size;
};

static_assert(shortlex() == shortlex());
static_assert(shortlex() <= shortlex());
static_assert(shortlex() >= shortlex());

static_assert(shortlex() != shortlex(1));
static_assert(shortlex() < shortlex(1));


static_assert(shortlex(1) == shortlex(1));
static_assert(shortlex(1) <= shortlex(1));
static_assert(shortlex(1) >= shortlex(1));

static_assert(shortlex(1) != shortlex(2));
static_assert(shortlex(1) < shortlex(2));

static_assert(shortlex(1) != shortlex(0));
static_assert(shortlex(1) > shortlex(0));

static_assert(shortlex(1) != shortlex(1, 2));
static_assert(shortlex(1) < shortlex(1, 2));

static_assert(shortlex(1) != shortlex(2, 1));
static_assert(shortlex(1) < shortlex(2, 1));

static_assert(shortlex(1) != shortlex(1, 1));
static_assert(shortlex(1) < shortlex(1, 1));

static_assert(shortlex(1) != shortlex(0, 1));
static_assert(shortlex(1) < shortlex(0, 1));


static_assert(shortlex(1, 2) == shortlex(1, 2));
static_assert(shortlex(1, 2) <= shortlex(1, 2));
static_assert(shortlex(1, 2) >= shortlex(1, 2));

static_assert(shortlex(1, 2) != shortlex(2, 2));
static_assert(shortlex(1, 2) < shortlex(2, 2));

static_assert(shortlex(1, 2) != shortlex(2, 1));
static_assert(shortlex(1, 2) < shortlex(2, 1));

static_assert(shortlex(1, 2) != shortlex(0, 1));
static_assert(shortlex(1, 2) > shortlex(0, 1));

static_assert(shortlex(1, 2) != shortlex(1, 2, 2));
static_assert(shortlex(1, 2) < shortlex(1, 2, 2));

static_assert(shortlex(1, 2) != shortlex(0, 1, 2));
static_assert(shortlex(1, 2) < shortlex(0, 1, 2));

static_assert(shortlex(1, 2) != shortlex(3, 1, 2));
static_assert(shortlex(1, 2) < shortlex(3, 1, 2));


} // namespace
