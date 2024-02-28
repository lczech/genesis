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
# Lucas Czech <lczech@carnegiescience.edu>
# Department of Plant Biology, Carnegie Institution For Science
# 260 Panama Street, Stanford, CA 94305, USA

# ------------------------------------------------------------------------------
#   Use PThreads
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# if Pthreads are available, and if so, set its paths and flags.
# We curently do not use pthreads internally any more, as we have switched to just using
# the plain C++11 threading model instead. However, they seem to be needed for properly
# linking htslib... So we have to keep this around.

message (STATUS "Looking for Threads")
set (CMAKE_THREAD_PREFER_PTHREAD ON)
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads)

# If found, set all needed compiler flags and also add those flags to the Genesis exported flags.
if(THREADS_FOUND)
    message( STATUS "Found Threads: ${CMAKE_THREAD_LIBS_INIT}" )
    message( STATUS "${ColorGreen}Using Threads${ColorEnd}" )

    # Set needed definition and linker flags for Genesis and for the parent scope.
    add_definitions( "-DGENESIS_PTHREADS" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_PTHREADS" )
    set( GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

else()
    message( STATUS "${ColorRed}Threads not found${ColorEnd}" )
    message( STATUS "To build without threading support, call CMake with -DGENESIS_USE_THREADS=OFF" )
    message( FATAL_ERROR "Required package threads not found.")
endif()
