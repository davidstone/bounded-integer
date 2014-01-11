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

class Program:
	def __init__(self, name, sources, defines = [], libraries = []):
		self.name = name
		self.sources = sources
		self.defines = defines
		self.libraries = libraries

forward_list = Program('forward_list', ['forward_list.cpp'], ['USE_SYSTEM_FORWARD_LIST=false'])
forward_list_std = Program('forward_list_std', ['forward_list.cpp'], ['USE_SYSTEM_FORWARD_LIST=true'])
moving_vector = Program('moving_vector', ['moving_vector.cpp'])
stable_flat_map = Program('flat_map_stable', ['flat_map.cpp'], ['USE_STABLE_FLAT_MAP'])
unstable_flat_map = Program('flat_map_unstable', ['flat_map.cpp'], ['USE_UNSTABLE_FLAT_MAP'])
flat_map_std = Program('flat_map_std', ['flat_map.cpp'], ['USE_SYSTEM_MAP'])

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
# array_sizes = ['1', '10', '40', '50', '60', '100', '200', '500', '1000']
array_sizes = ['100']
performance_tests = [Program('performance_' + container + '_' + reserve + '_' + size, performance_sources, [container.upper(), reserve, 'ARRAY_SIZE=' + size]) for container in container_types for size in array_sizes for reserve in reserve_options(container)]

source_directory = 'containers'

programs = [forward_list, forward_list_std, moving_vector, flat_map_std, unstable_flat_map, stable_flat_map] + performance_tests

include_directories = ['value_ptr']
