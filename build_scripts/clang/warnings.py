# Warnings if building with clang
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

# -Wno-c++98-compat is used because I do not care about being compatible with
# old versions of the standard. I use -Wno-c++98-compat-pedantic because I still
# do not care.
#
# -Wno-exit-time-destructors warns about any static variable that has a
# destructor. I use a few static const variables, but they do not depend on each
# other for their destruction (or any global variable), so my usage is safe.
#
# -Wimplicit-fallthrough must be disabled because it warns about an empty case
# followed by another case (in other words, giving multiple cases identical
# actions).
#
# -Wmismatched-tags is not disabled for two reasons:
# 1) It is turned on by default at a low warning level, and I don't want clang
# warning about my project for everyone else
# 2) It could possibly cause linker problems due to name mangling issues
#
# -Wmissing-braces is incompatible with the implementation of make_array. An
# implementation that doesn't run afoul of this warning ended up being very slow
# and memory intensive, so it seems that disabling the warning is the better
# option.
#
# -Wpadded is turned on occasionally to optimize the layout of classes, but it
# is not left on because not all classes have enough elements to remove padding
# at the end. In theory I could get some extra variables for 'free', but it's
# not worth the extra effort of maintaining that (if my class size changes,
# it's not easy to remove those previously free variables).
#
# -Wswitch-enum isn't behavior that I want. I don't want to handle every switch
# statement explicitly. It would be useful if the language had some mechanism
# to activate this on specified switch statements (to ensure that future
# changes to the enum are handled everywhere that they need to be), but it's
# overkill for an "all-or-nothing" setting.
#

warnings = ['-Weverything', '-Werror', '-Wno-c++98-compat', '-Wno-c++98-compat-pedantic', '-Wno-exit-time-destructors', '-Wno-implicit-fallthrough', '-Wno-missing-braces', '-Wno-padded', '-Wno-switch-enum']
warnings_debug = []
warnings_optimized = []
