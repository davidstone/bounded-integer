# Copyright David Stone 2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(bounded_integer LANGUAGES CXX)

find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
enable_testing()

add_library(bounded INTERFACE)

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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison_builtin.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison_function_object.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/comparison.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/conditional.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/construct_destroy.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/forward_declaration.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/hash.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/integer_range.hpp
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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/typedefs.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/detail/underlying_type.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/integer.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/integer_range.hpp
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/bounded/optional.hpp
)

target_include_directories(bounded INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(BOUNDED_CXX_STD "/std:c++17")
else()
	set(BOUNDED_CXX_STD "-std=c++17")
endif()

target_compile_options(bounded INTERFACE
	${BOUNDED_CXX_STD}
)

# Depend on a library that we defined in the top-level file
target_link_libraries(bounded INTERFACE
	boost
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
	source/bounded/detail/comparison_builtin.cpp
	source/bounded/detail/comparison.cpp
	source/bounded/detail/comparison_function_object.cpp
	source/bounded/detail/conditional.cpp
	source/bounded/detail/construct_destroy.cpp
	source/bounded/detail/forward_declaration.cpp
	source/bounded/detail/hash.cpp
	source/bounded/detail/integer_range.cpp
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
	source/bounded/detail/typedefs.cpp
	source/bounded/detail/underlying_type.cpp
	source/bounded/integer.cpp
	source/bounded/integer_range.cpp
	source/bounded/optional.cpp
)

target_link_libraries(bounded_test bounded)

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
	target_compile_options(bounded_test PRIVATE
		$<$<CONFIG:Debug>:-O1;-march=native>
		$<$<CONFIG:Release>:-march=native>
		"-Weverything"
		"-Werror"
		"-Wno-c++98-compat"
		"-Wno-c++98-compat-pedantic"
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
	set_sanitizers(bounded_test)

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(bounded_test PRIVATE
		$<$<CONFIG:Debug>:-Og;-march=native>
		$<$<CONFIG:Release>:-O3;-march=native>
		"-Wall"
		"-Wextra"
		"-Wpedantic"
		"-Wabi"
		"-Wcast-align"
		"-Wcast-qual"
		"-Wconversion"
		"-Wctor-dtor-privacy"
		"-Wdisabled-optimization"
		"-Wdouble-promotion"
		# -Weffc++ includes a warning if all data members are not explicitly
		# initialized in the initializer list. I intentionally do not do this in
		# many cases. This would be more useful as a collection of warnings
		# like -Wall instead of a single warning on its own.
		# "-Weffc++"
		"-Wfloat-equal"
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
		"-Wnoexcept"
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
		"-Wstrict-overflow=5"
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
		# -Wuseless-cast is incompatible with BOUNDED_INTEGER_CONDITIONAL
		# "-Wuseless-cast"
		"-Wvector-operation-performance"
		# -Wzero-as-null-pointer-constant does not work with the `operator<=>`
		# emulation
		# "-Wzero-as-null-pointer-constant"
		"-Werror"
	)
	set_sanitizers(bounded_test)
endif()

add_test(bounded_test bounded_test)
