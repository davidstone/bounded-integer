# Copyright David Stone 2024.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_library(containers STATIC)

target_sources(containers PUBLIC
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		algorithms/sort/cheaply_sortable.cpp
		algorithms/sort/chunked_insertion_sort.cpp
		algorithms/sort/common_prefix.cpp
		algorithms/sort/dereference_all.cpp
		algorithms/sort/double_buffered_ska_sort.cpp
		algorithms/sort/fixed_size_merge_sort.cpp
		algorithms/sort/low_high_ref.cpp
		algorithms/sort/inplace_radix_sort.cpp
		algorithms/sort/is_sorted.cpp
		algorithms/sort/merge_relocate_second_range.cpp
		algorithms/sort/relocate_in_order.cpp
		algorithms/sort/rotate_one.cpp
		algorithms/sort/ska_sort.cpp
		algorithms/sort/small_size_optimized_sort.cpp
		algorithms/sort/sort.cpp
		algorithms/sort/sort_exactly_1.cpp
		algorithms/sort/sort_exactly_2.cpp
		algorithms/sort/sort_exactly_3.cpp
		algorithms/sort/sort_exactly_4.cpp
		algorithms/sort/sort_exactly_5.cpp
		algorithms/sort/sort_exactly_6.cpp
		algorithms/sort/sort_exactly_n.cpp
		algorithms/sort/to_radix_sort_key.cpp
		algorithms/accumulate.cpp
		algorithms/adjacent.cpp
		algorithms/advance.cpp
		algorithms/all_any_none.cpp
		algorithms/binary_search.cpp
		algorithms/chunk_by.cpp
		algorithms/compare.cpp
		algorithms/concatenate.cpp
		algorithms/concatenate_view.cpp
		algorithms/copy.cpp
		algorithms/copy_or_relocate_from.cpp
		algorithms/count.cpp
		algorithms/destroy_range.cpp
		algorithms/distance.cpp
		algorithms/erase.cpp
		algorithms/filter.cpp
		algorithms/find.cpp
		algorithms/generate.cpp
		algorithms/generate_until.cpp
		algorithms/join.cpp
		algorithms/join_with.cpp
		algorithms/keyed_binary_search.cpp
		algorithms/keyed_erase.cpp
		algorithms/keyed_insert.cpp
		algorithms/maybe_find.cpp
		algorithms/minmax_element.cpp
		algorithms/move_range.cpp
		algorithms/partition.cpp
		algorithms/remove_none.cpp
		algorithms/reverse.cpp
		algorithms/reverse_iterator.cpp
		algorithms/set.cpp
		algorithms/splice.cpp
		algorithms/split.cpp
		algorithms/transform.cpp
		algorithms/uninitialized.cpp
		algorithms/unique.cpp
		algorithms/zip.cpp
		std/inplace_vector.cpp
		std/list.cpp
		std/std_allocator.cpp
		std/vector.cpp
		adapt.cpp
		adapt_iterator.cpp
		append.cpp
		append_after.cpp
		append_into_capacity.cpp
		array.cpp
		as_mutable_ptr.cpp
		assign.cpp
		assign_to_empty.cpp
		assign_to_empty_into_capacity.cpp
		associative_container.cpp
		at.cpp
		back.cpp
		batched.cpp
		begin_end.cpp
		bidirectional_iterator.cpp
		bidirectional_linked_list.cpp
		bidirectional_range.cpp
		bounded_vector.cpp
		c_array.cpp
		can_set_size.cpp
		clear.cpp
		common_functions.cpp
		common_iterator_functions.cpp
		compare_container.cpp
		containers.cpp
		contiguous_iterator.cpp
		count_type.cpp
		data.cpp
		dereference.cpp
		default_adapt_traits.cpp
		default_begin_end_size.cpp
		dynamic_array.cpp
		dynamic_array_data.cpp
		emplace_back.cpp
		emplace_back_into_capacity.cpp
		emplace_front.cpp
		erase_concepts.cpp
		extract_key_to_less.cpp
		flat_map.cpp
		forward_iterator.cpp
		forward_linked_list.cpp
		forward_random_access_iterator.cpp
		forward_random_access_range.cpp
		forward_range.cpp
		front.cpp
		get_source_size.cpp
		has_member_before_begin.cpp
		has_member_size.cpp
		index_type.cpp
		initializer_range.cpp
		insert.cpp
		integer_range.cpp
		is_container.cpp
		is_empty.cpp
		iter_difference_t.cpp
		iter_reference_t.cpp
		iter_value_t.cpp
		iterator.cpp
		iterator_addable.cpp
		iterator_category_base.cpp
		iterator_t.cpp
		iterator_to_sentinel.cpp
		key_comparison_function.cpp
		lazy_push_back.cpp
		lazy_push_back_into_capacity.cpp
		lazy_push_front.cpp
		legacy_append.cpp
		legacy_iterator.cpp
		linear_map.cpp
		linear_size.cpp
		linked_list_helper.cpp
		lookup.cpp
		map_tags.cpp
		map_value_type.cpp
		maximum_array_size.cpp
		member_assign.cpp
		member_lazy_push_backable.cpp
		member_push_backable.cpp
		mutable_iterator.cpp
		offset_type.cpp
		ordered_associative_container.cpp
		pop_back.cpp
		pop_front.cpp
		push_back.cpp
		push_back_into_capacity.cpp
		push_front.cpp
		random_access_iterator.cpp
		random_access_range.cpp
		random_access_sentinel_for.cpp
		range.cpp
		range_reference_t.cpp
		range_size_t.cpp
		range_value_t.cpp
		reallocation_size.cpp
		reference_or_value.cpp
		repeat_n.cpp
		reservable.cpp
		reserve_space_for.cpp
		resizable_container.cpp
		resize.cpp
		sentinel_for.cpp
		sentinel_t.cpp
		shrink_to_fit.cpp
		size.cpp
		size_then_use_range.cpp
		sized_range.cpp
		small_buffer_optimized_vector.cpp
		splicable.cpp
		stable_vector.cpp
		static_vector.cpp
		stored_function.cpp
		string.cpp
		subrange.cpp
		subtractable.cpp
		supports_lazy_insert_after.cpp
		take.cpp
		to_address.cpp
		to_any_string.cpp
		to_string.cpp
		trivial_inplace_function.cpp
		uninitialized_array.cpp
		uninitialized_dynamic_array.cpp
		value_wrapper.cpp
		vector.cpp
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

target_sources(containers_test PRIVATE
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		test/sort/chunked_insertion_sort.cpp
		test/sort/double_buffered_ska_sort.cpp
		test/sort/fixed_size_merge_sort.cpp
		test/sort/sort_test_data.cpp
		test/sort/ska_sort.cpp
		test/sort/sort.cpp
		test/sort/sort_exactly_n.cpp
		test/sort/test_sort.cpp
		test/algorithms/adjacent.cpp
		test/algorithms/binary_search.cpp
		test/algorithms/chunk_by.cpp
		test/algorithms/compare.cpp
		test/algorithms/count.cpp
		test/algorithms/concatenate_view.cpp
		test/algorithms/copy_or_relocate_from.cpp
		test/algorithms/erase.cpp
		test/algorithms/filter.cpp
		test/algorithms/find.cpp
		test/algorithms/join.cpp
		test/algorithms/join_with.cpp
		test/algorithms/keyed_binary_search.cpp
		test/algorithms/partition.cpp
		test/algorithms/set.cpp
		test/algorithms/split.cpp
		test/algorithms/uninitialized.cpp
		test/algorithms/zip.cpp
		test/append.cpp
		test/array.cpp
		test/assign.cpp
		test/assign_to_empty.cpp
		test/associative_container.cpp
		test/back.cpp
		test/bidirectional_linked_list.cpp
		test/clear.cpp
		test/concatenate.cpp
		test/dynamic_array.cpp
		test/flat_map.cpp
		test/forward_linked_list.cpp
		test/front.cpp
		test/is_container.cpp
		test/insert.cpp
		test/lazy_push_back.cpp
		test/lazy_push_back_into_capacity.cpp
		test/lazy_push_front.cpp
		test/linear_map.cpp
		test/pop_back.cpp
		test/push_back.cpp
		test/push_back_into_capacity.cpp
		test/push_front.cpp
		test/shrink_to_fit.cpp
		test/stable_vector.cpp
		test/subrange.cpp
		test/take.cpp
		test/test_associative_container.cpp
		test/test_reserve_and_capacity.cpp
		test/test_sequence_container.cpp
		test/test_set_size.cpp
		test/uninitialized_dynamic_array.cpp
		test/vector.cpp
)

target_sources(containers_test PRIVATE
	test/sort/to_radix_sort_key.cpp
	test/at.cpp
	test/small_buffer_optimized_vector.cpp
	test/static_vector.cpp
	test/string.cpp
	test/trivial_inplace_function.cpp
)

target_link_libraries(containers_test PRIVATE
	Catch2::Catch2WithMain
	containers
	strict_defaults
)
# https://github.com/catchorg/Catch2/issues/578
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(containers_test PRIVATE
		"-Wno-reserved-identifier"
	)
endif()

add_executable(flat_map
	test/map_benchmark.cpp
)
target_compile_definitions(flat_map PRIVATE "USE_FLAT_MAP")
target_link_libraries(flat_map PRIVATE
	containers
	strict_defaults
)

add_executable(std_map
	test/map_benchmark.cpp
)
target_compile_definitions(std_map PRIVATE "USE_SYSTEM_MAP")
target_link_libraries(std_map PRIVATE
	containers
	strict_defaults
)

add_executable(ska_sort_benchmark
	test/sort/ska_sort_benchmark.cpp
)
target_link_libraries(ska_sort_benchmark PRIVATE
	benchmark_main
	containers
	strict_defaults
)

add_executable(sort_benchmark
	test/sort/sort_benchmark.cpp
)
target_link_libraries(sort_benchmark PRIVATE
	benchmark_main
	containers
	strict_defaults
)

add_executable(vector_benchmark
	test/vector_benchmark.cpp
)
target_link_libraries(vector_benchmark PRIVATE
	benchmark_main
	containers
	strict_defaults
)

add_test(containers_test containers_test)
add_test(flat_map flat_map)
