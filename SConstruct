# SCons file
# Copyright (C) 2012 David Stone
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

import os
import multiprocessing

from build_scripts.sources import base_sources

SetOption('warn', 'no-duplicate-environment')

# Options to improve the default speed of SCons
SetOption('max_drift', 2)
SetOption('implicit_cache', 1)
SetOption('num_jobs', multiprocessing.cpu_count())

AddOption('--compiler', dest = 'compiler', type = 'string', action = 'store', help = 'Name of the compiler to use.')
AddOption('--compiler-command', dest = 'compiler_command', type = 'string', action = 'store', help = 'Command to launch the compiler.')

AddOption('--verbose', dest = 'verbose', action = "store_true", help = 'Print the full compiler output.')

Decider('MD5-timestamp')

SConscript('build_scripts/compiler_settings.py')
Import('flags', 'compiler_command', 'compiler_name')

default = DefaultEnvironment()

# This replaces the wall of text caused by compiling with max warnings turned on
# into something a little more readable.
if not GetOption('verbose'):
	default['CXXCOMSTR'] = 'Compiling $TARGET'
	default['LINKCOMSTR'] = 'Linking $TARGET'

default.Replace(CXX = compiler_command)

def setup_environment_flags(version):
	environment = default.Clone()
	environment.Append(CCFLAGS = flags['cc'][version])
	environment.Append(CXXFLAGS = flags['cxx'][version])
	environment.Append(LINKFLAGS = flags['link'][version])
	environment.Append(CPPDEFINES = flags['cpp'][version])
	if version != 'default':
		build_root = 'build/' + compiler_name + '/'
		environment.VariantDir(build_root + version, 'source', duplicate = 0)
	return environment

default = setup_environment_flags('default')
debug = setup_environment_flags('debug')
optimized = setup_environment_flags('optimized')

def generate_sources(sources, version, compiler_name):
	temp = []
	for source in sources:
		temp += ['build/' + compiler_name + '/' + version + '/' + source]
	return temp

def create_program(base):
	env = { 'debug':debug, 'optimized':optimized }
	suffix = { 'debug':'-debug', 'optimized':'' }
	name, sources, libraries = base
	for version in ['debug', 'optimized']:
		targets = generate_sources(sources, version, compiler_name)
		executable_name = name + suffix[version]
		env[version].Clone(LIBS = libraries).Program(executable_name, targets)

for sources in base_sources:
	create_program(sources)
