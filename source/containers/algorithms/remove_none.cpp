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
import containers.iter_difference_t;
import containers.iterator_t;
import containers.range;
import containers.range_reference_t;

import bounded;
import tv;
import std_module;

namespace containers {

template<typename Range>
struct remove_none_data {
	[[no_unique_address]] Range range;
	tv::optional<range_reference_t<Range>> cached_value;
};

template<typename Range>
struct remove_none_iterator {
	using difference_type = iter_difference_t<iterator_t<Range>>;

	constexpr explicit remove_none_iterator(iterator_t<Range> it, remove_none_data<Range> & data):
		m_it(std::move(it)),
		m_data(data)
	{
		advance_to_first_value();
	}

	remove_none_iterator(remove_none_iterator &&) = default;
	remove_none_iterator(remove_none_iterator const &) = delete;
	auto operator=(remove_none_iterator &&) & -> remove_none_iterator & = default;
	auto operator=(remove_none_iterator const &) & -> remove_none_iterator & = delete;

	constexpr auto operator*() const -> auto && {
		return **std::move(m_data.get().cached_value);
	}
	friend constexpr auto operator+(remove_none_iterator lhs, bounded::constant_t<1>) -> remove_none_iterator {
		++lhs.m_it;
		lhs.advance_to_first_value();
		return lhs;
	}

	friend constexpr auto operator==(remove_none_iterator const & lhs, remove_none_iterator const & rhs) -> bool requires bounded::equality_comparable<iterator_t<Range>> {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator==(remove_none_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_it == containers::end(lhs.m_data.get().range);
	}

private:
	constexpr auto advance_to_first_value() -> void {
		auto const last = containers::end(m_data.get().range);
		while (m_it != last) {
			auto & storage = m_data.get().cached_value;
			storage.emplace([&] -> decltype(auto) { return dereference<Range>(m_it); });
			if (*storage) {
				break;
			}
			++m_it;
		}
	}

	[[no_unique_address]] iterator_t<Range> m_it;
	std::reference_wrapper<remove_none_data<Range>> m_data;
};

// This goes from range<optional<T>> to range<T>
export template<range Range>
struct remove_none {
	constexpr explicit remove_none(Range && input):
		m_data(OPERATORS_FORWARD(input), tv::none)
	{
	}
	constexpr auto begin() {
		return remove_none_iterator<Range>(containers::begin(m_data.range), m_data);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}

private:
	remove_none_data<Range> m_data;
};

template<typename Range>
remove_none(Range &&) -> remove_none<Range>;

} // namespace containers

static_assert([]{
	constexpr auto source = containers::array{1, 2, tv::none, 3, tv::none};
	constexpr auto expected = containers::array{1, 2, 3};
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