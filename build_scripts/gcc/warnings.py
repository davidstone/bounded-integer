# Warnings if building with gcc
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


# -Wabi is not needed because I'm not combining binaries from different
# compilers.
#
# -Waggregate-return is not something that I consider an error. For instance,
# it triggers when using a range-based for loop on a vector of classes. Return
# value optimization should take care of any negative effects of this.
#
# -Weffc++ has a bug in the implementation as it relates to warning "X should
# be initialized in the member initialization list" for defaulted constructors
#
# -Winline is absent because I don't use the inline keyword for optimization
# purposes, just to define functions inline in headers. I don't care if the
# optimizer actually inlines it. This warning also complains if it can't inline
# a function declared in a class body (such as an empty virtual destructor).
#
# -Wmissing-format-attribute is not used because I do not use gnu extensions.
# Same for -Wsuggest-attribute and several others.
#
# -Wnormalized=nfc is already the default option, and looks to be the best.
#
# -Wpadded is turned on occasionally to optimize the layout of classes, but it
# is not left on because not all classes have enough elements to remove padding
# at the end. In theory I could get some extra variables for 'free', but it's
# not worth the extra effort of maintaining that (if my class size changes,
# it's not easy to remove those previously free variables).
#
# -Wstack-protector is not used because I do not use -fstack-protector.
#
# -Wstrict-aliasing=3 is turned on by -Wall and is the most accurate, but it
# looks like level 1 and 2 give more warnings. In theory a lower level is a
# 'stronger' warning, but it's at the cost of more false positives.
#
# -Wuseless-cast is incompatible with BOUNDED_INTEGER_CONDITIONAL

warnings = [
	'-Wall',
	'-Wextra',
	'-Wpedantic',
	'-Wcast-align',
	'-Wcast-qual',
	'-Wconversion',
	'-Wctor-dtor-privacy',
	'-Wdisabled-optimization',
	'-Wdouble-promotion',
#	'-Weffc++',
	'-Wfloat-equal',
	'-Wformat=2',
	'-Winit-self',
	'-Winvalid-pch',
	'-Wlogical-op',
	'-Wmissing-declarations',
	'-Wmissing-include-dirs',
	'-Wnoexcept',
	'-Wold-style-cast',
	'-Woverloaded-virtual',
	'-Wredundant-decls',
	'-Wshadow',
	'-Wsign-conversion',
	'-Wsign-promo',
	'-Wstrict-null-sentinel',
#	'-Wstrict-overflow=5',
	'-Wstrict-overflow=2',
	'-Wswitch-default',
	'-Wswitch-enum',
	'-Wtrampolines',
	'-Wundef',
	'-Wunsafe-loop-optimizations',
	'-Wvector-operation-performance',
	'-Wzero-as-null-pointer-constant',
	'-Werror',
]

warnings_debug = []
warnings_optimized = []
