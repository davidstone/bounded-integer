// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.join_with;

import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_range;
import containers.is_empty;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.iter_value_t;
import containers.iterator_t;
import containers.range;
import containers.range_reference_t;
import containers.range_size_t;
import containers.sentinel_t;

import bounded;
import std_module;
import tv;

namespace containers {
using namespace bounded::literal;

template<typename Range>
struct with_inner_range {
	with_inner_range() = default;
	with_inner_range(with_inner_range &&) = default;
	with_inner_range(with_inner_range const &) = delete;
	auto operator=(with_inner_range &&) & -> with_inner_range & = default;
	auto operator=(with_inner_range const &) & -> with_inner_range & = delete;

	constexpr auto set(auto const & outer_it) & -> void {
		m_range.emplace([&] { return *outer_it; });
		m_it = containers::begin(std::move(*m_range));
	}

	constexpr auto dereference() const -> decltype(auto) {
		return *m_it;
	}

	constexpr auto increment() & -> void {
		++m_it;
	}

	constexpr auto is_empty() const -> bool {
		return m_it == containers::end(*m_range);
	}

private:
	[[no_unique_address]] tv::optional<Range> m_range{};
	[[no_unique_address]] iterator_t<Range> m_it{};
};

template<typename Range> requires std::is_reference_v<Range>
struct with_inner_range<Range> {
};

template<typename Range>
struct with_inner_range_iterator {
	constexpr with_inner_range_iterator(with_inner_range<Range> & range, auto const & outer_it) {
		range.set(outer_it);
	}
	static constexpr auto dereference(with_inner_range<Range> & range) -> decltype(auto) {
		return range.dereference();
	}
	static constexpr auto increment(with_inner_range<Range> & range) -> void {
		range.increment();
	}
	static constexpr auto is_empty(with_inner_range<Range> const & range) -> bool {
		return range.is_empty();
	}
};

template<typename Range> requires std::is_reference_v<Range>
struct with_inner_range_iterator<Range> {
	template<typename RQualified>
	constexpr with_inner_range_iterator(with_inner_range<RQualified>, auto const & outer_it):
		with_inner_range_iterator(*outer_it)
	{
	}
	template<typename RQualified>
	constexpr auto dereference(with_inner_range<RQualified>) const -> decltype(auto) {
		return *m_it;
	}
	template<typename RQualified>
	constexpr auto increment(with_inner_range<RQualified>) & -> void {
		++m_it;
	}
	template<typename RQualified>
	constexpr auto is_empty(with_inner_range<RQualified>) const -> bool {
		return m_it == m_last;
	}

private:
	constexpr explicit with_inner_range_iterator(auto && value):
		m_it(containers::begin(OPERATORS_FORWARD(value))),
		m_last(containers::end(OPERATORS_FORWARD(value)))
	{
	}
	[[no_unique_address]] iterator_t<Range> m_it;
	[[no_unique_address]] sentinel_t<Range> m_last;
};

template<typename JW, typename Iterator>
struct join_with_diff_t {
	using type = std::ptrdiff_t;
};

template<typename JW, typename Iterator> requires(
	bounded::bounded_integer<iter_difference_t<Iterator>> and
	bounded::bounded_integer<range_size_t<iter_value_t<Iterator>>> and
	bounded::bounded_integer<range_size_t<typename JW::joiner_t>>
)
struct join_with_diff_t<JW, Iterator> {
	using type = decltype(
		(
			bounded::declval<iter_difference_t<Iterator>>() *
			bounded::declval<range_size_t<iter_value_t<Iterator>>>()
		) +
		bounded::max(
			0_bi,
			(bounded::declval<iter_difference_t<Iterator>>() - 1_bi) *
			bounded::declval<range_size_t<typename JW::joiner_t>>()
		)
	);
};

// TODO: It's possible to more efficiently add and subtract iterators
template<typename JW, typename Iterator, typename Sentinel>
struct join_with_iterator {
private:
	using inner_it = with_inner_range_iterator<iter_reference_t<Iterator>>;
	using joiner_it = iterator_t<typename JW::joiner_t const>;
public:
	using difference_type = typename join_with_diff_t<JW, Iterator>::type;
	using reference_type = bounded::common_type_and_value_category<
		decltype(bounded::declval<inner_it>().dereference(bounded::declval<JW &>().m_inner)),
		iter_reference_t<joiner_it>
	>;

	explicit constexpr join_with_iterator(
		JW & jw,
		Iterator it,
		Sentinel last
	):
		m_jw(jw),
		m_outer_it(std::move(it)),
		m_outer_last(std::move(last)),
		m_inner(joiner_it(containers::end(jw.m_joiner)))
	{
		normalize();
	}

	constexpr auto operator*() const -> reference_type {
		return tv::visit(m_inner, tv::overload(
			[&](inner_it const & it) -> reference_type {
				return it.dereference(m_jw.get().m_inner);
			},
			[&](joiner_it const & it) -> reference_type {
				return *it;
			}
		));
	}
	friend constexpr auto operator+(join_with_iterator lhs, bounded::constant_t<1>) -> join_with_iterator {
		tv::visit(lhs.m_inner, tv::overload(
			[&](inner_it & it) {
				it.increment(lhs.m_jw.get().m_inner);
			},
			[&](joiner_it & it) {
				++it;
			}
		));
		lhs.normalize();
		return lhs;
	}
	friend constexpr auto operator==(join_with_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_outer_it == lhs.m_outer_last;
	}

private:
	using inner_t = tv::variant<inner_it, joiner_it>;

	struct do_normalize {
		constexpr auto operator()(inner_it const & it) const -> bool {
			if (!it.is_empty(outer.m_jw.get().m_inner)) {
				return true;
			}
			++outer.m_outer_it;
			outer.m_inner.emplace([&] -> joiner_it {
				return containers::begin(outer.m_jw.get().m_joiner);
			});
			return false;
		}
		constexpr auto operator()(joiner_it const & it) const -> bool {
			if (it != containers::end(outer.m_jw.get().m_joiner)) {
				return true;
			}
			outer.m_inner.emplace([&] {
				return inner_it(outer.m_jw.get().m_inner, outer.m_outer_it);
			});
			return false;
		}
		join_with_iterator & outer;
	};
	constexpr auto normalize() & -> void {
		auto const visitor = do_normalize(*this);
		while (*this != std::default_sentinel) {
			auto const is_normalized = tv::visit(m_inner, visitor);
			if (is_normalized) {
				break;
			}
		}
	}

	[[no_unique_address]] std::reference_wrapper<JW> m_jw;
	[[no_unique_address]] Iterator m_outer_it;
	[[no_unique_address]] Sentinel m_outer_last;
	[[no_unique_address]] inner_t m_inner;
};

export template<range Range, forward_range Joiner>
struct join_with {
	explicit constexpr join_with(Range && range, Joiner && joiner):
		m_range(OPERATORS_FORWARD(range)),
		m_joiner(OPERATORS_FORWARD(joiner))
	{
	}
	constexpr auto begin() const & requires std::is_reference_v<range_reference_t<Range const &>> {
		return join_with_iterator(
			*this,
			containers::begin(m_range),
			containers::end(m_range)
		);
	}
	constexpr auto begin() & requires std::is_reference_v<range_reference_t<Range &>> {
		return join_with_iterator(
			*this,
			containers::begin(m_range),
			containers::end(m_range)
		);
	}
	constexpr auto begin() && {
		return join_with_iterator(
			*this,
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range))
		);
	}
	// TODO: Support bidirectional?
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	template<typename JW, typename Iterator, typename Sentinel>
	friend struct join_with_iterator;
	using joiner_t = Joiner;

	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Joiner m_joiner;
	[[no_unique_address]] with_inner_range<range_reference_t<Range>> m_inner;
};

template<range Range, forward_range Joiner>
join_with(Range &&, Joiner &&) -> join_with<Range, Joiner>;

} // namespace containers