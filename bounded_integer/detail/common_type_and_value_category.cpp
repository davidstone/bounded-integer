// Verify that the header can stand on its own
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "common_type_and_value_category.hpp"



static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int, int>, int>::value,
	"Incorrect type for int, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const, int>, int const>::value,
	"Incorrect type for int const, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int, int volatile>, int volatile>::value,
	"Incorrect type for int, int volatile."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const volatile, int>, int const volatile>::value,
	"Incorrect type for int const volatile, int."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int volatile, int const volatile>, int const volatile>::value,
	"Incorrect type for int volatile, int const volatile."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int volatile, int const>, int const volatile>::value,
	"Incorrect type for int volatile, int const."
);

static_assert(
	std::is_same<bounded::detail::common_cv_type_t<int const volatile, int const volatile>, int const volatile>::value,
	"Incorrect type for int const volatile, int const volatile."
);





static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int>, int>::value,
	"Incorrect type for int."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, int, int>, int>::value,
	"Incorrect type for int, int, int."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, int>, int>::value,
	"Incorrect type for int, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int>, int>::value,
	"Incorrect type for int &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int>, int>::value,
	"Incorrect type for int const &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int>, int>::value,
	"Incorrect type for int &&, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, int const &>, int const &>::value,
	"Incorrect type for int const &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int const &>, int const &>::value,
	"Incorrect type for int &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int const &>, int const &>::value,
	"Incorrect type for int &&, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, int &>, int &>::value,
	"Incorrect type for int &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &>, int const &>::value,
	"Incorrect type for int &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, int &&>, int &&>::value,
	"Incorrect type for int &&, int &&."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int, long>, long>::value,
	"Incorrect type for int, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long>, long>::value,
	"Incorrect type for int &, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long>, long>::value,
	"Incorrect type for int const &, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long>, long>::value,
	"Incorrect type for int &&, long."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int const &, long const &>, long>::value,
	"Incorrect type for int const &, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long const &>, long>::value,
	"Incorrect type for int &, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long const &>, long>::value,
	"Incorrect type for int &&, long const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &, long &>, long>::value,
	"Incorrect type for int &, long &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &>, long>::value,
	"Incorrect type for int &, long &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<int &&, long &&>, long>::value,
	"Incorrect type for int &&, long &&."
);



static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int>, long>::value,
	"Incorrect type for int &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int>, long>::value,
	"Incorrect type for int const &, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int>, long>::value,
	"Incorrect type for int &&, int."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long const &, int const &>, long>::value,
	"Incorrect type for int const &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int const &>, long>::value,
	"Incorrect type for int &, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int const &>, long>::value,
	"Incorrect type for int &&, int const &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &, int &>, long>::value,
	"Incorrect type for int &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &>, long>::value,
	"Incorrect type for int &, int &."
);

static_assert(
	std::is_same<bounded::detail::common_type_and_value_category_t<long &&, int &&>, long>::value,
	"Incorrect type for int &&, int &&."
);

