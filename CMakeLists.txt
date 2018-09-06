# Copyright David Stone 2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(bounded_integer LANGUAGES CXX)

enable_testing()

add_library(bounded INTERFACE)
add_library(containers INTERFACE)

target_sources(bounded INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/abs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/base.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/bitwise_and.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/common.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/divides.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/left_shift.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/minus.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/modulus.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/multiplies.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/operators_builtin.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/operators.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/plus.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/pointer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/right_shift.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/safe_abs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/unary_minus.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/arithmetic/unary_plus.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/basic_numeric_limits.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/cast.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/class.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/common_type_and_value_category.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/common_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison_function_object.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison_mixed.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/conditional.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/construct_destroy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/forward_declaration.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/hash.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/is_bounded_integer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/is_poisoned.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/literal.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/log.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/max_builtin.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/minmax.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/noexcept.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/numeric_limits.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/common_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/comparison_operators.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/forward_declaration.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/optional.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/value_or.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/overlapping_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/all.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/clamp_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/modulo_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/null_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/throw_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/requires.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/size_of.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/stream.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/string.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/strong_ordering.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/typedefs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/underlying_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/integer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/optional.hpp
)

target_sources(containers INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/accumulate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/all_any_none.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/concatenate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/concatenate_view.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/copy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/copy_n.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/count.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/distance.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/filter_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/find.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/move_destroy_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/move_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/negate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/remove.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/reverse_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/uninitialized.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/unique.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/allocator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/append.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/make_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/tuple.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/assign.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/at.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/begin_end.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/clear.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_container_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_iterator_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/data.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/dynamic_array/dynamic_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/dynamic_resizable_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/empty.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/erase.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/flat_map/flat_map.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/front_back.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/has_nested_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/index_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/insert.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/insert_emplace_impl.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/integer_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_container.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_iterable.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_iterator_sentinel.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/iterator_adapter.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/legacy_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/make_index_sequence.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/mutable_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/noexcept_iterable.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/operator_arrow.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/operator_bracket.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/push_back.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/range_view.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/repeat_n.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/reserve_if_reservable.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/resize.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/scope_guard.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/single_element_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/size.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/small_buffer_optimized_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/static_vector/make_static_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/static_vector/static_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/string.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/tuple.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/uninitialized_storage.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/base.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/get_index.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/is_valid_index.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/special_member_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/variadic_union.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/variant.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/variant/visit.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/vector/vector.hpp
)

target_include_directories(bounded INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)
target_include_directories(containers INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(BOUNDED_CXX_STD "/std:c++17")
else()
	set(BOUNDED_CXX_STD "-std=c++2a")
endif()

target_compile_options(bounded INTERFACE
	${BOUNDED_CXX_STD}
)
target_compile_options(containers INTERFACE
	${BOUNDED_CXX_STD}
)

target_link_libraries(containers INTERFACE
	bounded
)

add_executable(bounded_test
	source/bounded/detail/abs.cpp
	source/bounded/detail/arithmetic/base.cpp
	source/bounded/detail/arithmetic/bitwise_and.cpp
	source/bounded/detail/arithmetic/common.cpp
	source/bounded/detail/arithmetic/divides.cpp
	source/bounded/detail/arithmetic/left_shift.cpp
	source/bounded/detail/arithmetic/minus.cpp
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
	source/bounded/detail/basic_numeric_limits.cpp
	source/bounded/detail/cast.cpp
	source/bounded/detail/class.cpp
	source/bounded/detail/common_type_and_value_category.cpp
	source/bounded/detail/common_type.cpp
	source/bounded/detail/comparison.cpp
	source/bounded/detail/comparison_function_object.cpp
	source/bounded/detail/comparison_mixed.cpp
	source/bounded/detail/conditional.cpp
	source/bounded/detail/construct_destroy.cpp
	source/bounded/detail/forward_declaration.cpp
	source/bounded/detail/hash.cpp
	source/bounded/detail/is_bounded_integer.cpp
	source/bounded/detail/is_poisoned.cpp
	source/bounded/detail/literal.cpp
	source/bounded/detail/log.cpp
	source/bounded/detail/max_builtin.cpp
	source/bounded/detail/minmax.cpp
	source/bounded/detail/noexcept.cpp
	source/bounded/detail/numeric_limits.cpp
	source/bounded/detail/optional/common_type.cpp
	source/bounded/detail/optional/comparison_operators.cpp
	source/bounded/detail/optional/forward_declaration.cpp
	source/bounded/detail/optional/optional.cpp
	source/bounded/detail/optional/value_or.cpp
	source/bounded/detail/overlapping_range.cpp
	source/bounded/detail/policy/all.cpp
	source/bounded/detail/policy/clamp_policy.cpp
	source/bounded/detail/policy/modulo_policy.cpp
	source/bounded/detail/policy/null_policy.cpp
	source/bounded/detail/policy/throw_policy.cpp
	source/bounded/detail/requires.cpp
	source/bounded/detail/size_of.cpp
	source/bounded/detail/stream.cpp
	source/bounded/detail/string.cpp
	source/bounded/detail/strong_ordering.cpp
	source/bounded/detail/typedefs.cpp
	source/bounded/detail/underlying_type.cpp
	source/bounded/integer.cpp
	source/bounded/optional.cpp
)

target_link_libraries(bounded_test bounded)

add_library(containers_test STATIC
	source/containers/algorithms/accumulate.cpp
	source/containers/algorithms/all_any_none.cpp
	source/containers/algorithms/concatenate_view.cpp
	source/containers/algorithms/copy.cpp
	source/containers/algorithms/copy_n.cpp
	source/containers/algorithms/count.cpp
	source/containers/algorithms/distance.cpp
	source/containers/algorithms/filter_iterator.cpp
	source/containers/algorithms/find.cpp
	source/containers/algorithms/move_destroy_iterator.cpp
	source/containers/algorithms/move_iterator.cpp
	source/containers/algorithms/negate.cpp
	source/containers/algorithms/remove.cpp
	source/containers/algorithms/reverse_iterator.cpp
	source/containers/algorithms/uninitialized.cpp
	source/containers/algorithms/unique.cpp
	source/containers/allocator.cpp
	source/containers/append.cpp
	source/containers/array/array.cpp
	source/containers/array/iterator.cpp
	source/containers/array/make_array.cpp
	source/containers/array/tuple.cpp
	source/containers/assign.cpp
	source/containers/at.cpp
	source/containers/begin_end.cpp
	source/containers/clear.cpp
	source/containers/common_container_functions.cpp
	source/containers/data.cpp
	source/containers/empty.cpp
	source/containers/erase.cpp
	source/containers/front_back.cpp
	source/containers/has_nested_type.cpp
	source/containers/insert.cpp
	source/containers/insert_emplace_impl.cpp
	source/containers/integer_range.cpp
	source/containers/is_container.cpp
	source/containers/is_iterable.cpp
	source/containers/is_iterator.cpp
	source/containers/is_iterator_sentinel.cpp
	source/containers/iterator_adapter.cpp
	source/containers/legacy_iterator.cpp
	source/containers/make_index_sequence.cpp
	source/containers/mutable_iterator.cpp
	source/containers/noexcept_iterable.cpp
	source/containers/operator_arrow.cpp
	source/containers/push_back.cpp
	source/containers/range_view.cpp
	source/containers/reserve_if_reservable.cpp
	source/containers/resize.cpp
	source/containers/scope_guard.cpp
	source/containers/single_element_range.cpp
	source/containers/size.cpp
	source/containers/static_vector/make_static_vector.cpp
	source/containers/tuple.cpp
	source/containers/type.cpp
	source/containers/vector/vector.cpp
)

target_link_libraries(containers_test containers)

add_executable(concatenate
	source/containers/algorithms/concatenate.cpp
)
target_link_libraries(concatenate containers_test)

add_executable(static_vector
	source/containers/static_vector/static_vector.cpp
)
target_link_libraries(static_vector containers_test)

add_executable(small_buffer_optimized_vector
	source/containers/small_buffer_optimized_vector.cpp
)
target_link_libraries(small_buffer_optimized_vector containers_test)

add_executable(flat_map
	source/containers/flat_map/flat_map.cpp
)
target_link_libraries(flat_map containers_test)

add_executable(vector
	source/containers/vector/vector.cpp
)
target_link_libraries(vector containers_test)

add_executable(string
	source/containers/string.cpp
)
target_link_libraries(string containers_test)

add_executable(dynamic_array
	source/containers/dynamic_array/dynamic_array.cpp
)
target_link_libraries(dynamic_array containers_test)

add_executable(variant
	source/containers/variant/variant.cpp
)
target_link_libraries(variant containers_test)

function(set_sanitizers target)
	foreach(SANITIZER -fsanitize=undefined -fsanitize=address)
		target_compile_options(${target} PRIVATE
			$<$<CONFIG:Debug>:${SANITIZER}>
		)
		target_link_libraries(${target}
			$<$<CONFIG:Debug>:${SANITIZER}>
		)
	endforeach()
endfunction()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(COMPILE_OPTIONS
		$<$<CONFIG:Debug>:-O1;-march=native>
		$<$<CONFIG:Release>:-march=native>
		"-Weverything"
		"-Werror"
		"-Wno-c++98-compat"
		"-Wno-c++98-compat-pedantic"
		# Floating-point comparisons are safe in these tests
		"-Wno-float-equal"
		# -Wmissing-braces wants two sets of braces when using constructs such
		# as std::array
		"-Wno-missing-braces"
		"-Wno-newline-eof"
		"-Wno-padded"
		# -Wundefined-func-template has a bug that triggers on deduction guides
		"-Wno-undefined-func-template"
		# -Wunneeded-member-function has a bug that blocks constexpr libraries
		# from using it: https://llvm.org/bugs/show_bug.cgi?id=25084
		"-Wno-unneeded-member-function"
		# Has a bug that warns even for templates that are used
		"-Wno-unused-template"
		# -Wweak-vtables is incompatible with header-only libraries
		"-Wno-weak-vtables"
		# -Wzero-as-null-pointer-constant does not work with the `operator<=>`
		# emulation
		"-Wno-zero-as-null-pointer-constant"
	)
	target_compile_options(bounded_test PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(containers_test PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(concatenate PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(static_vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(small_buffer_optimized_vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(flat_map PRIVATE ${COMPILE_OPTIONS} "-DUSE_FLAT_MAP")
	target_compile_options(vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(string PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(dynamic_array PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(variant PRIVATE ${COMPILE_OPTIONS})

	set_sanitizers(bounded_test)
	set_sanitizers(containers_test)
	set_sanitizers(concatenate)
	set_sanitizers(static_vector)
	set_sanitizers(small_buffer_optimized_vector)
	set_sanitizers(flat_map)
	set_sanitizers(vector)
	set_sanitizers(string)
	set_sanitizers(dynamic_array)
	set_sanitizers(variant)

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(COMPILE_OPTIONS
		$<$<CONFIG:Debug>:-Og;-march=native>
		$<$<CONFIG:Release>:-O3;-march=native>
		"-Wall"
		"-Wextra"
		"-Wpedantic"
		"-Wabi"
		"-Wcast-align"
		"-Wcast-qual"
		# It seems impossible to initialize bit fields with this warning on
		#"-Wconversion"
		# -Wctor-dtor-privacy has a bug in 8.1:
		# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85792
		"-Wno-ctor-dtor-privacy"
		"-Wdisabled-optimization"
		"-Wdouble-promotion"
		# -Weffc++ includes a warning if all data members are not explicitly
		# initialized in the initializer list. I intentionally do not do this in
		# many cases. This would be more useful as a collection of warnings
		# like -Wall instead of a single warning on its own.
		# "-Weffc++"
		# -Wfloat-equal warns for safe comparisons
		# "-Wfloat-equal"
		"-Wformat=2"
		"-Winit-self"
		"-Winvalid-pch"
		# -Wlogical-op warns for expressions that happen to be equal in a
		# template instantiation
		# "-Wlogical-op"
		"-Wmissing-declarations"
		# -Wmissing-format-attribute is not used because I do not use GNU
		# extensions. Same for -Wsuggest-attribute and several others.
		"-Wmissing-include-dirs"
		#"-Wnoexcept"
		"-Wno-non-template-friend"
		"-Wodr"
		"-Wold-style-cast"
		"-Woverloaded-virtual"
		"-Wno-padded"
		"-Wredundant-decls"
		"-Wshadow"
		"-Wsign-conversion"
		# -Wsign-promo triggers on code that is guaranteed safe due to the use
		# of the bounded::integer library. Working around the warning would lead
		# to either less efficient code or more obfuscated code.
		"-Wsign-promo"
		# -Wsuggest-final-methods and -Wsuggest-final-types is a linker warning,
		# so it is not possible to disable it for boost and other third-party
		# libraries by saying they are system headers.
		# "-Wsuggest-final-methods"
		# "-Wsuggest-final-types"
		"-Wstrict-null-sentinel"
		# -Wstrict-overflow=2 warns about comparing two pointers
		"-Wstrict-overflow=1"
		"-Wswitch-default"
		"-Wswitch-enum"
		"-Wtrampolines"
		"-Wundef"
		# -Wunsafe-loop-optimizations causes too many spurious warnings. It may
		# be useful to apply this one periodically and manually verify the
		# results. It generated this warning in my code when I looped over all
		# elements in a vector to apply a set of functions to them (using the
		# range-based for loop).  It is also warning for the constructor of a
		# const array of const std::string where there is no loop in user code.
		# "-Wunsafe-loop-optimizations"
		# -Wunused-but-set-parameter does not properly account for
		# `if constexpr` branches.
		"-Wno-unused-but-set-parameter"
		# -Wuseless-cast is incompatible with BOUNDED_INTEGER_CONDITIONAL
		# "-Wuseless-cast"
		"-Wvector-operation-performance"
		# -Wzero-as-null-pointer-constant does not work with the `operator<=>`
		# emulation
		# "-Wzero-as-null-pointer-constant"
		"-Werror"
	)
	target_compile_options(bounded_test PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(containers_test PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(concatenate PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(static_vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(small_buffer_optimized_vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(flat_map PRIVATE ${COMPILE_OPTIONS} "-DUSE_FLAT_MAP")
	target_compile_options(vector PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(string PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(dynamic_array PRIVATE ${COMPILE_OPTIONS})
	target_compile_options(variant PRIVATE ${COMPILE_OPTIONS})

	set_sanitizers(bounded_test)
	set_sanitizers(containers_test)
	set_sanitizers(concatenate)
	set_sanitizers(static_vector)
	set_sanitizers(small_buffer_optimized_vector)
	set_sanitizers(flat_map)
	set_sanitizers(vector)
	set_sanitizers(string)
	set_sanitizers(dynamic_array)
	set_sanitizers(variant)
endif()

add_test(bounded_test bounded_test)
add_test(concatenate concatenate)
add_test(static_vector static_vector)
add_test(small_buffer_optimized_vector small_buffer_optimized_vector)
add_test(flat_map flat_map)
add_test(vector vector)
add_test(string string)
add_test(dynamic_array dynamic_array)
add_test(variant variant)

