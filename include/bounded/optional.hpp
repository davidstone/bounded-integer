// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/variant/variant.hpp>
#include <bounded/assert.hpp>
#include <operators/forward.hpp>
#include <bounded/concepts.hpp>
#include <bounded/tombstone_traits.hpp>
#include <bounded/value_to_function.hpp>

#include <operators/arrow.hpp>

#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {

// none_t cannot be default constructible or we get an ambiguity in op = {};
struct none_t {
	constexpr explicit none_t(int) {}
};
inline constexpr auto none = none_t(0);

namespace detail {

template<typename T>
struct optional_storage {
	constexpr optional_storage():
		m_data(none_index, none)
	{
	}
	
	constexpr explicit optional_storage(lazy_init_t, construct_function_for<T> auto && construct_):
		m_data(lazy_init, value_index, OPERATORS_FORWARD(construct_))
	{
	}
	
	constexpr explicit optional_storage(explicitly_convertible_to<T> auto && value):
		m_data(value_index, OPERATORS_FORWARD(value))
	{
	}
	
	constexpr auto is_initialized() const {
		return m_data.index() == value_index;
	}

	constexpr auto uninitialize() {
		::bounded::insert(m_data, none_index, none);
	}

	constexpr auto initialize(construct_function_for<T> auto && construct_) {
		m_data.emplace(value_index, OPERATORS_FORWARD(construct_));
	}
	
	constexpr auto && get() const & {
		return m_data[value_index];
	}
	constexpr auto && get() & {
		return m_data[value_index];
	}
	constexpr auto && get() && {
		return std::move(m_data)[value_index];
	}

private:
	static constexpr auto none_index = constant<0>;
	static constexpr auto value_index = constant<1>;
	variant<none_t, T> m_data;
};

template<typename T>
concept nullable = tombstone_traits<T>::spare_representations != constant<0>;

template<nullable T>
struct optional_storage<T> {
	constexpr optional_storage():
		m_data(make_uninitialized())
	{
	}
	
	constexpr explicit optional_storage(lazy_init_t, construct_function_for<T> auto && construct_):
		m_data(lazy_init, OPERATORS_FORWARD(construct_)())
	{
	}
	
	constexpr explicit optional_storage(explicitly_convertible_to<T> auto && value):
		m_data(OPERATORS_FORWARD(value))
	{
	}
	
	constexpr auto is_initialized() const {
		return tombstone_traits<T>::index(m_data) != uninitialized_index;
	}

	constexpr auto uninitialize() {
		destroy(m_data);
		construct_at(m_data, make_uninitialized);
	}

	constexpr auto initialize(construct_function_for<T> auto && construct_) {
		uninitialize();
		construct_at(m_data, OPERATORS_FORWARD(construct_));
	}

	constexpr auto && get() const & {
		return m_data;
	}
	constexpr auto && get() & {
		return m_data;
	}
	constexpr auto && get() && {
		return std::move(m_data);
	}

private:
	static constexpr auto uninitialized_index = constant<0>;
	static constexpr auto make_uninitialized() noexcept {
		static_assert(noexcept(tombstone_traits<T>::make(uninitialized_index)));
		return tombstone_traits<T>::make(uninitialized_index);
	}
	T m_data;
};

template<typename T>
struct optional_storage<T &> {
	constexpr optional_storage():
		m_data(nullptr)
	{
	}
	
	constexpr explicit optional_storage(lazy_init_t, construct_function_for<T &> auto && construct_):
		m_data(std::addressof(OPERATORS_FORWARD(construct_)()))
	{
	}
	
	constexpr explicit optional_storage(explicitly_convertible_to<T &> auto && value):
		m_data(std::addressof(OPERATORS_FORWARD(value)))
	{
	}
	
	constexpr auto is_initialized() const {
		return static_cast<bool>(m_data);
	}

	constexpr auto uninitialize() {
		m_data = nullptr;
	}

	constexpr auto initialize(construct_function_for<T &> auto && construct_) {
		m_data = std::addressof(OPERATORS_FORWARD(construct_)());
	}

	constexpr auto get() const & -> auto && {
		return *m_data;
	}

private:
	T * m_data;
};

constexpr auto & assign(auto & target, auto && source) {
	if (target) {
		*target = OPERATORS_FORWARD(source);
	} else {
		::bounded::insert(target, OPERATORS_FORWARD(source));
	}
	return target;
}

constexpr auto & assign_from_optional(auto & target, auto && source) {
	if (!source) {
		target = none;
	} else {
		assign(target, *OPERATORS_FORWARD(source));
	}
	return target;
}

} // namespace detail

template<typename T>
struct optional {
private:
	struct common_init_tag{};

public:
	using value_type = T;

	constexpr optional(none_t = none) {
	}

	constexpr explicit optional(lazy_init_t, construct_function_for<T> auto && construct_):
		m_storage(lazy_init, OPERATORS_FORWARD(construct_)) {
	}

	template<explicitly_convertible_to<value_type> Value>
	constexpr explicit(!bounded::convertible_to<Value, value_type>) optional(Value && other):
		m_storage(OPERATORS_FORWARD(other))
	{
	}

	template<typename U> requires explicitly_convertible_to<U const &, value_type>
	constexpr explicit optional(optional<U> const & other):
		optional(other, common_init_tag{})
	{
	}
	template<typename U> requires explicitly_convertible_to<U &&, value_type>
	constexpr explicit optional(optional<U> && other):
		optional(std::move(other), common_init_tag{})
	{
	}

	optional(optional const &) = default;
	optional(optional &&) = default;
	auto operator=(optional const &) & -> optional & requires(!std::is_reference_v<T>) = default;
	auto operator=(optional &&) & -> optional & requires(!std::is_reference_v<T>) = default;
	
	constexpr auto operator*() const & -> value_type const & {
		BOUNDED_ASSERT(*this);
		return m_storage.get();
	}
	constexpr auto operator*() & -> value_type & {
		BOUNDED_ASSERT(*this);
		return m_storage.get();
	}
	constexpr auto operator*() && -> value_type && {
		BOUNDED_ASSERT(*this);
		return std::move(m_storage).get();
	}

	OPERATORS_ARROW_DEFINITIONS

	constexpr explicit operator bool() const {
		return m_storage.is_initialized();
	}

	constexpr auto emplace(construct_function_for<T> auto && construct_) {
		m_storage.initialize(OPERATORS_FORWARD(construct_));
	}

	constexpr auto operator=(none_t) & -> optional & {
		m_storage.uninitialize();
		return *this;
	}
	constexpr auto operator=(bounded::convertible_to<value_type> auto && other) & -> optional & requires(!std::is_reference_v<T>) {
		return detail::assign(*this, OPERATORS_FORWARD(other));
	}
		
	friend constexpr auto operator==(optional const & lhs, optional const & rhs) -> bool requires(equality_comparable<T> and !std::is_reference_v<T>) {
		return (lhs and rhs) ?
			*lhs == *rhs :
			static_cast<bool>(lhs) == static_cast<bool>(rhs);
	}
	friend constexpr auto operator==(optional const & lhs, T const & rhs) -> bool requires(equality_comparable<T> and !std::is_reference_v<T>) {
		return static_cast<bool>(lhs) and *lhs == rhs;
	}
	friend constexpr auto operator==(optional const & lhs, none_t) -> bool {
		return !lhs;
	}

private:
	constexpr optional(auto && other_optional, common_init_tag):
		optional(none) {
		if (other_optional) {
			::bounded::insert(*this, *OPERATORS_FORWARD(other_optional));
		}
	}
	
	detail::optional_storage<T> m_storage;
};

// A limitation of emulating lazy parameters is that we cannot tell if the user
// is trying to create an optional function or lazily create an optional value
template<typename T> requires(!std::same_as<T, none_t> and !std::invocable<T>)
optional(T) -> optional<T>;

constexpr auto make_optional(auto && value) {
	return optional<std::decay_t<decltype(value)>>(OPERATORS_FORWARD(value));
}

constexpr auto make_optional_lazy(auto && function) -> optional<std::invoke_result_t<decltype(function)>> {
	return {OPERATORS_FORWARD(function)()};
}

} // namespace bounded

template<typename LHS, typename RHS>
struct std::common_type<bounded::optional<LHS>, RHS> {
	using type = bounded::optional<common_type_t<LHS, RHS>>;
};
template<typename LHS, typename RHS>
struct std::common_type<LHS, bounded::optional<RHS>> {
	using type = bounded::optional<common_type_t<LHS, RHS>>;
};
template<typename LHS, typename RHS>
struct std::common_type<bounded::optional<LHS>, bounded::optional<RHS>> {
	using type = bounded::optional<common_type_t<LHS, RHS>>;
};


template<typename RHS>
struct std::common_type<bounded::none_t, RHS> {
	using type = bounded::optional<RHS>;
};
template<typename LHS>
struct std::common_type<LHS, bounded::none_t> {
	using type = bounded::optional<LHS>;
};
template<>
struct std::common_type<bounded::none_t, bounded::none_t> {
	using type = bounded::none_t;
};


template<typename RHS>
struct std::common_type<bounded::none_t, bounded::optional<RHS>> {
	using type = bounded::optional<RHS>;
};
template<typename LHS>
struct std::common_type<bounded::optional<LHS>, bounded::none_t> {
	using type = bounded::optional<LHS>;
};
