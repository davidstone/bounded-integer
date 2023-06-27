# Copyright David Stone 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(bounded_integer LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()	

enable_testing()

get_directory_property(has_parent PARENT_DIRECTORY)
if (NOT has_parent)
	set(CMAKE_CXX_STANDARD 23)
	set(CMAKE_CXX_STANDARD_REQUIRED On)
	set(CMAKE_CXX_EXTENSIONS Off)

	set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
	set(CMake_TEST_CXXModules_UUID "a246741c-d067-4019-a8fb-3d16b0c9d1d3")
	set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

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

add_library(bounded STATIC)

target_link_libraries(bounded
	PUBLIC
		strict_defaults_interface
		numeric_traits
		operators
		std_module
	PRIVATE
		strict_defaults
)

target_include_directories(bounded PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_sources(bounded PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		source/bounded/arithmetic/base.cpp
		source/bounded/arithmetic/bitwise_and.cpp
		source/bounded/arithmetic/byte.cpp
		source/bounded/arithmetic/divides.cpp
		source/bounded/arithmetic/extreme_values.cpp
		source/bounded/arithmetic/left_shift.cpp
		source/bounded/arithmetic/increment_decrement.cpp
		source/bounded/arithmetic/modulus.cpp
		source/bounded/arithmetic/multiplies.cpp
		source/bounded/arithmetic/operators_builtin.cpp
		source/bounded/arithmetic/operators.cpp
		source/bounded/arithmetic/plus.cpp
		source/bounded/arithmetic/pointer.cpp
		source/bounded/arithmetic/right_shift.cpp
		source/bounded/arithmetic/round_up_divide.cpp
		source/bounded/arithmetic/safe_abs.cpp
		source/bounded/arithmetic/unary_minus.cpp
		source/bounded/arithmetic/unary_plus.cpp
		source/bounded/abs.cpp
		source/bounded/add_common_cv_reference.cpp
		source/bounded/assert_assume_helpers.cpp
		source/bounded/bounded.cpp
		source/bounded/builtin_min_max_value.cpp
		source/bounded/cast.cpp
		source/bounded/check_in_range.cpp
		source/bounded/clamp.cpp
		source/bounded/common_type_and_value_category.cpp
		source/bounded/comparison.cpp
		source/bounded/comparison_builtin.cpp
		source/bounded/comparison_function_object.cpp
		source/bounded/concepts.cpp
		source/bounded/construct_destroy.cpp
		source/bounded/copy.cpp
		source/bounded/copy_cv_ref.cpp
		source/bounded/declval.cpp
		source/bounded/function_ptr.cpp
		source/bounded/hash.cpp
		source/bounded/homogeneous_equals.cpp
		source/bounded/integer.cpp
		source/bounded/integer_tombstone_traits.cpp
		source/bounded/is_bounded_integer.cpp
		source/bounded/lazy_init.cpp
		source/bounded/literal.cpp
		source/bounded/log.cpp
		source/bounded/make_index_sequence.cpp
		source/bounded/minmax.cpp
		source/bounded/normalize.cpp
		source/bounded/number_of.cpp
		source/bounded/numeric_limits.cpp
		source/bounded/overlapping_range.cpp
		source/bounded/pow.cpp
		source/bounded/relocate.cpp
		source/bounded/representation_digits.cpp
		source/bounded/safe_extreme.cpp
		source/bounded/scope_guard.cpp
		source/bounded/size_of.cpp
		source/bounded/std_iterator.cpp
		source/bounded/stream.cpp
		source/bounded/test_int.cpp
		source/bounded/to_integer.cpp
		source/bounded/tombstone_traits.cpp
		source/bounded/type.cpp
		source/bounded/underlying_type.cpp
		source/bounded/value_to_function.cpp
)

# Until resolution of https://github.com/llvm/llvm-project/issues/60089
set_source_files_properties(source/bounded/arithmetic/increment_decrement.cpp
	PROPERTIES COMPILE_FLAGS "-Wno-implicit-int-conversion -Wno-shorten-64-to-32"
)

add_executable(bounded_test)

target_sources(bounded_test PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		test/bounded/assert.cpp
		test/bounded/assert-NDEBUG.cpp
		test/bounded/assume.cpp
		test/bounded/check_in_range.cpp
		test/bounded/comparison.cpp
		test/bounded/conditional.cpp
		test/bounded/stream.cpp
		test/bounded/to_integer.cpp
)

set_source_files_properties(source/bounded/detail/arithmetic/operators.cpp
	PROPERTIES COMPILE_FLAGS "-Wno-implicit-int-conversion -Wno-shorten-64-to-32"
)

target_link_libraries(bounded_test PUBLIC Catch2::Catch2WithMain bounded strict_defaults)

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
		source/tv/get_index.cpp
		source/tv/insert.cpp
		source/tv/is_valid_index.cpp
		source/tv/nth_type.cpp
		source/tv/optional.cpp
		source/tv/overload.cpp
		source/tv/single_element_storage.cpp
		source/tv/tuple.cpp
		source/tv/tv.cpp
		source/tv/variadic_union.cpp
		source/tv/variant.cpp
		source/tv/variant_index.cpp
		source/tv/visit.cpp
)

add_library(containers STATIC)

target_sources(containers PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		source/containers/algorithms/sort/cheaply_sortable.cpp
		source/containers/algorithms/sort/chunked_insertion_sort.cpp
		source/containers/algorithms/sort/common_prefix.cpp
		source/containers/algorithms/sort/dereference_all.cpp
		source/containers/algorithms/sort/double_buffered_ska_sort.cpp
		source/containers/algorithms/sort/fixed_size_merge_sort.cpp
		source/containers/algorithms/sort/inplace_radix_sort.cpp
		source/containers/algorithms/sort/is_sorted.cpp
		source/containers/algorithms/sort/merge_relocate_second_range.cpp
		source/containers/algorithms/sort/relocate_in_order.cpp
		source/containers/algorithms/sort/rotate_one.cpp
		source/containers/algorithms/sort/ska_sort.cpp
		source/containers/algorithms/sort/small_size_optimized_sort.cpp
		source/containers/algorithms/sort/sort.cpp
		source/containers/algorithms/sort/sort_exactly_1.cpp
		source/containers/algorithms/sort/sort_exactly_2.cpp
		source/containers/algorithms/sort/sort_exactly_3.cpp
		source/containers/algorithms/sort/sort_exactly_4.cpp
		source/containers/algorithms/sort/sort_exactly_5.cpp
		source/containers/algorithms/sort/sort_exactly_6.cpp
		source/containers/algorithms/sort/to_radix_sort_key.cpp
		source/containers/algorithms/accumulate.cpp
		source/containers/algorithms/advance.cpp
		source/containers/algorithms/all_any_none.cpp
		source/containers/algorithms/binary_search.cpp
		source/containers/algorithms/compare.cpp
		source/containers/algorithms/concatenate.cpp
		source/containers/algorithms/concatenate_view.cpp
		source/containers/algorithms/copy.cpp
		source/containers/algorithms/copy_or_relocate_from.cpp
		source/containers/algorithms/count.cpp
		source/containers/algorithms/destroy_range.cpp
		source/containers/algorithms/distance.cpp
		source/containers/algorithms/erase.cpp
		source/containers/algorithms/filter.cpp
		source/containers/algorithms/find.cpp
		source/containers/algorithms/generate.cpp
		source/containers/algorithms/keyed_binary_search.cpp
		source/containers/algorithms/keyed_erase.cpp
		source/containers/algorithms/keyed_insert.cpp
		source/containers/algorithms/maybe_find.cpp
		source/containers/algorithms/minmax_element.cpp
		source/containers/algorithms/move_iterator.cpp
		source/containers/algorithms/partition.cpp
		source/containers/algorithms/remove_none.cpp
		source/containers/algorithms/reverse.cpp
		source/containers/algorithms/reverse_iterator.cpp
		source/containers/algorithms/set.cpp
		source/containers/algorithms/splice.cpp
		source/containers/algorithms/transform.cpp
		source/containers/algorithms/transform_iterator.cpp
		source/containers/algorithms/uninitialized.cpp
		source/containers/algorithms/unique.cpp
		source/containers/std/list.cpp
		source/containers/std/std_allocator.cpp
		source/containers/std/vector.cpp
		source/containers/adapt.cpp
		source/containers/addable_subtractable.cpp
		source/containers/append.cpp
		source/containers/append_after.cpp
		source/containers/append_into_capacity.cpp
		source/containers/array.cpp
		source/containers/as_mutable_ptr.cpp
		source/containers/assign.cpp
		source/containers/assign_to_empty.cpp
		source/containers/assign_to_empty_into_capacity.cpp
		source/containers/associative_container.cpp
		source/containers/at.cpp
		source/containers/batched.cpp
		source/containers/begin_end.cpp
		source/containers/bidirectional_linked_list.cpp
		source/containers/c_array.cpp
		source/containers/can_set_size.cpp
		source/containers/clear.cpp
		source/containers/common_functions.cpp
		source/containers/common_iterator_functions.cpp
		source/containers/compare_container.cpp
		source/containers/containers.cpp
		source/containers/contiguous_iterator.cpp
		source/containers/count_type.cpp
		source/containers/data.cpp
		source/containers/dereference.cpp
		source/containers/default_adapt_traits.cpp
		source/containers/default_begin_end_size.cpp
		source/containers/dynamic_array.cpp
		source/containers/dynamic_array_data.cpp
		source/containers/emplace_back.cpp
		source/containers/empty_range.cpp
		source/containers/erase_concepts.cpp
		source/containers/extract_key_to_less.cpp
		source/containers/flat_map.cpp
		source/containers/forward_linked_list.cpp
		source/containers/front_back.cpp
		source/containers/get_source_size.cpp
		source/containers/has_member_before_begin.cpp
		source/containers/has_member_size.cpp
		source/containers/index_type.cpp
		source/containers/initializer_range.cpp
		source/containers/insert.cpp
		source/containers/integer_range.cpp
		source/containers/is_container.cpp
		source/containers/is_empty.cpp
		source/containers/is_iterator.cpp
		source/containers/is_iterator_sentinel.cpp
		source/containers/is_range.cpp
		source/containers/iter_difference_t.cpp
		source/containers/iter_reference_t.cpp
		source/containers/iter_value_t.cpp
		source/containers/iterator_adapter.cpp
		source/containers/iterator_category_base.cpp
		source/containers/iterator_t.cpp
		source/containers/key_comparison_function.cpp
		source/containers/lazy_push_back.cpp
		source/containers/lazy_push_back_into_capacity.cpp
		source/containers/lazy_push_front.cpp
		source/containers/legacy_append.cpp
		source/containers/legacy_iterator.cpp
		source/containers/linear_map.cpp
		source/containers/linked_list_helper.cpp
		source/containers/lookup.cpp
		source/containers/map_tags.cpp
		source/containers/map_value_type.cpp
		source/containers/maximum_array_size.cpp
		source/containers/member_assign.cpp
		source/containers/member_lazy_push_backable.cpp
		source/containers/member_push_backable.cpp
		source/containers/mutable_iterator.cpp
		source/containers/offset_type.cpp
		source/containers/ordered_associative_container.cpp
		source/containers/pop_back.cpp
		source/containers/pop_front.cpp
		source/containers/push_back.cpp
		source/containers/push_back_into_capacity.cpp
		source/containers/push_front.cpp
		source/containers/range_reference_t.cpp
		source/containers/range_value_t.cpp
		source/containers/range_view.cpp
		source/containers/reallocation_size.cpp
		source/containers/reference_wrapper.cpp
		source/containers/repeat_n.cpp
		source/containers/reservable.cpp
		source/containers/resizable_container.cpp
		source/containers/resize.cpp
		source/containers/shrink_to_fit.cpp
		source/containers/size.cpp
		source/containers/size_then_use_range.cpp
		source/containers/small_buffer_optimized_vector.cpp
		source/containers/splicable.cpp
		source/containers/stable_vector.cpp
		source/containers/static_vector.cpp
		source/containers/string.cpp
		source/containers/supports_lazy_insert_after.cpp
		source/containers/take.cpp
		source/containers/to_address.cpp
		source/containers/to_any_string.cpp
		source/containers/to_string.cpp
		source/containers/trivial_inplace_function.cpp
		source/containers/uninitialized_array.cpp
		source/containers/uninitialized_dynamic_array.cpp
		source/containers/vector.cpp
)

target_link_libraries(containers
	PUBLIC
		bounded
		strict_defaults_interface
		operators
		std_module
		tv
	PRIVATE
		strict_defaults
)

add_executable(containers_test)

target_sources(containers_test PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		test/containers/sort/chunked_insertion_sort.cpp
		test/containers/sort/double_buffered_ska_sort.cpp
		test/containers/sort/sort_test_data.cpp
		test/containers/sort/ska_sort.cpp
		test/containers/sort/sort.cpp
		test/containers/sort/sort_exactly_n.cpp
		test/containers/sort/test_sort.cpp
		test/containers/algorithms/copy_or_relocate_from.cpp
		test/containers/algorithms/erase.cpp
		test/containers/append.cpp
		test/containers/array.cpp
		test/containers/assign.cpp
		test/containers/assign_to_empty.cpp
		test/containers/bidirectional_linked_list.cpp
		test/containers/clear.cpp
		test/containers/concatenate.cpp
		test/containers/dynamic_array.cpp
		test/containers/flat_map.cpp
		test/containers/forward_linked_list.cpp
		test/containers/is_container.cpp
		test/containers/lazy_push_back.cpp
		test/containers/lazy_push_back_into_capacity.cpp
		test/containers/linear_map.cpp
		test/containers/pop_back.cpp
		test/containers/push_back.cpp
		test/containers/push_back_into_capacity.cpp
		test/containers/stable_vector.cpp
		test/containers/test_associative_container.cpp
		test/containers/test_reserve_and_capacity.cpp
		test/containers/test_sequence_container.cpp
		test/containers/test_set_size.cpp
		test/containers/uninitialized_dynamic_array.cpp
		test/containers/vector.cpp
)

target_sources(containers_test PUBLIC
	test/containers/sort/to_radix_sort_key.cpp
	test/containers/at.cpp
	test/containers/small_buffer_optimized_vector.cpp
	test/containers/static_vector.cpp
	test/containers/string.cpp
	test/containers/take.cpp
	test/containers/trivial_inplace_function.cpp
)

target_link_libraries(containers_test PRIVATE Catch2::Catch2WithMain containers strict_defaults)
# https://github.com/catchorg/Catch2/issues/578
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(containers_test PRIVATE
		"-Wno-reserved-identifier"
	)
endif()

add_executable(flat_map
	test/containers/map_benchmark.cpp
)
target_compile_definitions(flat_map PRIVATE "USE_FLAT_MAP")
target_link_libraries(flat_map PUBLIC containers strict_defaults)

add_executable(std_map
	test/containers/map_benchmark.cpp
)
target_compile_definitions(std_map PRIVATE "USE_SYSTEM_MAP")
target_link_libraries(std_map PUBLIC containers strict_defaults)

add_executable(ska_sort_benchmark
	test/containers/sort/ska_sort_benchmark.cpp
)
target_link_libraries(ska_sort_benchmark PUBLIC bounded benchmark containers strict_defaults)

add_executable(sort_benchmark
	test/containers/sort/sort_benchmark.cpp
)
target_link_libraries(sort_benchmark PUBLIC bounded benchmark_main containers strict_defaults)

add_executable(vector_benchmark
	test/containers/vector_benchmark.cpp
)
target_link_libraries(vector_benchmark PUBLIC bounded benchmark_main containers strict_defaults)


set(test_targets
	bounded_test
	containers_test
	flat_map
)

foreach(test_target ${test_targets})
	add_test(${test_target} ${test_target})
endforeach()
