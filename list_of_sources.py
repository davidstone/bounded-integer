# Copyright David Stone 2015.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

from program import prepend_dir, Program

source_directory = 'source/bounded'
include_directories = ['../include']

sources = [
	'integer.cpp',
	'integer_range.cpp',
	'optional.cpp',
]
sources += prepend_dir('detail', [
	'abs.cpp',
	'addressof.cpp',
	'basic_numeric_limits.cpp',
	'cast.cpp',
	'class.cpp',
	'common_type.cpp',
	'common_type_and_value_category.cpp',
	'comparison.cpp',
	'comparison_builtin.cpp',
	'comparison_function_object.cpp',
	'conditional.cpp',
	'construct_destroy.cpp',
	'forward_declaration.cpp',
	'hash.cpp',
	'integer_range.cpp',
	'is_bounded_integer.cpp',
	'literal.cpp',
	'log.cpp',
	'make.cpp',
	'minmax.cpp',
	'noexcept.cpp',
	'numeric_limits.cpp',
	'overlapping_range.cpp',
	'requires.cpp',
	'size_of.cpp',
	'stream.cpp',
	'string.cpp',
	'typedefs.cpp',
	'underlying_type.cpp',
])
sources += prepend_dir('detail/arithmetic', [
	'base.cpp',
	'bitwise_and.cpp',
	'plus.cpp',
	'minus.cpp',
	'multiplies.cpp',
	'divides.cpp',
	'modulus.cpp',
	'left_shift.cpp',
	'right_shift.cpp',
	'common.cpp',
	'operators.cpp',
	'operators_builtin.cpp',
	'pointer.cpp',
])
sources += prepend_dir('detail/optional', [
	'common_type.cpp',
	'comparison_operators.cpp',
	'forward_declaration.cpp',
	'optional.cpp',
	'value_or.cpp',
])
sources += prepend_dir('detail/policy', [
	'all.cpp',
	'clamp_policy.cpp',
	'modulo_policy.cpp',
	'null_policy.cpp',
	'throw_policy.cpp',
])


programs = [
	Program(
		'test',
		sources = sources,
		include_directories = include_directories
	)
]
