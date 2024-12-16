# Copyright David Stone 2024.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_library(bounded_arithmetic_test)

target_sources(bounded_arithmetic_test PRIVATE
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		pointer.cpp
)

target_link_libraries(bounded_arithmetic_test PUBLIC
	bounded
	strict_defaults
)

add_test(bounded_arithmetic_test bounded_arithmetic_test)
