# List of sources
# Copyright (C) 2015 David Stone
#
# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from program import prepend_dir, Program

source_directory = 'bounded_integer'

sources = [
	'array.cpp',
	'integer_range.cpp',
	'optional.cpp',
	'bounded_integer.cpp'
]
sources += prepend_dir('detail', [
	'algorithm.cpp',
	'cast.cpp',
	'class.cpp',
	'common_type.cpp',
	'common_type_and_value_category.cpp',
	'comparison.cpp',
	'comparison_builtin.cpp',
	'conditional.cpp',
	'forward_declaration.cpp',
	'has_nested_type.cpp',
	'hash.cpp',
	'integer_range.cpp',
	'is_bounded_integer.cpp',
	'iterator.cpp',
	'literal.cpp',
	'make.cpp',
	'math.cpp',
	'minmax.cpp',
	'noexcept.cpp',
	'numeric_limits.cpp',
	'overlapping_range.cpp',
	'requires.cpp',
	'stream.cpp',
	'string.cpp',
	'typedefs.cpp',
	'underlying_type.cpp',
])
sources += prepend_dir('detail/array', [
	'array.cpp',
	'iterator.cpp',
	'make_array.cpp',
])
sources += prepend_dir('detail/arithmetic', [
	'base.cpp',
	'plus.cpp',
	'minus.cpp',
	'multiplies.cpp',
	'divides.cpp',
	'modulus.cpp',
	'left_shift.cpp',
	'right_shift.cpp',
	'compound_assignment.cpp',
	'operators.cpp',
	'operators_builtin.cpp',
	'pointer.cpp',
])
sources += prepend_dir('detail/optional', [
	'comparison_operators.cpp',
	'make_optional_array.cpp',
	'optional.cpp',
	'specialization.cpp',
	'value_or.cpp',
])
sources += prepend_dir('detail/policy', [
	'all.cpp',
	'basic_policy.cpp',
	'clamp_policy.cpp',
	'common_policy.cpp',
	'is_overflow_policy.cpp',
	'modulo_policy.cpp',
	'null_policy.cpp',
	'throw_policy.cpp',
])


programs = [
	Program(
		'test',
		sources = sources
	)
]
