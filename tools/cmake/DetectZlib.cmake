# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2025 Lucas Czech
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
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

# ------------------------------------------------------------------------------
#   Use zlib
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# if zlib is available, and if so, set its paths and flags.

message (STATUS "Looking for zlib")
find_package(ZLIB)

# Some HPC module systems (e.g. EasyBuild) only expose a library via PKG_CONFIG_PATH
# and not via CMAKE_PREFIX_PATH/<Pkg>_ROOT, which is what find_package(ZLIB) relies on.
# Fall back to pkg-config in that case, so we still find it.
if(NOT ZLIB_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(PC_ZLIB QUIET zlib)
        if(PC_ZLIB_FOUND)
            find_library(
                ZLIB_LIBRARIES
                NAMES ${PC_ZLIB_LIBRARIES}
                HINTS ${PC_ZLIB_LIBRARY_DIRS}
            )
            if(ZLIB_LIBRARIES)
                set(ZLIB_FOUND TRUE)
                set(ZLIB_INCLUDE_DIRS ${PC_ZLIB_INCLUDE_DIRS})
                set(ZLIB_VERSION_STRING ${PC_ZLIB_VERSION})
            endif()
        endif()
    endif()
endif()

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
        "${ColorYellow}Try installing `zlib1g-dev` first, or the equivalent for your system. "
        "To build without zlib support, call CMake with `-DGENESIS_USE_ZLIB=OFF`.${ColorEnd}"
    )
    message( FATAL_ERROR "Required package zlib not found.")
endif()
