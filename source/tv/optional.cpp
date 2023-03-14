// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <cstdint>

#include <bounded/assert.hpp>

#include <operators/forward.hpp>
#include <operators/arrow.hpp>

#include "../test_assert.hpp"

export module tv.optional;
 
import tv.insert;
import tv.variant;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace tv {

// none_t cannot be default constructible or we get an ambiguity in op = {};
export struct none_t {
	constexpr explicit none_t(int) {}
};
export constexpr auto none = none_t(0);

template<typename T>
struct optional_storage {
	constexpr optional_storage():
		m_data(none_index, none)
	{
	}
	
	constexpr explicit optional_storage(bounded::lazy_init_t, bounded::construct_function_for<T> auto && construct_):
		m_data(bounded::lazy_init, value_index, OPERATORS_FORWARD(construct_))
	{
	}
	
	constexpr explicit optional_storage(bounded::explicitly_convertible_to<T> auto && value):
		m_data(value_index, OPERATORS_FORWARD(value))
	{
	}
	
	constexpr auto is_initialized() const -> bool {
		return m_data.index() == value_index;
	}

	constexpr auto uninitialize() -> void {
		::tv::insert(m_data, none_index, none);
	}

	constexpr auto initialize(bounded::construct_function_for<T> auto && construct_) -> T & {
		return m_data.emplace(value_index, OPERATORS_FORWARD(construct_));
	}
	
	constexpr auto get() const & -> T const & {
		return m_data[value_index];
	}
	constexpr auto get() & -> T & {
		return m_data[value_index];
	}
	constexpr auto get() && -> T && {
		return std::move(m_data)[value_index];
	}

private:
	static constexpr auto none_index = 0_bi;
	static constexpr auto value_index = 1_bi;
	variant<none_t, T> m_data;
};

template<typename T>
concept nullable = bounded::tombstone_traits<T>::spare_representations != 0_bi;

template<nullable T>
struct optional_storage<T> {
	constexpr optional_storage():
		m_data(make_uninitialized())
	{
	}
	
	constexpr explicit optional_storage(bounded::lazy_init_t, bounded::construct_function_for<T> auto && construct_):
		m_data(bounded::lazy_init, OPERATORS_FORWARD(construct_)())
	{
	}
	
	constexpr explicit optional_storage(bounded::explicitly_convertible_to<T> auto && value):
		m_data(OPERATORS_FORWARD(value))
	{
	}
	
	constexpr auto is_initialized() const -> bool {
		return bounded::tombstone_traits<T>::index(m_data) != uninitialized_index;
	}

	constexpr auto uninitialize() -> void {
		bounded::destroy(m_data);
		bounded::construct_at(m_data, make_uninitialized);
	}

	constexpr auto initialize(bounded::construct_function_for<T> auto && construct_) -> T & {
		uninitialize();
		return bounded::construct_at(m_data, OPERATORS_FORWARD(construct_));
	}

	constexpr auto get() const & -> T const & {
		return m_data;
	}
	constexpr auto get() & -> T & {
		return m_data;
	}
	constexpr auto get() && -> T && {
		return std::move(m_data);
	}

private:
	static constexpr auto uninitialized_index = 0_bi;
	static constexpr auto make_uninitialized() noexcept -> T {
		static_assert(noexcept(bounded::tombstone_traits<T>::make(uninitialized_index)));
		return bounded::tombstone_traits<T>::make(uninitialized_index);
	}
	T m_data;
};

template<typename T>
struct optional_storage<T &> {
	constexpr optional_storage():
		m_data(nullptr)
	{
	}
	
	constexpr explicit optional_storage(bounded::lazy_init_t, bounded::construct_function_for<T &> auto && construct_):
		m_data(std::addressof(OPERATORS_FORWARD(construct_)()))
	{
	}
	
	constexpr explicit optional_storage(bounded::explicitly_convertible_to<T &> auto && value):
		m_data(std::addressof(OPERATORS_FORWARD(value)))
	{
	}
	
	constexpr auto is_initialized() const -> bool {
		return static_cast<bool>(m_data);
	}

	constexpr auto uninitialize() -> void {
		m_data = nullptr;
	}

	constexpr auto initialize(bounded::construct_function_for<T &> auto && construct_) -> T & {
		m_data = std::addressof(OPERATORS_FORWARD(construct_)());
		return *m_data;
	}

	constexpr auto get() const & -> T & {
		return *m_data;
	}

private:
	T * m_data;
};

constexpr auto assign(auto & target, auto && source) -> auto & {
	if (target) {
		*target = OPERATORS_FORWARD(source);
	} else {
		::tv::insert(target, OPERATORS_FORWARD(source));
	}
	return target;
}

constexpr auto assign_from_optional(auto & target, auto && source) -> auto & {
	if (!source) {
		target = none;
	} else {
		assign(target, *OPERATORS_FORWARD(source));
	}
	return target;
}

template<typename T>
concept non_reference = !std::is_reference_v<T>;

template<typename T>
concept optional_equality_comparable = bounded::equality_comparable<T> and non_reference<T>;

export template<typename T>
struct optional {
	using value_type = T;

	constexpr optional(none_t = none) {
	}

	constexpr explicit optional(bounded::lazy_init_t, bounded::construct_function_for<T> auto && construct_):
		m_storage(bounded::lazy_init, OPERATORS_FORWARD(construct_)) {
	}

	template<bounded::explicitly_convertible_to<T> Value = T>
	constexpr explicit(!bounded::convertible_to<Value, T>) optional(Value && other):
		m_storage(OPERATORS_FORWARD(other))
	{
	}

	optional(optional const &) = default;
	optional(optional &&) = default;
	auto operator=(optional const &) & -> optional & requires non_reference<T> = default;
	auto operator=(optional &&) & -> optional & requires non_reference<T> = default;
	
	constexpr auto operator*() const & -> T const & {
		BOUNDED_ASSERT(*this);
		return m_storage.get();
	}
	constexpr auto operator*() & -> T & {
		BOUNDED_ASSERT(*this);
		return m_storage.get();
	}
	constexpr auto operator*() && -> T && {
		BOUNDED_ASSERT(*this);
		return std::move(m_storage).get();
	}

	OPERATORS_ARROW_DEFINITIONS

	constexpr explicit operator bool() const {
		return m_storage.is_initialized();
	}

	constexpr auto emplace(bounded::construct_function_for<T> auto && construct_) -> T & {
		return m_storage.initialize(OPERATORS_FORWARD(construct_));
	}

	constexpr auto operator=(none_t) & -> optional & {
		m_storage.uninitialize();
		return *this;
	}
	constexpr auto operator=(bounded::convertible_to<T> auto && other) & -> optional & requires non_reference<T> {
		return ::tv::assign(*this, OPERATORS_FORWARD(other));
	}
		
	friend constexpr auto operator==(optional const & lhs, optional const & rhs) -> bool requires optional_equality_comparable<T> {
		return (lhs and rhs) ?
			*lhs == *rhs :
			static_cast<bool>(lhs) == static_cast<bool>(rhs);
	}
	friend constexpr auto operator==(optional const & lhs, T const & rhs) -> bool requires optional_equality_comparable<T> {
		return static_cast<bool>(lhs) and *lhs == rhs;
	}
	friend constexpr auto operator==(optional const & lhs, none_t) -> bool {
		return !lhs;
	}

private:
	
	optional_storage<T> m_storage;
};

template<typename T> requires(!std::same_as<T, none_t>)
optional(T) -> optional<T>;

export constexpr auto make_optional(auto && value) {
	return optional<std::decay_t<decltype(value)>>(OPERATORS_FORWARD(value));
}

export constexpr auto make_optional_lazy(auto && function) -> optional<std::invoke_result_t<decltype(function)>> {
	return {OPERATORS_FORWARD(function)()};
}

} // namespace tv

template<typename LHS, typename RHS>
struct std::common_type<tv::optional<LHS>, RHS> {
	using type = tv::optional<common_type_t<LHS, RHS>>;
};
template<typename LHS, typename RHS>
struct std::common_type<LHS, tv::optional<RHS>> {
	using type = tv::optional<common_type_t<LHS, RHS>>;
};
template<typename LHS, typename RHS>
struct std::common_type<tv::optional<LHS>, tv::optional<RHS>> {
	using type = tv::optional<common_type_t<LHS, RHS>>;
};


template<typename RHS>
struct std::common_type<tv::none_t, RHS> {
	using type = tv::optional<RHS>;
};
template<typename LHS>
struct std::common_type<LHS, tv::none_t> {
	using type = tv::optional<LHS>;
};
template<>
struct std::common_type<tv::none_t, tv::none_t> {
	using type = tv::none_t;
};


template<typename RHS>
struct std::common_type<tv::none_t, tv::optional<RHS>> {
	using type = tv::optional<RHS>;
};
template<typename LHS>
struct std::common_type<tv::optional<LHS>, tv::none_t> {
	using type = tv::optional<LHS>;
};

static_assert(
	std::same_as<std::common_type_t<tv::none_t, int>, tv::optional<int>>,
	"common_type with none_t first wrong."
);
static_assert(
	std::same_as<std::common_type_t<int, tv::none_t>, tv::optional<int>>,
	"common_type with none_t last wrong."
);
static_assert(
	std::same_as<
		std::common_type_t<bounded::constant_t<1>, tv::none_t, bounded::constant_t<5>, tv::none_t>,
		tv::optional<bounded::integer<1, 5>>
	>,
	"common_type with bounded::integer and none_t wrong."
);

template<typename T>
constexpr auto test_optional_traits() {
	static_assert(bounded::convertible_to<T, tv::optional<T>>);
	static_assert(std::is_trivially_destructible_v<T> == std::is_trivially_destructible_v<tv::optional<T>>);
	static_assert(std::is_trivially_copy_constructible_v<T> == std::is_trivially_copy_constructible_v<T>);
	static_assert(std::is_trivially_copy_assignable_v<T> == std::is_trivially_copy_assignable_v<T>);
	return true;
}

namespace {

struct S {
	int member;
};

} // namespace

static_assert(test_optional_traits<int>());
static_assert(test_optional_traits<bounded::integer<0, 10>>());
static_assert(test_optional_traits<decltype(bounded::integer(0))>());
constexpr auto test_insert() {
	auto op = tv::optional<S>();
	tv::insert(op, S{3});
	return op and op->member == 3;
}
static_assert(test_insert());

static_assert(*tv::optional(5) == 5);

using integer = bounded::integer<0, 10>;
using optional = tv::optional<integer>;

constexpr auto uninitialized = optional();

static_assert(uninitialized == tv::none, "Fail to compare uninitialized optional to none_t.");
static_assert(uninitialized == optional(tv::none), "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(uninitialized != optional(0_bi), "Fail to compare uninitialized optional to a value.");
static_assert(tv::none == uninitialized, "Fail to compare uninitialized optional to none_t.");
static_assert(optional(tv::none) == uninitialized, "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(optional(0_bi) != uninitialized, "Fail to compare uninitialized optional to a value.");

constexpr auto initialized = optional(5_bi);

static_assert(initialized != tv::none, "Fail to compare initialized optional to none_t.");
static_assert(initialized != optional(tv::none), "Fail to compare initialized optional to uninitialized optional.");
static_assert(initialized == optional(5_bi), "Fail to compare initialized optional to a value.");
static_assert(initialized != optional(4_bi), "Fail to compare initialized optional to a value.");

static_assert(tv::none != initialized, "Fail to compare initialized optional to none_t.");
static_assert(optional(5_bi) == initialized, "Fail to compare initialized optional to a value.");
static_assert(optional(1_bi) != initialized, "Fail to compare initialized optional to a value.");

template<typename Optional>
constexpr auto check_empty_braces() {
	Optional op = {};
	BOUNDED_TEST(!op);
	op = {};
	BOUNDED_TEST(!op);
	return true;
}

template<typename T>
constexpr auto check_uncompressed_optional() {
	using type = bounded::integer<bounded::builtin_min_value<T>, bounded::builtin_max_value<T>>;
	static_assert(
		sizeof(type) < sizeof(tv::optional<type>),
		"Compressing an optional that should not be compressed."
	);
	check_empty_braces<tv::optional<type>>();
}
template<auto minimum, auto maximum>
constexpr auto check_compressed_optional() {
	using type = bounded::integer<minimum, maximum>;
	static_assert(
		sizeof(type) == sizeof(tv::optional<type>),
		"compressed_optional too big."
	);
	check_empty_braces<tv::optional<type>>();
}

template<typename integer_type>
constexpr auto check_integer_optional() {
	constexpr auto uninitialized_optional = tv::optional<integer_type>();
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	constexpr auto constexpr_optional_integer = tv::optional<integer_type>(integer_type(5_bi));
	static_assert(constexpr_optional_integer, "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");

	auto optional_integer = tv::optional<integer_type>(integer_type(4_bi));
	optional_integer = uninitialized_optional;
	BOUNDED_TEST(!optional_integer);

	optional_integer = integer_type(7_bi);
	BOUNDED_TEST(optional_integer);

	static_assert(std::same_as<decltype(*optional_integer), integer_type &>, "Incorrect type of *optional.");
	*optional_integer = integer_type(1_bi);
	BOUNDED_TEST(optional_integer);
	BOUNDED_TEST(*optional_integer == 1_bi);
	
	optional_integer = tv::none;
	BOUNDED_TEST(!optional_integer);

	check_empty_braces<tv::optional<integer_type>>();
}

template<typename type>
constexpr auto check_non_trivial_optional() {
	auto uninitialized_optional = tv::optional<type>();
	BOUNDED_TEST(!uninitialized_optional);
	auto optional_int = tv::optional<type>(1);
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 1);

	optional_int = uninitialized_optional;
	BOUNDED_TEST(!optional_int);

	optional_int = type(2);
	BOUNDED_TEST(optional_int);
	optional_int = 3;
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 3);
	BOUNDED_TEST(*optional_int != 4);

	static_assert(std::same_as<decltype(*optional_int), type &>, "Incorrect type of *optional.");
	*optional_int = type(5);
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 5);
	
	optional_int = tv::none;
	BOUNDED_TEST(!optional_int);

	check_empty_braces<tv::optional<type>>();
}

constexpr auto check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<std::uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<numeric_traits::max_signed_t>();

	check_integer_optional<int>();
	check_integer_optional<bounded::integer<1, 10>>();
	check_non_trivial_optional<bounded_test::integer>();

	constexpr auto original = tv::make_optional(0_bi);
	constexpr auto copy = original;
	static_assert(original == copy);

	return true;
}

static_assert(check_optional());

static_assert(!tv::optional<int const &>());
static_assert(!tv::optional<int &>());

static_assert(bounded::constructible_from<tv::optional<int const &>, int const &>);
static_assert(bounded::constructible_from<tv::optional<int const &>, int &&>);
static_assert(bounded::constructible_from<tv::optional<int const &>, int &>);
static_assert(!bounded::constructible_from<tv::optional<int &>, int const &>);
static_assert(!bounded::constructible_from<tv::optional<int &>, int &&>);
static_assert(bounded::constructible_from<tv::optional<int &>, int &>);
static_assert(!std::is_assignable_v<tv::optional<int &> &, int &>);

static_assert([] {
	auto const value = 5;
	auto optional_ref = tv::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = tv::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = tv::optional<int &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value1 = 5;
	auto value2 = 5;
	auto optional_ref = tv::optional<int &>(value1);
	optional_ref.emplace(bounded::value_to_function(value2));
	return std::addressof(*optional_ref) == std::addressof(value2);
}());

static_assert(tv::optional<tv::optional<int>>() == tv::none);
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>()) != tv::none);
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>()) == tv::optional<int>());
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>(3)) == tv::optional<int>(3));

