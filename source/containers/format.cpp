// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.format;

import containers.associative_container;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.legacy_iterator;
import containers.range;
import containers.range_reference_t;
import containers.vector;

import bounded;
import std_module;

namespace containers {

template<typename Range>
struct fmt_associative {
	constexpr fmt_associative(Range const & range):
		m_range(range)
	{
	}

	constexpr auto begin() const {
		return legacy_iterator(::containers::begin(m_range));
	}
	constexpr auto end() const {
		return legacy_iterator(::containers::end(m_range));
	}

private:
	Range const & m_range;
};

template<typename Range>
struct fmt_range {
	constexpr fmt_range(Range const & range):
		m_range(range)
	{
	}

	constexpr auto begin() const {
		return legacy_iterator(::containers::begin(m_range));
	}
	constexpr auto end() const {
		return legacy_iterator(::containers::end(m_range));
	}

private:
	Range const & m_range;
};

template<typename T>
using compatible = std::conditional_t<
	associative_range<T const &>,
	fmt_associative<T>,
	fmt_range<T>
>;

template<typename T>
concept uses_bounded_integers = bounded::bounded_integer<
	containers::iter_difference_t<containers::iterator_t<T const &>>
>;

template<typename T>
concept non_std_formattable_range =
	range<T const &> and
	uses_bounded_integers<T> and
	std::formattable<containers::range_reference_t<T>, char>;

} // namespace containers

#if 0
// Disabled until https://github.com/llvm/llvm-project/issues/244974 is fixed
// We format `map_value_type`, so we do not need special map vs. set handling.
template<typename T>
constexpr auto std::format_kind<containers::fmt_associative<T>> = std::range_format::set;
#endif

template<containers::non_std_formattable_range T>
struct std::formatter<T> {
	constexpr auto parse(auto & context) {
		return m_underlying.parse(context);
	}

	constexpr auto format(T const & range, auto & context) const {
		auto const view = ::containers::compatible<T>(range);
		return m_underlying.format(view, context);
	}

private:
	std::formatter<containers::compatible<T>> m_underlying;
};
