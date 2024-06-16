# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2024 Lucas Czech
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
#   Test and include AVX
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# if AVX is available, and if so, set its paths and flags.

message (STATUS "Looking for AVX/AVX2")
find_package( AVX )

# Test AVX
if(AVX_FOUND)

    # message( STATUS "Found AVX: ${AVX_COMPILE_FLAGS}" )
    message (STATUS "${ColorGreen}Using AVX${ColorEnd}")

    # Set needed definition for Genesis and for the parent scope.
    add_definitions( "-DGENESIS_AVX" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_AVX" )

    # Set the flags needed for compiling Genesis.
    set( CMAKE_C_FLAGS            "${CMAKE_C_FLAGS}            ${AVX_COMPILE_FLAGS}")
    set( CMAKE_CXX_FLAGS          "${CMAKE_CXX_FLAGS}          ${AVX_COMPILE_FLAGS}" )
    # set( CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}   ${AVX_COMPILE_FLAGS}" )

    # Set the flags that are exported to parent scope
    # so that the parent can also compile using the those flags.
    set( GENESIS_C_FLAGS          "${GENESIS_C_FLAGS}          ${AVX_COMPILE_FLAGS}" )
    set( GENESIS_CXX_FLAGS        "${GENESIS_CXX_FLAGS}        ${AVX_COMPILE_FLAGS}" )
    # set( GENESIS_EXE_LINKER_FLAGS "${GENESIS_EXE_LINKER_FLAGS} ${AVX_COMPILE_FLAGS}" )

else()
    message( STATUS "AVX not found" )
endif()

# Test AVX2
if(AVX2_FOUND)

    # message( STATUS "Found AVX2: ${AVX2_COMPILE_FLAGS}" )
    message (STATUS "${ColorGreen}Using AVX2${ColorEnd}")

    # Set needed definition for Genesis and for the parent scope.
    add_definitions( "-DGENESIS_AVX2" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_AVX2" )

    # Set the flags needed for compiling Genesis.
    set( CMAKE_C_FLAGS            "${CMAKE_C_FLAGS}            ${AVX2_COMPILE_FLAGS}")
    set( CMAKE_CXX_FLAGS          "${CMAKE_CXX_FLAGS}          ${AVX2_COMPILE_FLAGS}" )
    # set( CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}   ${AVX2_COMPILE_FLAGS}" )

    # Set the flags that are exported to parent scope
    # so that the parent can also compile using the those flags.
    set( GENESIS_C_FLAGS          "${GENESIS_C_FLAGS}          ${AVX2_COMPILE_FLAGS}" )
    set( GENESIS_CXX_FLAGS        "${GENESIS_CXX_FLAGS}        ${AVX2_COMPILE_FLAGS}" )
    # set( GENESIS_EXE_LINKER_FLAGS "${GENESIS_EXE_LINKER_FLAGS} ${AVX2_COMPILE_FLAGS}" )

else()
    message( STATUS "AVX2 not found" )
endif()
