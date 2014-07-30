# List of sources
# Copyright (C) 2014 David Stone
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

def prepend_dir(directory, sources):
	"""Remove redundant specification of a directory for multiple sources"""
	return map(lambda source: directory + '/' + source, sources)

# Example of use:
#
# example_sources = prepend_dir('cool', ['alright.cpp', 'awesome.cpp'])
# example_sources += prepend_dir('yeah', ['no.cpp', 'maybe.cpp'])
# example_libraries = ['boost_filesystem']
#
# example = ('example_program_name', example_sources, example_libraries)
# base_sources = [example]

base_sources = [('test',
	prepend_dir('detail', [
		'algorithm.cpp',
		'class.cpp',
		'common_policy.cpp',
		'common_type.cpp',
		'conditional.cpp',
		'div.cpp',
		'enable_if.cpp',
		'forward_declaration.cpp',
		'integer_range.cpp',
		'is_bounded_integer.cpp',
		'iterator.cpp',
		'literal.cpp',
		'make.cpp',
		'math.cpp',
		'minmax.cpp',
		'numeric_limits.cpp',
		'overlapping_range.cpp',
		'stream.cpp',
		'string.cpp',
		'typedefs.cpp',
		'underlying_type.cpp',
	]) +
	prepend_dir('detail/array', [
		'array.cpp',
		'make_array.cpp',
	]) +
	prepend_dir('detail/operators', [
		'arithmetic.cpp',
		'arithmetic_result_type.cpp',
		'comparison.cpp',
		'compound_assignment.cpp',
		'operators.cpp',
	]) +
	prepend_dir('detail/optional', [
		'forward_declaration.cpp',
		'make_optional_array.cpp',
		'optional.cpp',
		'comparison_operators.cpp',
	]) +
	prepend_dir('detail/policy', [
		'all.cpp',
		'basic_policy.cpp',
		'clamp_policy.cpp',
		'dynamic_policy.cpp',
		'is_overflow_policy.cpp',
		'null_policy.cpp',
		'throw_policy.cpp',
	]) +
	['array.cpp', 'integer_range.cpp', 'optional.cpp', 'bounded_integer.cpp']
, [])]
