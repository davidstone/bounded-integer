# Copyright David Stone 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(bounded_integer LANGUAGES CXX)

enable_testing()

add_library(bounded INTERFACE)
add_library(containers INTERFACE)

target_sources(bounded INTERFACE
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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/common_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/comparison_operators.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/forward_declaration.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/optional.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/optional/value_or.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/all.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/clamp_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/modulo_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/null_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/policy/throw_policy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/base.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/get_index.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/is_valid_index.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/special_member_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/variadic_union.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/variant.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/variant/visit.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/abs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/assume.hpp
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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/copy_cv_ref.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/forward_declaration.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/hash.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/is_bounded_integer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/literal.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/log.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/max_builtin.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/make_index_sequence.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/minmax.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/noexcept.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/numeric_limits.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/overlapping_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/overload.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/requires.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/size_of.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/stream.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/string.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/tuple.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/typedefs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/underlying_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/assert.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/integer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/optional.hpp
)

target_sources(containers INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/accumulate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/advance.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/all_any_none.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/compare.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/concatenate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/concatenate_view.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/copy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/copy_n.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/count.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/distance.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/filter_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/find.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/maybe_find.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/minmax_element.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/move_destroy_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/move_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/negate.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/remove.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/reverse_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/set.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/sort.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/transform.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/transform_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/uninitialized.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/algorithms/unique.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/make_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/array/tuple.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/static_vector/make_static_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/static_vector/static_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/adapt.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/append.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/assign.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/at.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/begin_end.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/clear.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_container_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/common_iterator_functions.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/contiguous_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/data.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/dynamic_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/dynamic_resizable_array.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/empty.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/erase.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/fixed_capacity_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/flat_map.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/front_back.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/has_nested_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/index_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/insert.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/insert_emplace_impl.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/integer_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_container.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_iterator_sentinel.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/is_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/iterator_adapter.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/legacy_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/maximum_array_size.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/mutable_iterator.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/noexcept_iterable.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/operator_arrow.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/operator_bracket.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/push_back.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/range_view.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/reference_wrapper.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/repeat_n.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/reserve_if_reservable.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/resize.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/scope_guard.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/single_element_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/size.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/small_buffer_optimized_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/stable_vector.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/string.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/uninitialized_storage.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/containers/vector.hpp
)

target_include_directories(bounded INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)
target_include_directories(containers INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(containers INTERFACE
	bounded
)

add_executable(bounded_test
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
	source/bounded/detail/optional/common_type.cpp
	source/bounded/detail/optional/comparison_operators.cpp
	source/bounded/detail/optional/forward_declaration.cpp
	source/bounded/detail/optional/optional.cpp
	source/bounded/detail/optional/value_or.cpp
	source/bounded/detail/policy/all.cpp
	source/bounded/detail/policy/clamp_policy.cpp
	source/bounded/detail/policy/modulo_policy.cpp
	source/bounded/detail/policy/null_policy.cpp
	source/bounded/detail/policy/throw_policy.cpp
	source/bounded/detail/variant/visit.cpp
	source/bounded/detail/abs.cpp
	source/bounded/detail/assume.cpp
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
	source/bounded/detail/copy_cv_ref.cpp
	source/bounded/detail/forward_declaration.cpp
	source/bounded/detail/hash.cpp
	source/bounded/detail/int128.cpp
	source/bounded/detail/is_bounded_integer.cpp
	source/bounded/detail/literal.cpp
	source/bounded/detail/log.cpp
	source/bounded/detail/make_index_sequence.cpp
	source/bounded/detail/max_builtin.cpp
	source/bounded/detail/minmax.cpp
	source/bounded/detail/noexcept.cpp
	source/bounded/detail/numeric_limits.cpp
	source/bounded/detail/overlapping_range.cpp
	source/bounded/detail/overload.cpp
	source/bounded/detail/requires.cpp
	source/bounded/detail/size_of.cpp
	source/bounded/detail/stream.cpp
	source/bounded/detail/string.cpp
	source/bounded/detail/to_integer.cpp
	source/bounded/detail/tuple.cpp
	source/bounded/detail/type.cpp
	source/bounded/detail/typedefs.cpp
	source/bounded/detail/underlying_type.cpp
	source/bounded/assert.cpp
	source/bounded/assert-NDEBUG.cpp
	source/bounded/integer.cpp
	source/bounded/optional.cpp
)

target_link_libraries(bounded_test PUBLIC bounded)

add_library(containers_test STATIC
	source/containers/algorithms/accumulate.cpp
	source/containers/algorithms/advance.cpp
	source/containers/algorithms/all_any_none.cpp
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
	source/containers/empty.cpp
	source/containers/erase.cpp
	source/containers/fixed_capacity_vector.cpp
	source/containers/front_back.cpp
	source/containers/has_nested_type.cpp
	source/containers/insert.cpp
	source/containers/insert_emplace_impl.cpp
	source/containers/integer_range.cpp
	source/containers/is_container.cpp
	source/containers/is_iterator.cpp
	source/containers/is_iterator_sentinel.cpp
	source/containers/is_range.cpp
	source/containers/iterator_adapter.cpp
	source/containers/legacy_iterator.cpp
	source/containers/maximum_array_size.cpp
	source/containers/mutable_iterator.cpp
	source/containers/noexcept_iterable.cpp
	source/containers/operator_arrow.cpp
	source/containers/push_back.cpp
	source/containers/range_view.cpp
	source/containers/reference_wrapper.cpp
	source/containers/reserve_if_reservable.cpp
	source/containers/resize.cpp
	source/containers/scope_guard.cpp
	source/containers/single_element_range.cpp
	source/containers/size.cpp
)

target_link_libraries(containers_test PUBLIC containers)

add_executable(concatenate
	source/containers/algorithms/concatenate.cpp
)
target_link_libraries(concatenate PUBLIC containers)

add_executable(stable_vector
	source/containers/stable_vector.cpp
)
target_link_libraries(stable_vector PUBLIC containers)

add_executable(static_vector
	source/containers/static_vector/static_vector.cpp
)
target_link_libraries(static_vector PUBLIC containers)

add_executable(small_buffer_optimized_vector
	source/containers/small_buffer_optimized_vector.cpp
)
target_link_libraries(small_buffer_optimized_vector PUBLIC containers)

add_executable(flat_map
	source/containers/flat_map.cpp
)
target_link_libraries(flat_map PUBLIC containers)

add_executable(vector
	source/containers/vector.cpp
)
target_link_libraries(vector PUBLIC containers)

add_executable(string
	source/containers/string.cpp
)
target_link_libraries(string PUBLIC containers)

add_executable(dynamic_array
	source/containers/dynamic_array.cpp
)
target_link_libraries(dynamic_array PUBLIC containers)

add_executable(variant
	source/bounded/detail/variant/variant.cpp
)
target_link_libraries(variant PUBLIC bounded)


set(COMMON_CLANG_COMPILE_OPTIONS
	"-Weverything"
	"-Werror"
	"-Wno-c++98-compat"
	"-Wno-c++98-compat-pedantic"
	# Floating-point comparisons are safe in these tests
	"-Wno-float-equal"
	#
	"-Wno-inaccessible-base"
	# -Wmissing-braces wants two sets of braces when using constructs such
	# as std::array
	"-Wno-missing-braces"
	"-Wno-newline-eof"
	"-Wno-padded"
	# -Wunneeded-member-function has a bug that blocks constexpr libraries
	# from using it: https://llvm.org/bugs/show_bug.cgi?id=25084
	"-Wno-unneeded-member-function"
	# -Wweak-vtables is incompatible with header-only libraries
	"-Wno-weak-vtables"
	# -Wzero-as-null-pointer-constant does not work with the `operator<=>`
	# emulation
	"-Wno-zero-as-null-pointer-constant"
)

if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 8)
	set(COMMON_CLANG_COMPILE_OPTIONS
		${COMMON_CLANG_COMPILE_OPTIONS}
		"-Wno-ctad-maybe-unsupported"
	)
endif()

if(MSVC)
	target_compile_options(bounded INTERFACE
		"/std:c++latest"
		"/MP"
	)
	target_link_options(bounded INTERFACE
		"/WX"
	)

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		target_compile_options(bounded INTERFACE
			${COMMON_CLANG_COMPILE_OPTIONS}
		)
		# Sanitizers currently do not work very well on Windows
		set(USE_SANITIZERS OFF)
	else()
		target_compile_options(bounded INTERFACE
			"/FI ciso646"
		)
		set(USE_SANITIZERS OFF)
	endif()

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(bounded INTERFACE
		"-std=c++2a"
	)
	target_compile_options(bounded INTERFACE
		${COMMON_CLANG_COMPILE_OPTIONS}
		"-g"
		"-march=native"
	)

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(bounded INTERFACE
		"-std=c++2a"
	)
	target_compile_options(bounded INTERFACE
		"-g"
		"-march=native"
		"-fconcepts"
		# We have to start by turning off all warnings just so we can disable a
		# warning about an inaccessible base class. Then we turn on all
		# possible warnings. This means that some on-by-default warnings are
		# disabled with no way to re-enable. See
		# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90449.
		"-w"
		"-Waggressive-loop-optimizations"
		"-Wattribute-alias=2"
		"-Wattributes"
		"-Wattribute-warning"
		"-Wbuiltin-declaration-mismatch"
		"-Wbuiltin-macro-redefined"
		"-Wconversion-null"
		"-Wcoverage-mismatch"
		"-Wcpp"
		"-Wdeprecated"
		"-Wdeprecated-declarations"
		"-Wdiv-by-zero"
		"-Wendif-labels"
		"-Wfree-nonheap-object"
		"-Winherited-variadic-ctor"
		"-Wint-to-pointer-cast"
		"-Winvalid-offsetof"
		"-Wmultichar"
		"-Wodr"
		"-Woverflow"
		"-Wpragmas"
		"-Wprio-ctor-dtor"
		"-Wrestrict"
		"-Wreturn-local-addr"
		"-Wscalar-storage-order"
		"-Wunused-result"
		"-Wvariadic-macros"
		"-Wvirtual-move-assign"
		"-Wall"
		"-Wextra"
		"-Wpedantic"
		# Do not care about ABI differences
		# "-Wabi"
		"-Wcast-align"
		"-Wcast-qual"
		# It seems impossible to initialize bit fields with this warning on
		#"-Wconversion"
		"-Wctor-dtor-privacy"
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
		"-Wold-style-cast"
		"-Woverloaded-virtual"
		"-Wno-padded"
		"-Wredundant-decls"
		"-Wshadow"
		# -Wsign-compare warns in constexpr code for which comparison is safe.
		# Putting in the necessary casts to silence this warning would silence
		# the more useful warning on clang that warns if the value would
		# actually change.
		"-Wno-sign-compare"
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
		# -Wunused-but-set-variable does not properly account for
		# `if constexpr` branches.
		"-Wno-unused-but-set-variable"
		# -Wuseless-cast is incompatible with BOUNDED_INTEGER_CONDITIONAL
		# "-Wuseless-cast"
		"-Wvector-operation-performance"
		# -Wzero-as-null-pointer-constant does not work with the `operator<=>`
		# emulation
		# "-Wzero-as-null-pointer-constant"
		"-Werror"
	)
endif()

target_compile_definitions(flat_map PRIVATE "USE_FLAT_MAP")

set(test_targets bounded_test concatenate stable_vector static_vector small_buffer_optimized_vector flat_map vector string dynamic_array variant)
set(all_targets ${test_targets} containers_test)

set(sanitizers "-fsanitize=undefined" "-fsanitize=address")
foreach(target ${all_targets})
	if(DEFINED USE_SANITIZERS)
		if(${USE_SANITIZERS})
			target_compile_options(${target} PUBLIC ${sanitizers})
			target_link_options(${target} PUBLIC ${sanitizers})
		endif()
	else()
		target_compile_options(${target} PUBLIC
			$<$<CONFIG:Debug>:${sanitizers}>
		)
		target_link_options(${target} PUBLIC
			$<$<CONFIG:Debug>:${sanitizers}>
		)
	endif()
endforeach()

foreach(test_target ${test_targets})
	add_test(${test_target} ${test_target})
endforeach()
