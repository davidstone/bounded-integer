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

warnings = [
	'-Weverything',
	'-Werror',
	'-Wno-c++98-compat',
	'-Wno-c++98-compat-pedantic',
	'-Wno-exit-time-destructors',
	'-Wno-padded',
	'-Wno-switch-enum',
]
