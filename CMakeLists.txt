# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2017 Lucas Czech
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
# Lucas Czech <lucas.czech@h-its.org>
# Exelixis Lab, Heidelberg Institute for Theoretical Studies
# Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany

# ------------------------------------------------------------------------------
#   Cmake Init
# ------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

if (NOT WIN32)
    # Fun with colors!
    string(ASCII 27 Esc)
    set(ColorRed   "${Esc}[31m")
    set(ColorGreen "${Esc}[32m")
    set(ColorBlue  "${Esc}[34m")
    set(ColorEnd   "${Esc}[0m")
endif()

# ------------------------------------------------------------------------------
#   Project Definition
# ------------------------------------------------------------------------------

project ( genesis CXX C )

# set (genesis_VERSION_MAJOR 0)
# set (genesis_VERSION_MINOR 1)

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

option (BUILD_SHARED_LIB    "Build shared library" ON)
option (BUILD_STATIC_LIB    "Build static library" OFF)

option (BUILD_APPLICATIONS  "Build applications"   ON)
option (BUILD_PYTHON_MODULE "Build python module"  OFF)
option (BUILD_TESTS         "Build test suites"    ON)

option (USE_SHARED_BOOST    "Link against shared boost lib instead of static" ON)

option (USE_THREADS         "Use a threading library (mainly, pthreads)"      ON)
option (USE_OPENMP          "Use OpenMP if available"                         ON)

if (BUILD_APPLICATIONS OR BUILD_PYTHON_MODULE OR BUILD_TESTS)
    # The applications, the python module and the tests all use the shared lib,
    # so we need to build it if one of those is activated.
    set (BUILD_SHARED_LIB ON)
endif()

# ------------------------------------------------------------------------------
#   Build Type
# ------------------------------------------------------------------------------

set (CMAKE_BUILD_TYPE RELEASE)

# Using the environment variable `GENESIS_DEBUG`, it is possible to change from
# release mode (default) to debug mode. This is useful for developers.
if( $ENV{GENESIS_DEBUG} )
    set (CMAKE_BUILD_TYPE DEBUG)
    # set (CMAKE_VERBOSE_MAKEFILE ON)
endif()

message (STATUS "${ColorBlue}Building ${CMAKE_BUILD_TYPE}${ColorEnd}")

# ------------------------------------------------------------------------------
#   Compiler and Linker Options
# ------------------------------------------------------------------------------

set (WARN_FLAGS              "-Wall -Wextra -pedantic -pedantic-errors")
set (CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11 ${WARN_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG   "-O2 -g -ggdb3 -DDEBUG -D_GLIBCXX_DEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O2       -DNDEBUG")

# Misc options
# STDCC   = clang++
# MPICC   = mpic++
# mpi: CCFLAGS += -DUSE_MPI
# LDFLAGS = -lm $(THREADS)

# ------------------------------------------------------------------------------
#   Threads and OpenMP
# ------------------------------------------------------------------------------

# We check whether to use threads and OpenMP here,
# so that all binaries are compiled with the same settings.

IF(USE_THREADS)
    set (CMAKE_THREAD_PREFER_PTHREAD ON)
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads)
ENDIF()

if(THREADS_FOUND)
    message (STATUS "${ColorGreen}Using threads${ColorEnd}")
    add_definitions( "-DGENESIS_PTHREADS" )
endif()

IF(USE_OPENMP)
    find_package( OpenMP )
ENDIF()

if(OPENMP_FOUND)
    message (STATUS "${ColorGreen}Using OpenMP${ColorEnd}")
    add_definitions( "-DGENESIS_OPENMP" )

    set( CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${OpenMP_C_FLAGS}")
    set( CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${OpenMP_CXX_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
endif()

# ------------------------------------------------------------------------------
#   Sub-Scripts
# ------------------------------------------------------------------------------

# Using the build options from above, include the cmake sub-scripts
# that the user wants to compile.

if (BUILD_SHARED_LIB OR BUILD_STATIC_LIB)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
endif()

if (BUILD_APPLICATIONS)
    add_subdirectory(${PROJECT_SOURCE_DIR}/apps)
endif()

if (BUILD_PYTHON_MODULE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/python/src)
endif()

if (BUILD_TESTS)
    # Need to enable testing here, otherwise `./build/make test` will not work.
    enable_testing()

    add_subdirectory(${PROJECT_SOURCE_DIR}/test/src)
endif()
