// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>
#include <operators/arrow.hpp>

export module tv.optional;
 
import tv.insert;
import tv.none;
import tv.variant;

import bounded;
import std_module;

using namespace bounded::literal;

namespace tv {

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

	using tombstone_t = bounded::tombstone_traits<variant<none_t, T>>;
	constexpr explicit optional_storage(bounded::tombstone_tag, auto const index) noexcept:
		m_data(tombstone_t::make(index))
	{
	}
	constexpr auto tombstone_index() const noexcept {
		return tombstone_t::index(m_data);
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
		if (is_initialized()) {
			bounded::destroy(m_data);
		}
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
	
	using underlying_tombstone = bounded::tombstone_traits<T>;
	constexpr explicit optional_storage(bounded::tombstone_tag, auto const index) noexcept:
	m_data(underlying_tombstone::make(index + 1_bi))
	{
	}
	using tombstone_index_t = bounded::integer<
		-1,
		bounded::normalize<underlying_tombstone::spare_representations - 2_bi>
	>;
	constexpr auto tombstone_index() const noexcept -> tombstone_index_t {
		auto const underlying_index = underlying_tombstone::index(m_data);
		return bounded::assume_in_range<tombstone_index_t>(
			underlying_index == -1_bi ?
				-1_bi :
				underlying_index - 1_bi,
		);
	}

private:
	static constexpr auto uninitialized_index = 0_bi;
	static constexpr auto make_uninitialized() noexcept -> T {
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

	// TODO: look into using the unused bits of the pointer
	static constexpr auto tombstone_index() noexcept -> bounded::constant_t<-1> {
		return -1_bi;
	}

private:
	T * m_data;
};

} // namespace tv

template<typename T>
struct bounded::tombstone<tv::optional_storage<T>>  {
	static constexpr auto make(auto const index) noexcept -> tv::optional_storage<T> {
		return tv::optional_storage<T>(tombstone_tag(), index);
	}
	static constexpr auto index(tv::optional_storage<T> const & value) noexcept {
		return value.tombstone_index();
	}
};

namespace tv {

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
	constexpr explicit optional(bounded::tombstone_tag, auto const make):
		m_storage(make())
	{
	}
	
	optional_storage<T> m_storage;
	friend bounded::tombstone<optional<T>>;
	friend bounded::tombstone_member<&optional<T>::m_storage>;
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

template<typename T>
struct bounded::tombstone<tv::optional<T>> : bounded::tombstone_member<&tv::optional<T>::m_storage> {
};

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
