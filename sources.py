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

source_directory = 'source'

include_directories = ['../bounded_integer', '../value_ptr', '../include']

common_sources = [
	'addressof.cpp',
	'allocator.cpp',
	'has_nested_type.cpp',
	'is_container.cpp',
	'is_iterator.cpp',
	'is_iterator_sentinel.cpp',
	'iterator_adapter.cpp',
	'legacy_iterator.cpp',
	'type_list.cpp',
	'algorithms/accumulate.cpp',
	'algorithms/all_any_none.cpp',
	'algorithms/count.cpp',
	'algorithms/iterator.cpp',
	'algorithms/negate.cpp',
	'algorithms/remove.cpp',
]

flat_map_sources = common_sources + [
	'flat_map/flat_map.cpp',
	'algorithms/find.cpp',
	'algorithms/unique.cpp',
]

programs = [
	Program(
		'array',
		sources = common_sources + [
			'array/array.cpp',
			'array/iterator.cpp',
			'array/make_array.cpp',
			'array/tuple.cpp',
		],
		include_directories = include_directories
	),
	Program(
		'dynamic_array',
		sources = common_sources + ['dynamic_array/dynamic_array.cpp'],
		include_directories = include_directories
	),
	Program(
		'static_vector',
		sources = common_sources + [
			'static_vector/make_static_vector.cpp',
			'static_vector/static_vector.cpp',
		],
		include_directories = include_directories
	),
	Program(
		'vector',
		sources = common_sources + ['vector/vector.cpp'],
		include_directories = include_directories
	),
	Program(
		'forward_list',
		sources = common_sources + ['forward_list/forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=false'],
		include_directories = include_directories
	),
	Program(
		'forward_list_std',
		sources = common_sources + ['forward_list/forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=true'],
		include_directories = include_directories
	),
	Program(
		'moving_vector',
		sources = common_sources + ['moving_vector/moving_vector.cpp'],
		include_directories = include_directories
	),
	Program(
		'flat_map_stable',
		sources = flat_map_sources,
		defines = ['USE_STABLE_FLAT_MAP'],
		include_directories = include_directories
	),
	Program(
		'flat_map_unstable',
		sources = flat_map_sources,
		defines = ['USE_UNSTABLE_FLAT_MAP'],
		include_directories = include_directories
	),
	Program(
		'flat_map_std',
		sources = flat_map_sources,
		defines = ['USE_SYSTEM_MAP'],
		include_directories = include_directories
	),
]

def reserve_options(container):
	# reserve doesn't work properly in gcc 4.7
	return ['NO_RESERVE']
	if container != 'deque' and container != 'list':
		return ['RESERVE', 'NO_RESERVE']
	else:
		return ['NO_RESERVE']

performance_sources = ['performance_test_sequence.cpp']
container_types = ['deque', 'list', 'moving_vector', 'vector']
# Don't keep compiling all of these other versions that I rarely use
array_sizes = [
#	'1',
#	'10',
#	'40',
#	'50',
#	'60',
	'100',
#	'200',
#	'500',
#	'1000',
]
performance_tests = [
	Program(
		'performance_' + container + '_' + reserve + '_' + size,
		sources = performance_sources,
		defines = [container.upper(), reserve, 'ARRAY_SIZE=' + size],
		include_directories = include_directories
	) for container in container_types for size in array_sizes for reserve in reserve_options(container)
]

programs += performance_tests
