# Copyright David Stone 2024.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_library(tv STATIC)

target_link_libraries(tv
	PUBLIC
		bounded
		std_module
		strict_defaults_interface
	PRIVATE
		strict_defaults
)

target_sources(tv PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		get_index.cpp
		indexed_value.cpp
		insert.cpp
		is_valid_index.cpp
		none.cpp
		nth_type.cpp
		optional.cpp
		overload.cpp
		single_element_storage.cpp
		transform.cpp
		tuple.cpp
		tuple_cat.cpp
		tuple_size.cpp
		tv.cpp
		variadic_union.cpp
		variant.cpp
		variant_index.cpp
		visit.cpp
)

add_library(tv_test)

target_link_libraries(tv_test
	PRIVATE
		bounded
		std_module
		strict_defaults
		tv
)

target_sources(tv_test PRIVATE
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		test/is_valid_index.cpp
		test/optional.cpp
		test/transform.cpp
		test/tuple.cpp
		test/tuple_cat.cpp
		test/tuple_size.cpp
		test/variant.cpp
		test/visit.cpp
)
