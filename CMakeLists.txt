# Copyright David Stone 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

project(bounded_integer LANGUAGES CXX)

enable_testing()

add_subdirectory(operators)

add_library(bounded INTERFACE)

target_link_libraries(bounded INTERFACE
	operators
)

target_include_directories(bounded INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_library(containers INTERFACE)

target_link_libraries(containers INTERFACE
	bounded
)

target_include_directories(containers INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_executable(bounded_test
	source/bounded/detail/arithmetic/base.cpp
	source/bounded/detail/arithmetic/bitwise_and.cpp
	source/bounded/detail/arithmetic/divides.cpp
	source/bounded/detail/arithmetic/left_shift.cpp
	source/bounded/detail/arithmetic/modulus.cpp
	source/bounded/detail/arithmetic/multiplies.cpp
	source/bounded/detail/arithmetic/operators_builtin.cpp
	source/bounded/detail/arithmetic/operators.cpp
	source/bounded/detail/arithmetic/plus.cpp
	source/bounded/detail/arithmetic/pointer.cpp
	source/bounded/detail/arithmetic/right_shift.cpp
	source/bounded/detail/arithmetic/safe_abs.cpp
	source/bounded/detail/arithmetic/unary_minus.cpp
	source/bounded/detail/arithmetic/unary_plus.cpp
	source/bounded/detail/policy/clamp_policy.cpp
	source/bounded/detail/policy/modulo_policy.cpp
	source/bounded/detail/policy/null_policy.cpp
	source/bounded/detail/policy/throw_policy.cpp
	source/bounded/detail/variant/get_index.cpp
	source/bounded/detail/variant/variant.cpp
	source/bounded/detail/variant/visit.cpp
	source/bounded/detail/abs.cpp
	source/bounded/detail/assume.cpp
	source/bounded/detail/builtin_min_max_value.cpp
	source/bounded/detail/cast.cpp
	source/bounded/detail/class.cpp
	source/bounded/detail/common_type_and_value_category.cpp
	source/bounded/detail/comparison.cpp
	source/bounded/detail/comparison_function_object.cpp
	source/bounded/detail/comparison_mixed.cpp
	source/bounded/detail/conditional.cpp
	source/bounded/detail/construct_destroy.cpp
	source/bounded/detail/copy_cv_ref.cpp
	source/bounded/detail/int128.cpp
	source/bounded/detail/integer_tombstone_traits.cpp
	source/bounded/detail/is_bounded_integer.cpp
	source/bounded/detail/literal.cpp
	source/bounded/detail/log.cpp
	source/bounded/detail/make_index_sequence.cpp
	source/bounded/detail/max_builtin.cpp
	source/bounded/detail/minmax.cpp
	source/bounded/detail/min_max_value.cpp
	source/bounded/detail/modulo_cast.cpp
	source/bounded/detail/normalize.cpp
	source/bounded/detail/overlapping_range.cpp
	source/bounded/detail/overload.cpp
	source/bounded/detail/pow.cpp
	source/bounded/detail/size_of.cpp
	source/bounded/detail/stream.cpp
	source/bounded/detail/tuple.cpp
	source/bounded/detail/type.cpp
	source/bounded/detail/underlying_type.cpp
	source/bounded/assert.cpp
	source/bounded/assert-NDEBUG.cpp
	source/bounded/concepts.cpp
	source/bounded/copy.cpp
	source/bounded/representation_digits.cpp
	source/bounded/hash.cpp
	source/bounded/identity.cpp
	source/bounded/insert.cpp
	source/bounded/integer.cpp
	source/bounded/lazy_init.cpp
	source/bounded/numeric_limits.cpp
	source/bounded/nth_type.cpp
	source/bounded/optional.cpp
	source/bounded/std_iterator.cpp
	source/bounded/string.cpp
	source/bounded/to_integer.cpp
	source/bounded/tombstone_traits.cpp
	source/bounded/value_to_function.cpp
)

target_link_libraries(bounded_test PUBLIC bounded strict_defaults)

add_library(containers_test STATIC
	source/containers/algorithms/accumulate.cpp
	source/containers/algorithms/advance.cpp
	source/containers/algorithms/all_any_none.cpp
	source/containers/algorithms/binary_search.cpp
	source/containers/algorithms/compare.cpp
	source/containers/algorithms/concatenate.cpp
	source/containers/algorithms/concatenate_view.cpp
	source/containers/algorithms/copy.cpp
	source/containers/algorithms/copy_n.cpp
	source/containers/algorithms/count.cpp
	source/containers/algorithms/distance.cpp
	source/containers/algorithms/filter_iterator.cpp
	source/containers/algorithms/find.cpp
	source/containers/algorithms/maybe_find.cpp
	source/containers/algorithms/minmax_element.cpp
	source/containers/algorithms/move_destroy_iterator.cpp
	source/containers/algorithms/move_iterator.cpp
	source/containers/algorithms/negate.cpp
	source/containers/algorithms/partition.cpp
	source/containers/algorithms/remove.cpp
	source/containers/algorithms/reverse_iterator.cpp
	source/containers/algorithms/set.cpp
	source/containers/algorithms/sort.cpp
	source/containers/algorithms/transform.cpp
	source/containers/algorithms/transform_iterator.cpp
	source/containers/algorithms/uninitialized.cpp
	source/containers/algorithms/unique.cpp
	source/containers/array/array.cpp
	source/containers/array/make_array.cpp
	source/containers/array/tuple.cpp
	source/containers/static_vector/make_static_vector.cpp
	source/containers/adapt.cpp
	source/containers/append.cpp
	source/containers/assign.cpp
	source/containers/at.cpp
	source/containers/begin_end.cpp
	source/containers/clear.cpp
	source/containers/common_container_functions.cpp
	source/containers/contiguous_iterator.cpp
	source/containers/data.cpp
	source/containers/emplace_back.cpp
	source/containers/empty.cpp
	source/containers/erase.cpp
	source/containers/front_back.cpp
	source/containers/insert.cpp
	source/containers/integer_range.cpp
	source/containers/is_container.cpp
	source/containers/is_iterator.cpp
	source/containers/is_iterator_sentinel.cpp
	source/containers/is_range.cpp
	source/containers/iterator_adapter.cpp
	source/containers/lazy_push_back.cpp
	source/containers/legacy_iterator.cpp
	source/containers/maximum_array_size.cpp
	source/containers/mutable_iterator.cpp
	source/containers/non_modifying_common_container_functions.cpp
	source/containers/pop_back.cpp
	source/containers/pop_front.cpp
	source/containers/push_back.cpp
	source/containers/range_view.cpp
	source/containers/reference_wrapper.cpp
	source/containers/repeat_n.cpp
	source/containers/reserve_if_reservable.cpp
	source/containers/resize.cpp
	source/containers/scope_guard.cpp
	source/containers/single_element_range.cpp
	source/containers/shrink_to_fit.cpp
	source/containers/size.cpp
	source/containers/stable_vector.cpp
	source/containers/string.cpp
	source/containers/take.cpp
	source/containers/uninitialized_dynamic_array.cpp
	source/containers/vector.cpp
)

target_link_libraries(containers_test PUBLIC containers strict_defaults)

add_executable(concatenate
	source/containers/algorithms/concatenate.cpp
)
target_link_libraries(concatenate PUBLIC containers strict_defaults)

add_executable(static_vector
	source/containers/static_vector/static_vector.cpp
)
target_link_libraries(static_vector PUBLIC containers strict_defaults)

add_executable(small_buffer_optimized_vector
	source/containers/small_buffer_optimized_vector.cpp
)
target_link_libraries(small_buffer_optimized_vector PUBLIC containers strict_defaults)

add_executable(flat_map
	source/containers/flat_map.cpp
)
target_compile_definitions(flat_map PRIVATE "USE_FLAT_MAP")
target_link_libraries(flat_map PUBLIC containers strict_defaults)

add_executable(std_map
	source/containers/flat_map.cpp
)
target_compile_definitions(std_map PRIVATE "USE_SYSTEM_MAP")
target_link_libraries(std_map PUBLIC containers strict_defaults)

add_executable(dynamic_array
	source/containers/dynamic_array.cpp
)
target_link_libraries(dynamic_array PUBLIC containers strict_defaults)

SET(gtest_disable_pthreads ON CACHE BOOL "Disable uses of pthreads in gtest.")
add_subdirectory("${PROJECT_SOURCE_DIR}/source/googletest-1.8.1")
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable testing benchmark.")
add_subdirectory("${PROJECT_SOURCE_DIR}/source/benchmark")
set_target_properties(benchmark PROPERTIES FOLDER extern)

add_executable(ska_sort
	source/containers/algorithms/ska_sort.cpp
)
target_link_libraries(ska_sort PUBLIC bounded gtest gtest_main benchmark strict_defaults)

add_executable(trivial_inplace_function
	source/containers/trivial_inplace_function.cpp
)
target_link_libraries(trivial_inplace_function PUBLIC containers strict_defaults)


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	if(NOT MSVC)
		target_compile_options(gtest PUBLIC
			"-stdlib=libc++"
			"-std=c++20"
		)
		target_compile_options(gtest_main PUBLIC
			"-stdlib=libc++"
			"-std=c++20"
		)
		target_compile_options(benchmark PUBLIC
			"-stdlib=libc++"
			"-std=c++20"
		)
	endif()

	target_compile_options(ska_sort PRIVATE
		"-Wno-global-constructors"
		"-Wno-implicit-int-conversion"
		"-Wno-sign-conversion"
		"-Wno-used-but-marked-unused"
		"-Wno-unused-function"
		"-Wno-unused-macros"
		"-Wno-unused-template"
		"-Wno-unused-variable"
	)

	target_compile_options(benchmark PUBLIC
		"-Wno-suggest-override"
	)

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
else()
	message(FATAL_ERROR "Unknown compiler " "${CMAKE_CXX_COMPILER_ID}")
endif()

set(test_targets
	bounded_test
	concatenate
	static_vector
	small_buffer_optimized_vector
	flat_map
	dynamic_array
	variant
	ska_sort
)

foreach(test_target ${test_targets})
	add_test(${test_target} ${test_target})
endforeach()
