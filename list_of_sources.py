# Copyright David Stone 2016.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

from program import prepend_dir, Program

source_directory = 'source'

include_directories = ['../bounded_integer/include', '../value_ptr', '../include']

# The sources are just needed to make sure the include files are self-contained.
# To minimize compile times, only compile these files with the array program,
# otherwise they keep getting compiled for various macros that have no meaning.
common_sources = [
	'algorithms/accumulate.cpp',
	'algorithms/advance.cpp',
	'algorithms/all_any_none.cpp',
	'algorithms/copy.cpp',
	'algorithms/count.cpp',
	'algorithms/distance.cpp',
	'algorithms/equal.cpp',
	'algorithms/find.cpp',
	'algorithms/filter_iterator.cpp',
	'algorithms/lexicographical_compare.cpp',
	'algorithms/move_destroy_iterator.cpp',
	'algorithms/move_iterator.cpp',
	'algorithms/negate.cpp',
	'algorithms/remove.cpp',
	'algorithms/reverse_iterator.cpp',
	'algorithms/unique.cpp',
	'add_remove_const.cpp',
	'allocator.cpp',
	'append.cpp',
	'assign.cpp',
	'at.cpp',
	'begin_end.cpp',
	'clear.cpp',
	'data.cpp',
	'empty.cpp',
	'erase.cpp',
	'front_back.cpp',
	'has_nested_type.cpp',
	'front_back.cpp',
	'insert.cpp',
	'is_container.cpp',
	'is_iterable.cpp',
	'is_iterator.cpp',
	'is_iterator_sentinel.cpp',
	'iterator_adapter.cpp',
	'legacy_iterator.cpp',
	'make_index_sequence.cpp',
	'mutable_iterator.cpp',
	'push_back.cpp',
	'resize.cpp',
	'scope_guard.cpp',
	'size.cpp',
	'tuple.cpp',
	'type.cpp',
]

flat_map_sources = [
	'flat_map/flat_map.cpp',
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
		sources = ['dynamic_array/dynamic_array.cpp'],
		include_directories = include_directories
	),
	Program(
		'static_vector',
		sources = [
			'static_vector/make_static_vector.cpp',
			'static_vector/static_vector.cpp',
		],
		include_directories = include_directories
	),
	Program(
		'vector',
		sources = ['vector/vector.cpp'],
		include_directories = include_directories
	),
	Program(
		'small_buffer_optimized_vector',
		sources = ['small_buffer_optimized_vector.cpp'],
		include_directories = include_directories
	),
	Program(
		'string',
		sources = ['string.cpp'],
		include_directories = include_directories
	),
	Program(
		'forward_list',
		sources = ['forward_list/forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=false'],
		include_directories = include_directories
	),
	Program(
		'forward_list_std',
		sources = ['forward_list/forward_list.cpp'],
		defines = ['USE_SYSTEM_FORWARD_LIST=true'],
		include_directories = include_directories
	),
	Program(
		'flat_map',
		sources = flat_map_sources,
		defines = ['USE_FLAT_MAP'],
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
container_types = ['deque', 'list', 'vector']
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
