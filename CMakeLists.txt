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

add_library(bounded_warnings INTERFACE)

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
	source/bounded/detail/builtin_min_max_value.cpp
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
	source/bounded/detail/int128.cpp
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
	source/bounded/detail/requires.cpp
	source/bounded/detail/returns.cpp
	source/bounded/detail/size_of.cpp
	source/bounded/detail/stream.cpp
	source/bounded/detail/tuple.cpp
	source/bounded/detail/type.cpp
	source/bounded/detail/typedefs.cpp
	source/bounded/detail/underlying_type.cpp
	source/bounded/assert.cpp
	source/bounded/assert-NDEBUG.cpp
	source/bounded/representation_digits.cpp
	source/bounded/hash.cpp
	source/bounded/integer.cpp
	source/bounded/numeric_limits.cpp
	source/bounded/optional.cpp
	source/bounded/string.cpp
	source/bounded/to_integer.cpp
)

target_link_libraries(bounded_test PUBLIC bounded bounded_warnings)

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
	source/containers/size.cpp
	source/containers/take.cpp
)

target_link_libraries(containers_test PUBLIC containers bounded_warnings)

add_executable(concatenate
	source/containers/algorithms/concatenate.cpp
)
target_link_libraries(concatenate PUBLIC containers bounded_warnings)

add_executable(stable_vector
	source/containers/stable_vector.cpp
)
target_link_libraries(stable_vector PUBLIC containers bounded_warnings)

add_executable(static_vector
	source/containers/static_vector/static_vector.cpp
)
target_link_libraries(static_vector PUBLIC containers bounded_warnings)

add_executable(small_buffer_optimized_vector
	source/containers/small_buffer_optimized_vector.cpp
)
target_link_libraries(small_buffer_optimized_vector PUBLIC containers bounded_warnings)

add_executable(flat_map
	source/containers/flat_map.cpp
)
target_compile_definitions(flat_map PRIVATE "USE_FLAT_MAP")
target_link_libraries(flat_map PUBLIC containers bounded_warnings)

add_executable(std_map
	source/containers/flat_map.cpp
)
target_compile_definitions(std_map PRIVATE "USE_SYSTEM_MAP")
target_link_libraries(std_map PUBLIC containers bounded_warnings)

add_executable(vector
	source/containers/vector.cpp
)
target_link_libraries(vector PUBLIC containers bounded_warnings)

add_executable(string
	source/containers/string.cpp
)
target_link_libraries(string PUBLIC containers bounded_warnings)

add_executable(dynamic_array
	source/containers/dynamic_array.cpp
)
target_link_libraries(dynamic_array PUBLIC containers bounded_warnings)

add_executable(variant
	source/bounded/detail/variant/variant.cpp
)
target_link_libraries(variant PUBLIC bounded bounded_warnings)

add_subdirectory("${PROJECT_SOURCE_DIR}/source/googletest-1.8.1")
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/source/benchmark")
set_target_properties(benchmark PROPERTIES FOLDER extern)

add_executable(ska_sort
	source/containers/algorithms/ska_sort.cpp
)
target_link_libraries(ska_sort PUBLIC bounded gtest gtest_main benchmark bounded_warnings)


set(COMMON_CLANG_COMPILE_OPTIONS
	"-Xclang"
	"-fconcepts-ts"
)
set(COMMON_CLANG_WARNINGS
	"-Weverything"
	"-Werror"
	"-Wno-c++98-compat"
	"-Wno-c++98-compat-pedantic"
	"-Wno-ctad-maybe-unsupported"
	# Floating-point comparisons are safe in these tests
	"-Wno-float-equal"
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
set(SKA_SORT_COMMON_COMPILE_OPTIONS
	"-Wno-global-constructors"
	"-Wno-implicit-int-conversion"
	"-Wno-sign-conversion"
	"-Wno-used-but-marked-unused"
	"-Wno-unused-function"
	"-Wno-unused-macros"
	"-Wno-unused-template"
	"-Wno-unused-variable"
)

if(MSVC)
	target_compile_options(bounded INTERFACE
		"/std:c++latest"
		"/MP"
	)
	target_link_options(bounded INTERFACE
		"/WX"
	)

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		target_compile_options(bounded INTERFACE ${COMMON_CLANG_COMPILE_OPTIONS})
		target_compile_options(bounded_warnings INTERFACE ${COMMON_CLANG_WARNINGS})
		target_compile_options(ska_sort PRIVATE ${SKA_SORT_COMMON_COMPILE_OPTIONS})
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
		${COMMON_CLANG_COMPILE_OPTIONS}
		"-std=c++2a"
		"-stdlib=libc++"
	)
	target_compile_options(bounded_warnings INTERFACE
		${COMMON_CLANG_WARNINGS}
		"-g"
		"-march=native"
		$<$<CONFIG:Release>:-flto=thin>
	)
	target_link_options(bounded INTERFACE
		"-Wl,--fatal-warnings"
		"-stdlib=libc++"
		"-fuse-ld=lld"
		# TODO: Figure out how to make this work on Visual Studio
		"-Wl,--thinlto-cache-dir=${CMAKE_BINARY_DIR}/lto-cache"
	)
	target_compile_options(gtest PUBLIC
		"-std=c++2a"
		"-stdlib=libc++"
	)
	target_compile_options(gtest_main PUBLIC
		"-std=c++2a"
		"-stdlib=libc++"
	)
	target_compile_options(benchmark PUBLIC
		"-std=c++2a"
		"-stdlib=libc++"
	)
	target_compile_options(ska_sort PRIVATE ${SKA_SORT_COMMON_COMPILE_OPTIONS})

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(bounded INTERFACE
		"-std=c++2a"
		"-fconcepts"
	)
	target_compile_options(bounded_warnings INTERFACE
		"-g"
		"-march=native"
		$<$<CONFIG:Release>:-flto=4 -flto-odr-type-merging -fuse-linker-plugin>
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

set(test_targets
	bounded_test
	concatenate
	stable_vector
	static_vector
	small_buffer_optimized_vector
	flat_map
	vector
	string
	dynamic_array
	variant
	ska_sort
)

if(DEFINED USE_SANITIZERS)
	if(${USE_SANITIZERS})
		target_compile_options(bounded_warnings PUBLIC ${sanitizers})
		target_link_options(bounded_warnings PUBLIC ${sanitizers})
	endif()
else()
	target_compile_options(bounded_warnings INTERFACE
		$<$<CONFIG:Debug>:${sanitizers}>
	)
	target_link_options(bounded_warnings INTERFACE
		$<$<CONFIG:Debug>:${sanitizers}>
	)
endif()

foreach(test_target ${test_targets})
	add_test(${test_target} ${test_target})
endforeach()
