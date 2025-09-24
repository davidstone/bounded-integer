# Copyright David Stone 2024.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

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
		arithmetic/bitwise_and.cpp
		arithmetic/byte.cpp
		arithmetic/divides.cpp
		arithmetic/extreme_values.cpp
		arithmetic/left_shift.cpp
		arithmetic/increment_decrement.cpp
		arithmetic/modulus.cpp
		arithmetic/multiplies.cpp
		arithmetic/min_max.cpp
		arithmetic/operators_builtin.cpp
		arithmetic/operators.cpp
		arithmetic/plus.cpp
		arithmetic/pointer.cpp
		arithmetic/right_shift.cpp
		arithmetic/round_up_divide.cpp
		arithmetic/safe_abs.cpp
		arithmetic/unary_minus.cpp
		arithmetic/unary_plus.cpp
		abs.cpp
		add_common_cv_reference.cpp
		bounded.cpp
		bounded_integer.cpp
		builtin_integer.cpp
		builtin_min_max_value.cpp
		cast.cpp
		character.cpp
		check_in_range.cpp
		checked_bit_cast.cpp
		clamp.cpp
		common_type_and_value_category.cpp
		comparison.cpp
		comparison_builtin.cpp
		comparison_function_object.cpp
		concepts.cpp
		conditional_function.cpp
		constexpr_only_if.cpp
		construct.cpp
		construct_at.cpp
		copy.cpp
		declval.cpp
		destroy.cpp
		equality_comparable.cpp
		function_ptr.cpp
		hash.cpp
		homogeneous_equals.cpp
		index_sequence_struct.cpp
		integer.cpp
		integral.cpp
		integral_constant_of_integral.cpp
		is_bounded_integer.cpp
		isomorphic_to_integral.cpp
		lazy_init.cpp
		literal.cpp
		log.cpp
		make_index_sequence.cpp
		minmax.cpp
		non_const.cpp
		normalize.cpp
		number_of.cpp
		numeric_limits.cpp
		ordered.cpp
		pow.cpp
		prevent_comma.cpp
		relocate.cpp
		representation_digits.cpp
		safe_compare.cpp
		safe_equal.cpp
		safe_extreme.cpp
		scope_fail.cpp
		scope_guard.cpp
		signed_builtin.cpp
		size_of.cpp
		std_iterator.cpp
		stream.cpp
		test_int.cpp
		to_integer.cpp
		tombstone.cpp
		type.cpp
		unchecked.cpp
		underlying_type_t.cpp
		unsigned_builtin.cpp
		value_fits_in_type.cpp
		value_to_function.cpp
)

# Until resolution of https://github.com/llvm/llvm-project/issues/60089
set_source_files_properties(arithmetic/increment_decrement.cpp
	PROPERTIES COMPILE_FLAGS "-Wno-implicit-int-conversion -Wno-shorten-64-to-32"
)
set_source_files_properties(arithmetic/operators_builtin.cpp
	PROPERTIES COMPILE_FLAGS "-Wno-implicit-int-conversion -Wno-shorten-64-to-32"
)

add_subdirectory(arithmetic/test)

add_executable(bounded_test)

target_sources(bounded_test PRIVATE
	FILE_SET CXX_MODULES
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		test/assert.cpp
		test/assert-NDEBUG.cpp
		test/check_in_range.cpp
		test/comparison.cpp
		test/conditional.cpp
		test/stream.cpp
		test/to_integer.cpp
		test/tombstone.cpp
)

target_link_libraries(bounded_test PUBLIC
	Catch2::Catch2WithMain
	bounded
	strict_defaults
)

add_test(bounded_test bounded_test)
