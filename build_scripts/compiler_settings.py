# Determine the correct settings for the compiler being used.
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

import re
import os

compiler_name = GetOption('compiler')
compiler_command = GetOption('compiler_command')

def extract_info(name, command):
	"""Get the compiler name from the command used to compile, and vice versa.
	
	This allows a user to specify that their compiler is named 'clang', and have
	SCons search in the normal directories for it, or the user can specify that
	their compiler is located at 'arbitrary/path/gcc' and SCons will assume that
	they are actually compiling with gcc. However, if the user has clang
	installed at 'path/g++', then the user must specify both the real name of
	the compiler and the path.
	"""
	if name == None:
		if command == None:
			name = DefaultEnvironment()['CXX']
		else:
			name = os.path.basename(command)
	if command == None:
		command = name
	return (name, command)

def normalize_name(compiler):
	"""Normalize multiple names into one canonical name. Example: g++ -> gcc"""
	search = re.match('[a-z+]+', compiler.lower())
	compiler = search.group(0)
	lookup = {}
	lookup['gcc'] = lookup['g++'] = 'gcc'
	lookup['clang'] = lookup['clang++'] = 'clang'
	return lookup[compiler]

compiler_name, compiler_command = extract_info(compiler_name, compiler_command)
compiler_name = normalize_name(compiler_name)

if compiler_name == 'gcc':
	from gcc.debug import debug
	from gcc.std import cxx_std
	from gcc.warnings import warnings
	from gcc.optimizations import optimize
elif compiler_name == 'clang':
	from clang.debug import debug
	from clang.std import cxx_std
	from clang.warnings import warnings
	from clang.optimizations import optimize

cc_flags = {
	'debug': warnings + debug.compile_flags,
	'release': warnings + debug.compile_flags_release
}
cxx_flags = {
	'debug': cxx_std,
	'release': cxx_std
}
link_flags = {
	'debug': warnings + cxx_std + debug.link_flags,
	'release': warnings + cxx_std + debug.link_flags_release + optimize.link_flags
}
cpp_flags = {
	'debug': [],
	'release': []
}

flags = {
	'cc': cc_flags,
	'cxx': cxx_flags,
	'link': link_flags,
	'cpp': cpp_flags
}

Export('flags', 'compiler_command', 'compiler_name')
