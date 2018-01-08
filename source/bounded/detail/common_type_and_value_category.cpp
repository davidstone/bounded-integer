// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/common_type_and_value_category.hpp>

namespace {

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int, int>, int>{},
	"Incorrect type for int, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const, int>, int const>{},
	"Incorrect type for int const, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int, int volatile>, int volatile>{},
	"Incorrect type for int, int volatile."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const volatile, int>, int const volatile>{},
	"Incorrect type for int const volatile, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int volatile, int const volatile>, int const volatile>{},
	"Incorrect type for int volatile, int const volatile."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int volatile, int const>, int const volatile>{},
	"Incorrect type for int volatile, int const."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const volatile, int const volatile>, int const volatile>{},
	"Incorrect type for int const volatile, int const volatile."
);





static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int>, int>{},
	"Incorrect type for int."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, int>, int>{},
	"Incorrect type for int, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int>, int>{},
	"Incorrect type for int &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int>, int>{},
	"Incorrect type for int const &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int>, int>{},
	"Incorrect type for int &&, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int const &>, int const &>{},
	"Incorrect type for int const &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int const &>, int const &>{},
	"Incorrect type for int &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int const &>, int>{},
	"Incorrect type for int &&, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int &>, int &>{},
	"Incorrect type for int &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &>, int>{},
	"Incorrect type for int &&, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &&>, int &&>{},
	"Incorrect type for int &&, int &&."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, long>, long>{},
	"Incorrect type for int, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long>, long>{},
	"Incorrect type for int &, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long>, long>{},
	"Incorrect type for int const &, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long>, long>{},
	"Incorrect type for int &&, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long const &>, long>{},
	"Incorrect type for int const &, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long const &>, long>{},
	"Incorrect type for int &, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long const &>, long>{},
	"Incorrect type for int &&, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long &>, long>{},
	"Incorrect type for int &, long &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &>, long>{},
	"Incorrect type for int &&, long &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &&>, long>{},
	"Incorrect type for int &&, long &&."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int>, long>{},
	"Incorrect type for long &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int>, long>{},
	"Incorrect type for long const &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int>, long>{},
	"Incorrect type for long &&, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int const &>, long>{},
	"Incorrect type for long const &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int const &>, long>{},
	"Incorrect type for long &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int const &>, long>{},
	"Incorrect type for long &&, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int &>, long>{},
	"Incorrect type for long &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &>, long>{},
	"Incorrect type for long &&, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &&>, long>{},
	"Incorrect type for long &&, int &&."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, int, int>, int>{},
	"Incorrect type for int, int, int."
);

}	// namespace
