// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/generate.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/count_type.hpp>
#include <containers/data.hpp>
#include <containers/front_back.hpp>
#include <containers/iterator_t.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/resizable_container.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <concepts>
#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto insert_without_reallocation(Container & container, iterator_t<Container const &> const position, auto && input_range, count_type<Container> const number_of_elements) {
	auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
	auto const original_end = containers::end(container);
	auto const new_end = original_end + number_of_elements;
	auto const new_position = containers::uninitialized_relocate(
		containers::reversed(range_view(mutable_position, original_end)),
		containers::reverse_iterator(new_end)
	).base();
	try {
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(input_range), mutable_position);
	} catch (...) {
		BOUNDED_ASSERT(new_position == mutable_position + number_of_elements);
		::containers::uninitialized_relocate(range_view(new_position, new_end), mutable_position);
		throw;
	}
	container.append_from_capacity(number_of_elements);
	return mutable_position;
}

template<typename Container>
constexpr auto insert_with_reallocation(Container & container, iterator_t<Container const &> const position, auto && input_range, auto const number_of_elements) {
	// There is a reallocation required, so just put everything in the
	// correct place to begin with
	auto const original_size = containers::size(container);
	auto temp = Container();
	temp.reserve(::containers::detail::reallocation_size(container.capacity(), original_size, number_of_elements));
	// First construct the new element because the arguments to
	// construct it may reference an old element. We cannot move
	// elements it references before constructing it
	auto const offset = position - containers::begin(container);
	containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(input_range), containers::data(temp) + offset);
	auto const mutable_position = containers::begin(container) + offset;
	auto const temp_begin = containers::begin(temp);
	auto const it = containers::uninitialized_relocate_no_overlap(
		range_view(containers::begin(container), mutable_position),
		temp_begin
	);
	BOUNDED_ASSERT(temp_begin + offset == it);
	::containers::uninitialized_relocate_no_overlap(
		range_view(mutable_position, containers::end(container)),
		it + static_cast<offset_type<iterator_t<Container &>>>(number_of_elements)
	);
	container.append_from_capacity(-original_size);
	temp.append_from_capacity(original_size + number_of_elements);
	container = std::move(temp);
	return containers::begin(container) + offset;
}

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, iterator_t<Container const &> it) {
	return (containers::begin(container) <= it) and (it <= containers::end(container));
}

} // namespace detail

// TODO: Check if the range lies within the container
template<resizable_container Container, range Range> requires std::convertible_to<range_value_t<Container>, range_value_t<Range>>
constexpr auto insert(Container & container, iterator_t<Container const &> position, Range && range) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const range_size = ::containers::detail::linear_size(range);
	if (containers::size(container) + range_size <= container.capacity()) {
		return ::containers::detail::insert_without_reallocation(container, position, OPERATORS_FORWARD(range), static_cast<count_type<Container>>(range_size));
	} else if constexpr (detail::reservable<Container>) {
		return ::containers::detail::insert_with_reallocation(container, position, OPERATORS_FORWARD(range), range_size);
	} else {
		bounded::assert_or_assume_unreachable();
	}
}

// If `constructor` references any element at or beyond `position` and
// `container` does not have reference stability on insertion, the behavior is
// undefined.
template<resizable_container Container>
constexpr auto lazy_insert(
	Container & container,
	iterator_t<Container const &> const position,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	return ::containers::insert(container, position, generate_n(1_bi, [&] { return OPERATORS_FORWARD(constructor)(); }));
}

// If `args` reference any element at or beyond `position` and `container` does
// not have reference stability on insertion, the behavior is undefined. Note
// that this is unlike member `emplace` on `std::vector` and `std::deque`. See
// https://cplusplus.github.io/LWG/issue2164 for a discussion on the standard
// containers. This is an intentional behavioral difference; with the standard's
// constraints, `emplace` becomes equivalent to
// `insert(container, position, T(args...))` -- that is to say, the element is
// not actually emplaced.
template<resizable_container Container>
constexpr auto emplace(Container & container, iterator_t<Container const &> const position, auto && ... args) {
	return ::containers::lazy_insert(container, position, [&] {
		return bounded::construct_return<range_value_t<Container>>(OPERATORS_FORWARD(args)...);
	});
}

// TODO: Determine how to handle aliasing with `value`, probably with another
// function
template<resizable_container Container>
constexpr auto insert(Container & container, iterator_t<Container const &> const position, range_value_t<Container> const & value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(value));
}
template<resizable_container Container>
constexpr auto insert(Container & container, iterator_t<Container const &> const position, range_value_t<Container> && value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(std::move(value)));
}

} // namespace containers
