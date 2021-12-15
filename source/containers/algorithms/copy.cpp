// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/copy.hpp>

#include <containers/array.hpp>
#include <containers/range_view.hpp>

namespace {

using namespace bounded::literal;

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy(
		containers::range_view(containers::begin(container), containers::begin(container)),
		containers::begin(container) + 2_bi
	);
	return container == containers::array{1, 2, 3, 4};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::begin(container) + 2_bi
	);
	return container == containers::array{1, 2, 1, 2};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy_backward(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::end(container)
	);
	return container == containers::array{1, 2, 1, 2};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3};
	containers::copy_backward(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::end(container)
	);
	return container == containers::array{1, 1, 2};
}());

} // namespace