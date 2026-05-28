// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.remove_none;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.dereference;
import containers.forward_range;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.range;
import containers.range_reference_t;
import containers.range_value_t;
import containers.sentinel_t;

import bounded;
import std_module;
import tv;

namespace containers {

template<typename T>
struct wrapper {
	wrapper() = default;
	constexpr auto get(this auto && self) -> auto && {
		return OPERATORS_FORWARD(self).m_value;
	}
	constexpr auto replace(auto make) {
		bounded::construct_at(m_value, make);
	}
private:
	[[no_unique_address]] T m_value;
};

template<typename T> requires(!std::is_trivially_destructible_v<T>)
struct wrapper<T> {
	wrapper() = default;
	constexpr auto get(this auto && self) -> auto && {
		return *OPERATORS_FORWARD(self).m_storage;
	}
	constexpr auto replace(auto make) {
		m_storage.emplace(make);
	}
private:
	[[no_unique_address]] tv::optional<T> m_storage{bounded::lazy_init, bounded::construct<T>};
};

template<typename T> requires(std::is_reference_v<T> and !std::is_pointer_v<std::remove_reference_t<T>>)
struct wrapper<T> {
	wrapper() = default;
	constexpr auto get(this wrapper const self) -> T {
		return static_cast<T>(*self.m_ptr);
	}
	constexpr auto replace(auto make) {
		auto && ref = make();
		m_ptr = std::addressof(ref);
	}

private:
	std::remove_reference_t<T> * m_ptr;
};

template<typename T> requires(std::is_pointer_v<std::remove_reference_t<T>>)
struct wrapper<T> {
	using value_type = std::remove_cvref_t<T>;
	wrapper() = default;
	constexpr auto get(this wrapper const self) -> value_type {
		return self.m_ptr;
	}
	constexpr auto replace(auto make) {
		m_ptr = make();
	}

private:
	value_type m_ptr;
};

template<typename Range>
concept use_inplace = bounded::mostly_trivial<wrapper<containers::range_reference_t<Range>>>;

template<typename Range>
struct remove_none_data {
	[[no_unique_address]] Range range;
	[[no_unique_address]] wrapper<range_reference_t<Range>> cached_value;
};

template<typename Range>
struct iterator_reference_data {
	constexpr iterator_reference_data(iterator_t<Range> it, remove_none_data<Range> & data):
		m_it(std::move(it)),
		m_data(data)
	{
	}

	iterator_reference_data(iterator_reference_data &&) = default;
	iterator_reference_data(iterator_reference_data const &) = delete;
	auto operator=(iterator_reference_data &&) & -> iterator_reference_data & = default;
	auto operator=(iterator_reference_data const &) & -> iterator_reference_data & = delete;

	constexpr auto iterator() const -> iterator_t<Range> const & {
		return m_it;
	}
	constexpr auto iterator() -> iterator_t<Range> & {
		return m_it;
	}
	constexpr auto sentinel() const -> sentinel_t<Range> {
		return containers::end(m_data.get().range);
	}

	constexpr auto cached() const -> wrapper<range_reference_t<Range>> & {
		return m_data.get().cached_value;
	}
private:
	[[no_unique_address]] iterator_t<Range> m_it;
	std::reference_wrapper<remove_none_data<Range>> m_data;
};

template<typename Range>
struct iterator_inplace_data {
	constexpr iterator_inplace_data(iterator_t<Range> it, sentinel_t<Range> last):
		m_it(std::move(it)),
		m_last(std::move(last))
	{
	}

	constexpr auto iterator() const -> iterator_t<Range> const & {
		return m_it;
	}
	constexpr auto iterator() -> iterator_t<Range> & {
		return m_it;
	}
	constexpr auto sentinel() const -> sentinel_t<Range> {
		return m_last;
	}

	constexpr auto cached(this auto && self) -> auto & {
		return self.m_cached_value;
	}
private:
	[[no_unique_address]] iterator_t<Range> m_it;
	[[no_unique_address]] sentinel_t<Range> m_last;
	[[no_unique_address]] wrapper<range_reference_t<Range>> m_cached_value;
};

template<typename T>
constexpr auto is_rvalue = std::is_rvalue_reference_v<T> or !std::is_reference_v<T>;

template<typename Range>
struct remove_none_iterator {
	using difference_type = iter_difference_t<iterator_t<Range>>;

	constexpr explicit remove_none_iterator(auto && ... args):
		m_data(OPERATORS_FORWARD(args)...)
	{
		advance_to_first_value();
	}

	constexpr auto operator*() const -> auto && {
		if constexpr (is_rvalue<range_reference_t<Range>>) {
			return std::move(*m_data.cached().get());
		} else {
			return *m_data.cached().get();
		}
	}

	friend constexpr auto operator+(remove_none_iterator lhs, bounded::constant_t<1>) -> remove_none_iterator {
		++lhs.m_data.iterator();
		lhs.advance_to_first_value();
		return lhs;
	}

	friend constexpr auto operator==(remove_none_iterator const & lhs, remove_none_iterator const & rhs) -> bool requires bounded::equality_comparable<iterator_t<Range>> {
		return lhs.m_data.iterator() == rhs.m_data.iterator();
	}
	friend constexpr auto operator==(remove_none_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_data.iterator() == lhs.m_data.sentinel();
	}

private:
	constexpr auto advance_to_first_value() -> void {
		auto const last = m_data.sentinel();
		auto & it = m_data.iterator();
		while (it != last) {
			auto & storage = m_data.cached();
			storage.replace([&] -> decltype(auto) { return dereference<Range>(it); });
			if (storage.get()) {
				break;
			}
			++it;
		}
	}

	using data_t = std::conditional_t<
		use_inplace<Range>,
		iterator_inplace_data<Range>,
		iterator_reference_data<Range>
	>;
	[[no_unique_address]] data_t m_data;
};

template<typename Range>
concept optional_range = range<Range> and requires(range_reference_t<Range> value) {
	*value;
	value ? void() : void();
};

// This goes from range<optional<T>> to range<T>. Requires that the value_type's
// default constructor puts the optional into the empty state. If an exception
// is thrown by dereferencing iterators in the underlying range and the
// underlying range returns a non-pointer optional type by value, then this
// object can no longer be iterated over.
export template<optional_range Range>
struct remove_none {
	constexpr explicit remove_none(Range && input):
		m_data(OPERATORS_FORWARD(input))
	{
	}
	constexpr auto begin() && -> remove_none_iterator<Range> {
		if constexpr (use_inplace<Range>) {
			return remove_none_iterator<Range>(
				containers::begin(OPERATORS_FORWARD(m_data)),
				containers::end(OPERATORS_FORWARD(m_data))
			);
		} else {
			return remove_none_iterator<Range>(
				containers::begin(std::move(m_data).range),
				m_data
			);
		}
	}
	constexpr auto begin() const & -> remove_none_iterator<Range const> requires use_inplace<Range> and forward_range<Range> {
		return remove_none_iterator<Range const>(
			containers::begin(m_data),
			containers::end(m_data)
		);
	}
	constexpr auto begin() & -> remove_none_iterator<Range> requires use_inplace<Range> and forward_range<Range> {
		return remove_none_iterator<Range>(
			containers::begin(m_data),
			containers::end(m_data)
		);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}

private:
	using data_t = std::conditional_t<
		use_inplace<Range>,
		Range,
		remove_none_data<Range>
	>;
	[[no_unique_address]] data_t m_data;
};

template<typename Range>
remove_none(Range &&) -> remove_none<Range>;

} // namespace containers

static_assert([]{
	constexpr auto source = containers::array{1, 2, tv::none, 3, tv::none};
	constexpr auto expected = containers::array{1, 2, 3};
	static_assert(containers::forward_range<decltype(containers::remove_none(source))>);
	return containers::equal(containers::remove_none(source), expected);
}());

static_assert([]{
	static constexpr int a = 0;
	static constexpr int b = 1;
	static constexpr int c = 3;
	constexpr auto source = containers::array{&a, &b, nullptr, nullptr, &c, nullptr};
	constexpr auto expected = containers::array{0, 1, 3};
	static_assert(containers::forward_range<decltype(containers::remove_none(source))>);
	return containers::equal(containers::remove_none(source), expected);
}());

struct by_value_iterator {
	using difference_type = int;
	constexpr explicit by_value_iterator(int const value):
		m_value(value)
	{
	}
	constexpr auto operator*() const -> tv::optional<int> {
		return m_value;
	}
	constexpr auto operator++() -> by_value_iterator & {
		++m_value;
		return *this;
	}
	friend auto operator==(by_value_iterator, by_value_iterator) -> bool = default;
private:
	int m_value;
};
struct by_value_range {
	static constexpr auto begin() -> by_value_iterator {
		return by_value_iterator(1);
	}
	static constexpr auto end() -> by_value_iterator {
		return by_value_iterator(5);
	}
};

static_assert(containers::equal(containers::remove_none(by_value_range()), by_value_range()));
constexpr auto r = containers::remove_none(by_value_range());
static_assert(containers::equal(r, by_value_range()));
static_assert(containers::forward_range<decltype(containers::remove_none(by_value_range()))>);