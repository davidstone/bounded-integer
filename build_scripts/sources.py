# List of sources
# Copyright (C) 2013 David Stone
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

base_sources = [('test', ['optional.cpp', 'ranged_integer.cpp'] + prepend_dir('detail', [
	'arithmetic_operators.cpp',
	'arithmetic_result_type.cpp',
	'class.cpp',
	'common_policy.cpp',
	'common_type.cpp',
	'comparison_operators.cpp',
	'compound_assignment_operators.cpp',
	'enable_if.cpp',
	'forward_declaration.cpp',
	'is_ranged_integer.cpp',
	'literal.cpp',
	'make_ranged.cpp',
	'make_ranged_array.cpp',
	'make_ranged_optional.cpp',
	'make_ranged_optional_array.cpp',
	'math.cpp',
	'minmax.cpp',
	'numeric_limits.cpp',
	'operators.cpp',
	'optional.cpp',
	'optional_forward.cpp',
	'optional_relational_operators.cpp',
	'overlapping_range.cpp',
	'policy.cpp',
	'stream.cpp',
	'ternary_conditional.cpp',
	]) + 	prepend_dir('detail/policy', [
		'clamp_policy.cpp',
		'null_policy.cpp',
		'throw_policy.cpp',
	])
, [])]
