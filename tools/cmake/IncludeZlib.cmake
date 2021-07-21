# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2021 Lucas Czech
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lczech@carnegiescience.edu>
# Department of Plant Biology, Carnegie Institution For Science
# 260 Panama Street, Stanford, CA 94305, USA

# ------------------------------------------------------------------------------
#   Use zlib
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# if zlib is available, and if so, set its paths and flags.

message (STATUS "Looking for zlib")
find_package(ZLIB)

# If found, set all needed compiler flags and also add those flags to the Genesis exported flags.
if(ZLIB_FOUND)
    message( STATUS "Found zlib: ${ZLIB_INCLUDE_DIRS} ${ZLIB_VERSION_STRING}" )
    message( STATUS "${ColorGreen}Using zlib${ColorEnd}" )

    # Include the headers etc
    include_directories( ${ZLIB_INCLUDE_DIRS} )

    # Set needed definition and linker flags for Genesis and for the parent scope.
    add_definitions( "-DGENESIS_ZLIB" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_ZLIB" )
    set( GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES} ${ZLIB_LIBRARIES} )

else()
    message( STATUS "${ColorRed}zlib not found${ColorEnd}" )
    message(
        STATUS
        "${ColorYellow}Try installing `zlib1g-dev` first, or the equivalent for your system."
        "To build without zlib support, call CMake with `-DGENESIS_USE_ZLIB=OFF`.${ColorEnd}"
    )
    message( FATAL_ERROR "Required package zlib not found.")
endif()
