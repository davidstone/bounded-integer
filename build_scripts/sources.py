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

from program import prepend_dir, Program

source_directory = 'containers'

include_directories = ['../value_ptr']

flat_map_sources = ['flat_map.cpp', 'unique_inplace_merge.cpp']

programs = [
	Program(
		'forward_list',
		sources = ['forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=false'],
		include_directories = include_directories
	),
	Program(
		'forward_list_std',
		sources = ['forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=true'],
		include_directories = include_directories
	),
	Program(
		'moving_vector',
		sources = ['moving_vector.cpp'],
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
