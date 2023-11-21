// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.visit;

import tv.is_valid_index;
import tv.overload;
import tv.visit;

import bounded;
import std_module;

namespace {

using namespace bounded::literal;

template<int max>
using integer_t = bounded::integer<0, max>;

template<int max>
struct visitable_index {
	explicit constexpr visitable_index(integer_t<max> integer_):
		m_integer(integer_)
	{
	}
	constexpr auto integer() const -> integer_t<max> {
		return m_integer;
	}
private:
	integer_t<max> m_integer;
};

template<int size>
struct variant {
private:
	using index_t = visitable_index<size - 1>;
public:
	explicit constexpr variant(integer_t<size - 1> const index_):
		m_index(index_)
	{
	}
	constexpr auto index() const -> index_t {
		return m_index;
	}
	constexpr auto operator[](auto const constant) const -> auto const & {
		if (constant != index().integer()) {
			throw std::runtime_error("Invalid index");
		}
		static constexpr auto result = constant;
		return result;
	}
private:
	index_t m_index;
};

template<typename Function, typename... Variants>
concept visitable = requires {
	tv::visit(bounded::declval<Variants>()..., bounded::declval<Function>());
};

struct not_a_function {
};
static_assert(!visitable<not_a_function>);
static_assert(!visitable<not_a_function, variant<1>>);
static_assert(!visitable<not_a_function, variant<2>>);
static_assert(!visitable<not_a_function, variant<1>, variant<2>>);

struct nullary {
	static auto operator()() -> void {
	}
};
static_assert(visitable<nullary>);
static_assert(!visitable<nullary, variant<1>>);
static_assert(!visitable<nullary, variant<2>>);
static_assert(!visitable<nullary, variant<1>, variant<2>>);

struct accepts_0 {
	static auto operator()(bounded::constant_t<0>) -> void {
	}
};
static_assert(!visitable<accepts_0>);
static_assert(visitable<accepts_0, variant<1>>);
static_assert(!visitable<accepts_0, variant<2>>);
static_assert(!visitable<accepts_0, variant<1>, variant<2>>);

struct accepts_1 {
	static auto operator()(bounded::constant_t<1>) -> void {
	}
};
static_assert(!visitable<accepts_1>);
static_assert(!visitable<accepts_1, variant<1>>);
static_assert(!visitable<accepts_1, variant<2>>);
static_assert(!visitable<accepts_1, variant<1>, variant<2>>);

struct accepts_0_or_1 {
	static auto operator()(bounded::constant_t<0>) -> void {
	}
	static auto operator()(bounded::constant_t<1>) -> void {
	}
};
static_assert(!visitable<accepts_0_or_1>);
static_assert(visitable<accepts_0_or_1, variant<1>>);
static_assert(visitable<accepts_0_or_1, variant<2>>);
static_assert(!visitable<accepts_0_or_1, variant<1>, variant<2>>);

struct accepts_0_and_0_or_1 {
	static auto operator()(bounded::constant_t<0>, bounded::constant_t<0>) -> void {
	}
	static auto operator()(bounded::constant_t<0>, bounded::constant_t<1>) -> void {
	}
};
static_assert(!visitable<accepts_0_and_0_or_1>);
static_assert(!visitable<accepts_0_and_0_or_1, variant<1>>);
static_assert(!visitable<accepts_0_and_0_or_1, variant<2>>);
static_assert(visitable<accepts_0_and_0_or_1, variant<1>, variant<2>>);

struct accepts_0_and_0_through_1 {
	static auto operator()(bounded::constant_t<0>, bounded::integer<0, 1>) -> void {
	}
};
static_assert(!visitable<accepts_0_and_0_through_1>);
static_assert(!visitable<accepts_0_and_0_through_1, variant<1>>);
static_assert(!visitable<accepts_0_and_0_through_1, variant<2>>);
static_assert(visitable<accepts_0_and_0_through_1, variant<1>, variant<2>>);

struct accepts_0_or_0_and_0_through_1 {
	static auto operator()(bounded::constant_t<0>) -> void {
	}
	static auto operator()(bounded::constant_t<0>, bounded::integer<0, 1>) -> void {
	}
};
static_assert(!visitable<accepts_0_or_0_and_0_through_1>);
static_assert(visitable<accepts_0_or_0_and_0_through_1, variant<1>>);
static_assert(!visitable<accepts_0_or_0_and_0_through_1, variant<2>>);
static_assert(visitable<accepts_0_or_0_and_0_through_1, variant<1>, variant<2>>);

struct accepts_any {
	static auto operator()(auto...) -> void {
	}
};
static_assert(visitable<accepts_any>);
static_assert(visitable<accepts_any, variant<1>>);
static_assert(visitable<accepts_any, variant<2>>);
static_assert(visitable<accepts_any, variant<1>, variant<2>>);

static_assert(tv::visit(
	variant<1>(0_bi),
	[](bounded::constant_t<0>) { return true; }
));

static_assert(tv::visit(
	variant<2>(0_bi),
	tv::overload(
		[](bounded::constant_t<0>) { return true; },
		[](bounded::constant_t<1>) { return false; }
	)
));

template<int max>
constexpr auto function = [](bounded::integer<0, max - 1> x) {
	return x + 1_bi;
};
static_assert(tv::visit(variant<16>(15_bi), function<16>) == 16_bi);
static_assert(tv::visit(variant<17>(16_bi), function<17>) == 17_bi);
static_assert(tv::visit(variant<32>(31_bi), function<32>) == 32_bi);
static_assert(tv::visit(variant<33>(32_bi), function<33>) == 33_bi);


struct int_or_unsigned {
private:
	using index_t = visitable_index<1>;
public:
	explicit constexpr int_or_unsigned(int const x):
		m_index(int_index),
		m_int(x)
	{
	}
	explicit constexpr int_or_unsigned(unsigned const x):
		m_index(long_index),
		m_unsigned(x)
	{
	}
	constexpr auto index() const -> index_t {
		return m_index;
	}
	constexpr auto operator[](auto const constant) const & -> auto const & {
		if constexpr (constant == int_index) {
			return m_int;
		} else {
			return m_unsigned;
		}
	}
	constexpr auto operator[](auto const constant) & -> auto & {
		if constexpr (constant == int_index) {
			return m_int;
		} else {
			return m_unsigned;
		}
	}
	constexpr auto operator[](auto const constant) && -> auto && {
		if constexpr (constant == int_index) {
			return std::move(m_int);
		} else {
			return std::move(m_unsigned);
		}
	}
private:
	static constexpr auto int_index = 0_bi;
	static constexpr auto long_index = 1_bi;
	index_t m_index;
	union {
		int m_int;
		unsigned m_unsigned;
	};
};
static_assert([]{
	auto const v = int_or_unsigned(3);
	return tv::visit(v, tv::overload(
		[](int const & x) { return x == 3; },
		[](auto &&) { return false; }
	));
}());
static_assert([]{
	auto v = int_or_unsigned(3);
	return tv::visit(v, tv::overload(
		[](int & x) { return x == 3; },
		[](auto &&) { return false; }
	));
}());
static_assert([]{
	auto v = int_or_unsigned(3);
	return tv::visit(std::move(v), tv::overload(
		[](int && x) { return x == 3; },
		[](auto &&) { return false; }
	));
}());
static_assert([]{
	auto const v = int_or_unsigned(3U);
	return tv::visit(v, tv::overload(
		[](unsigned const & x) { return x == 3U; },
		[](auto &&) { return false; }
	));
}());
static_assert([]{
	auto v = int_or_unsigned(3U);
	return tv::visit(v, tv::overload(
		[](unsigned & x) { return x == 3U; },
		[](auto &&) { return false; }
	));
}());
static_assert([]{
	auto v = int_or_unsigned(3U);
	return tv::visit(std::move(v), tv::overload(
		[](unsigned && x) { return x == 3U; },
		[](auto &&) { return false; }
	));
}());

struct qualified_function {
	constexpr auto operator()(auto) const & {
		return 0;
	}
	constexpr auto operator()(auto) & {
		return 1;
	}
	constexpr auto operator()(auto) && {
		return 2;
	}
};
static_assert([]{
	auto const function = qualified_function();
	return tv::visit(int_or_unsigned(3U), function) == 0;
}());
static_assert([]{
	auto function = qualified_function();
	return tv::visit(int_or_unsigned(3U), function) == 1;
}());
static_assert([]{
	auto function = qualified_function();
	return tv::visit(int_or_unsigned(3U), std::move(function)) == 2;
}());


struct nonmovable {
	constexpr nonmovable() = default;
	nonmovable(nonmovable &&) = delete;
	nonmovable(nonmovable const &) = delete;

	static constexpr auto index() -> visitable_index<0> {
		return visitable_index<0>(0_bi);
	}
	constexpr auto operator[](bounded::constant_t<0>) const -> nonmovable const & {
		return *this;
	}
};
static_assert([]{
	auto const v = nonmovable();
	return tv::visit(v, [&](nonmovable const & u) {
		return std::addressof(u) == std::addressof(v);
	});
}());
static_assert([]{
	auto v = nonmovable();
	return tv::visit(std::move(v), [&](nonmovable const & u) {
		return std::addressof(u) == std::addressof(v);
	});
}());

} // namespace