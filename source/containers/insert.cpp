// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.insert;

import containers.algorithms.generate;
import containers.algorithms.reverse_iterator;
import containers.algorithms.splice;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.count_type;
import containers.data;
import containers.iterator_t;
import containers.linear_size;
import containers.mutable_iterator;
import containers.offset_type;
import containers.range;
import containers.range_value_t;
import containers.reallocation_size;
import containers.reservable;
import containers.resizable_container;
import containers.size;
import containers.splicable;
import containers.subrange;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Container>
constexpr auto insert_without_reallocation(Container & container, iterator_t<Container const &> const position, auto && input_range, count_type<Container> const number_of_elements) -> iterator_t<Container &> {
	auto const mutable_position = ::containers::mutable_iterator(container, position);
	auto const original_end = containers::end(container);
	auto const new_end = original_end + number_of_elements;
	auto const new_position = containers::uninitialized_relocate(
		containers::reversed(subrange(mutable_position, original_end)),
		containers::reverse_iterator(new_end)
	).base();
	BOUNDED_ASSERT(new_position == mutable_position + number_of_elements);
	try {
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(input_range), mutable_position);
	} catch (...) {
		::containers::uninitialized_relocate(subrange(new_position, new_end), mutable_position);
		throw;
	}
	container.set_size(containers::size(container) + number_of_elements);
	return mutable_position;
}

template<typename Container>
constexpr auto insert_with_reallocation(Container & container, iterator_t<Container const &> const position, auto && input_range, auto const number_of_elements) -> iterator_t<Container &> {
	// There is a reallocation required, so just put everything in the
	// correct place to begin with
	auto const original_size = containers::size(container);
	auto temp = Container();
	temp.reserve(::containers::reallocation_size(container.capacity(), original_size, number_of_elements));
	// First construct the new element because the arguments to
	// construct it may reference an old element. We cannot move
	// elements it references before constructing it
	auto const offset = position - containers::begin(container);
	containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(input_range), containers::data(temp) + offset);
	auto const mutable_position = containers::begin(container) + offset;
	auto const temp_begin = containers::begin(temp);
	auto const it = containers::uninitialized_relocate_no_overlap(
		subrange(containers::begin(container), mutable_position),
		temp_begin
	);
	BOUNDED_ASSERT(temp_begin + offset == it);
	::containers::uninitialized_relocate_no_overlap(
		subrange(mutable_position, containers::end(container)),
		it + ::bounded::assume_in_range<offset_type<iterator_t<Container &>>>(number_of_elements)
	);
	container.set_size(0_bi);
	temp.set_size(original_size + number_of_elements);
	container = std::move(temp);
	return containers::begin(container) + offset;
}

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, iterator_t<Container const &> it) -> bool {
	return (containers::begin(container) <= it) and (it <= containers::end(container));
}

template<typename Container, typename Range>
constexpr auto to(Range && range) -> Container {
	if constexpr (bounded::constructible_from<Container, Range &&>) {
		return Container(OPERATORS_FORWARD(range));
	} else {
		return Container(
			containers::begin(OPERATORS_FORWARD(range)),
			containers::end(OPERATORS_FORWARD(range))
		);
	}
}

// TODO: Check if the range lies within the container
export template<resizable_container Container, range Range> requires bounded::convertible_to<range_value_t<Container>, range_value_t<Range>>
constexpr auto insert(Container & container, iterator_t<Container const &> position, Range && range) -> iterator_t<Container &> {
	if constexpr (splicable<Container>) {
		::containers::splice(container, position, ::containers::to<Container>(OPERATORS_FORWARD(range)));
		return ::containers::mutable_iterator(container, position);
	} else {
		BOUNDED_ASSERT(::containers::iterator_points_into_container(container, position));
		auto const range_size = ::containers::linear_size(range);
		if (containers::size(container) + range_size <= container.capacity()) {
			return ::containers::insert_without_reallocation(
				container,
				position,
				OPERATORS_FORWARD(range),
				::bounded::assume_in_range<count_type<Container>>(range_size)
			);
		} else if constexpr (reservable<Container>) {
			return ::containers::insert_with_reallocation(
				container,
				position,
				OPERATORS_FORWARD(range),
				range_size
			);
		} else {
			throw std::bad_alloc();
		}
	}
}

// If `constructor` references any element at or beyond `position` and
// `container` does not have reference stability on insertion, the behavior is
// undefined.
export template<resizable_container Container>
constexpr auto lazy_insert(
	Container & container,
	iterator_t<Container const &> const position,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) -> iterator_t<Container &> {
	return ::containers::insert(
		container,
		position,
		generate_n(1_bi, OPERATORS_FORWARD(constructor))
	);
}

// If `args` reference any element at or beyond `position` and `container` does
// not have reference stability on insertion, the behavior is undefined. Note
// that this is unlike member `emplace` on `std::vector` and `std::deque`. See
// https://cplusplus.github.io/LWG/issue2164 for a discussion on the standard
// containers. This is an intentional behavioral difference; with the standard's
// constraints, `emplace` becomes equivalent to
// `insert(container, position, T(args...))` -- that is to say, the element is
// not actually emplaced.
export template<resizable_container Container>
constexpr auto emplace(Container & container, iterator_t<Container const &> const position, auto && ... args) -> iterator_t<Container &> {
	return ::containers::lazy_insert(container, position, [&] {
		return range_value_t<Container>(OPERATORS_FORWARD(args)...);
	});
}

// TODO: Determine how to handle aliasing with `value`, probably with another
// function
export template<resizable_container Container>
constexpr auto insert(Container & container, iterator_t<Container const &> const position, range_value_t<Container> const & value) -> iterator_t<Container &> {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(value));
}
export template<resizable_container Container>
constexpr auto insert(Container & container, iterator_t<Container const &> const position, range_value_t<Container> && value) -> iterator_t<Container &> {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(std::move(value)));
}

} // namespace containers
