# Copyright David Stone 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(bounded_integer LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()	

enable_testing()

get_directory_property(has_parent PARENT_DIRECTORY)
if (NOT has_parent)
	set(CMAKE_CXX_STANDARD 26)
	set(CMAKE_CXX_STANDARD_REQUIRED On)
	set(CMAKE_CXX_EXTENSIONS Off)

	add_subdirectory(dependencies/cmake_strict_defaults)
	add_subdirectory(dependencies/numeric-traits)
	add_subdirectory(dependencies/operators)
	add_subdirectory(dependencies/std_module)

	add_subdirectory(dependencies/Catch2 EXCLUDE_FROM_ALL)

	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable testing benchmark.")
	add_subdirectory(dependencies/benchmark EXCLUDE_FROM_ALL)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		target_compile_options(benchmark PUBLIC
			"-Wno-global-constructors"
			"-Wno-shift-sign-overflow"
		)
	endif()
endif()

add_subdirectory(source/bounded)
add_subdirectory(source/tv)
add_subdirectory(source/containers)
