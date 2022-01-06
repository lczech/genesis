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

# --------------------------------------------------------------------------------------------------
#   CMake Init
# --------------------------------------------------------------------------------------------------

# Require the minimum version of any system that we test on - which is the version used by Travis.
# Thus, we can be sure that this works (as long as they don't change it...).
cmake_minimum_required (VERSION 2.8.7 FATAL_ERROR)

# Fun with colors!
if (NOT WIN32)
    string(ASCII 27 Esc)

    set(ColorBlue   "${Esc}[34m") # Build information.
    set(ColorGreen  "${Esc}[32m") # Optional additions.
    set(ColorYellow "${Esc}[33m") # Tipps for the user.
    set(ColorRed    "${Esc}[31m") # Warnings and Errors.
    set(ColorEnd    "${Esc}[0m")
endif()

# Some very first output, so that CMake projects which use us as a dependency cleary see where
# the Genesis configuration starts.
message (STATUS "${ColorBlue}Configuring Genesis${ColorEnd}")
message (STATUS "CMake version ${CMAKE_VERSION}")

# Important settings.
set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

# Include extra functionality.
include(ExternalProject)

# Include our own extra CMake functionality.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/tools/cmake" )
include( "${CMAKE_CURRENT_LIST_DIR}/tools/cmake/AssembleMonolith.cmake" )

# Need to make sure that AppleClang is actually recognized and not mistakingly identifyed
# as normal Clang. See https://cmake.org/cmake/help/v3.0/policy/CMP0025.html
# This is important for the test cmake script, where we need to set some warnings for clang,
# but not for apple clang.
# But, this policy was only introduced in CMake 3.0, which we hence need to test first;
# this test however is cumbersome, becuase VERSION_GREATER_EQUAL was only introduced in CMake 3.7,
# so we need to test the negated version...
if (NOT CMAKE_VERSION VERSION_LESS 3.0)
    # Appleclang compatibility
    cmake_policy(SET CMP0025 NEW)
    # Support for :: aliasing, used in new way to include OpenMP
    cmake_policy(SET CMP0028 NEW)
endif()

# --------------------------------------------------------------------------------------------------
#   Project Definitions and Variables
# --------------------------------------------------------------------------------------------------

# Set the project. We currently need "C" only because of the endianness test below...
project ( genesis CXX C )

# Set the file that needs to be used for `include_directories()`.
set( GENESIS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/lib" )

# Prepare an empty string for the list of definitions that Genesis exports to the CMake parent
# scope. Those can then be used when building programs that use Genesis. The definitions can be
# added in the parent CMake scope via `add_definitions( ${GENESIS_DEFINITIONS} )`, and are
# accessed in the Genesis source code via `#ifdef` macros.
set( GENESIS_DEFINITIONS "" )

# Prepare a variable that holds the default Genesis library to linked against.
# This is either the shared and static library.
set( GENESIS_LIBRARY "" )

# Prepare a variable that can be used to explicitly link against the Genesis shared library.
# This is exported to the parent scope, but only usable if GENESIS_BUILD_SHARED_LIB is ON.
set( GENESIS_SHARED_LIBRARY "" )

# Prepare a variable that can be used to explicitly link against the Genesis static library.
# This is exported to the parent scope, but only usable if GENESIS_BUILD_STATIC_LIB is ON.
set( GENESIS_STATIC_LIBRARY "" )

# Prepare variable that collects libraries that Genesis needs to be linked against, e.g. Pthreads.
# This is used internally, and also added to the Genesis library exports, so that in parent scope,
# only one target needs to be linked against.
set( GENESIS_INTERNAL_LINK_LIBRARIES "" )

# Prepare variables that collect compiler and linker flags, e.g. for OpenMP.
set( GENESIS_C_FLAGS "" )
set( GENESIS_CXX_FLAGS "" )
set( GENESIS_EXE_LINKER_FLAGS "" )

# Extract Genesis version from version source file.
file(
    STRINGS "${CMAKE_CURRENT_LIST_DIR}/lib/genesis/utils/core/version.hpp" GENESIS_VERSION_LINE
    REGEX "^.*#GENESIS_VERSION#.*$"
)
if( ${GENESIS_VERSION_LINE} MATCHES ".*\"(v.*)\".*" )
    set( GENESIS_VERSION "${CMAKE_MATCH_1}")
endif()
message (STATUS "OS: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message (STATUS "${ColorBlue}Genesis version: ${GENESIS_VERSION}${ColorEnd}")

# Figure out whether Genesis is compiled stand-alone or as a library/dependency.
# We use two complementary variables, because we want to use them as default for options
# (see later on), which do not accept a `NOT`...
# Also, we need to re-set GENESIS_HAS_PARENT_DIR to an actual bool value.
get_directory_property( GENESIS_HAS_PARENT_DIR PARENT_DIRECTORY )
if( GENESIS_HAS_PARENT_DIR )
    set( GENESIS_HAS_PARENT_DIR ON )
    set( GENESIS_IS_MASTER_PROJECT OFF )
    message (STATUS "Building Genesis as a dependency")
else()
    set( GENESIS_HAS_PARENT_DIR OFF )
    set( GENESIS_IS_MASTER_PROJECT ON )
    message (STATUS "Building Genesis stand-alone")
endif()

# --------------------------------------------------------------------------------------------------
#   Build Options
# --------------------------------------------------------------------------------------------------

# Options for which library version to build. Default: Build shared for stand-alone version,
# and build static when being used as a library.
option (GENESIS_BUILD_SHARED_LIB    "Build the shared library version of Genesis." ${GENESIS_IS_MASTER_PROJECT})
option (GENESIS_BUILD_STATIC_LIB    "Build the static library version of Genesis." ${GENESIS_HAS_PARENT_DIR})

# If we are the master project, we want to build the apps by default.
option (GENESIS_BUILD_APPLICATIONS  "Build Genesis applications." ${GENESIS_IS_MASTER_PROJECT})

# Currently, the Python module and tests are deactivated by default.
option (GENESIS_BUILD_PYTHON_MODULE "Build the Genesis python module."                   OFF)
option (GENESIS_BUILD_TESTS         "Build the Genesis test suites."                     OFF)

# If available, use external dependencies.
option (GENESIS_USE_ZLIB            "Use zlib."                                          ON)

# We default to using threads (if available, which will be checked later).
option (GENESIS_USE_THREADS         "Use a threading library (mainly, pthreads)."        ON)
option (GENESIS_USE_OPENMP          "Use OpenMP."                                        ON)

# Additional dependencies: htslib
# We use a locally installed (in the build directory) version of htslib,
# so that we can check out the exact commit that we want.
# By setting `GENESIS_USE_HTSLIB OFF`, all htslib-related functionality can also be deactivated,
# but those functions then won't be available in the library.
option (GENESIS_USE_HTSLIB          "Use htslib. See also GENESIS_SYSTEM_HTSLIB"         ON)

# Define unity build option with list of valid values. Default is full unity build.
set( GENESIS_UNITY_BUILD FULL CACHE STRING
    "Build modules monolithically - trading memory for speed. This also enables poor-mans LTO."
)
set_property( CACHE GENESIS_UNITY_BUILD PROPERTY STRINGS FULL MODULES OFF )

# --------------------------------------------------------------------------------------------------
#   Build Type
# --------------------------------------------------------------------------------------------------

# The build type is determined by the standard cmake variable CMAKE_BUILD_TYPE.
# Thus, if you want to change the build type when using genesis as a dependency,
# simply set this variable in your cmake script prior to the add_subdirectory call for genesis.
# We here only check that the variable is not completely empty, so that we have a definitive
# value at all times.
IF(CMAKE_BUILD_TYPE STREQUAL "")
    set (CMAKE_BUILD_TYPE RELEASE)
ENDIF()

# Using the environment variable `GENESIS_DEBUG`, it is possible to change from release mode
# (default) to debug mode, even without changing the option CMAKE_BUILD_TYPE from the outside.
# This is useful for developers, and can be activated for example in bash  via `export GENESIS_DEBUG=1`
# prior to calling CMake. It also activates test building automatically.
# Do not use this when genesis is used as a dependency. In that case, simply set CMAKE_BUILD_TYPE
# prior to including the genesis subdirectory.
if( GENESIS_IS_MASTER_PROJECT AND DEFINED ENV{GENESIS_DEBUG} AND "$ENV{GENESIS_DEBUG}" )

    set (CMAKE_BUILD_TYPE DEBUG)

    # set (CMAKE_VERBOSE_MAKEFILE ON)
    SET( GENESIS_BUILD_TESTS ON )
    SET( GENESIS_UNITY_BUILD OFF )

endif()

# Print build type. Useful for debugging user issues.
message (STATUS "${ColorBlue}Build type: ${CMAKE_BUILD_TYPE}${ColorEnd}")
message (STATUS "Unity build: ${GENESIS_UNITY_BUILD}")

# Helpful output. Compiler version information is unfortunately only available for CMake >= 2.8.9
message (STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} at ${CMAKE_CXX_COMPILER}")
message (STATUS "C compiler  : ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} at ${CMAKE_C_COMPILER}")

# --------------------------------------------------------------------------------------------------
#   Compiler and Linker Options
# --------------------------------------------------------------------------------------------------

if (MSVC OR (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
    # MinGW/MSVC do not support much. Also, the above string comparison in the condition does
    # not work there, hence the extra `MSVC` check...
    set (WARN_FLAGS "-Wall")
else ()
    # For other operating systems, set the warn flags to a very high level - except unknown pragmas,
    # as this causes needless warnings with OpenMP and other pragma-based techniques.
    set (WARN_FLAGS "-Wall -Wextra -Wno-unknown-pragmas -pedantic -pedantic-errors")

    # Furthermore, in debug mode, we also want all warnings to be errors, so that we do not miss them
    # In release, this might hinder users to successfully build genesis if any warnings remain
    # or occur only on their compiler, so let's avoid trouble for them.
    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        set (WARN_FLAGS "${WARN_FLAGS} -Werror")
    ENDIF()
endif ()

set (CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11 ${WARN_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O2 -DDEBUG -g -ggdb3 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Misc options
# STDCC   = clang++
# MPICC   = mpic++
# mpi: CCFLAGS += -DUSE_MPI
# LDFLAGS = -lm $(THREADS)

# Pre-compilation check for endianness
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if( IS_BIG_ENDIAN )
    message(
        STATUS
        "${ColorYellow}Your system uses big endian memory, which is rare. Genesis is not tested "
        "with such systems. There might be bugs because of this!${ColorEnd}"
    )
endif()

# Policy CMP0042: Enable MACOSX_RPATH
if (APPLE)
   set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)

# --------------------------------------------------------------------------------------------------
#   External Dependencies
# --------------------------------------------------------------------------------------------------

# ----------------------------------------------------------
#     zlib
# ----------------------------------------------------------

IF(GENESIS_USE_ZLIB)
    # Included from modules dir tools/cmake
    include( IncludeZlib )
ENDIF()

# ----------------------------------------------------------
#     (p)threads
# ----------------------------------------------------------

IF(GENESIS_USE_THREADS)
    # Included from modules dir tools/cmake
    include( IncludePthreads )
ENDIF()

# ----------------------------------------------------------
#     OpenMP
# ----------------------------------------------------------

IF(GENESIS_USE_OPENMP)
    # Included from modules dir tools/cmake
    include( IncludeOpenMP )
ENDIF()

# ----------------------------------------------------------
#     htslib
# ----------------------------------------------------------

if(GENESIS_USE_HTSLIB)

    # Store the commit hash that we want to check out for htslib.
    SET( htslib_COMMIT_HASH "bd133acf51498431a5c7dfd8aa06ce17ec6d3b96" ) #htslib_COMMIT_HASH#

    # Included from modules dir tools/cmake
    include( IncludeHtslib )

endif()

# --------------------------------------------------------------------------------------------------
#   Sub-Scripts
# --------------------------------------------------------------------------------------------------

# The applications, the python module and the tests all prefer the shared lib,
# so we need to build it if one of those is activated.
if( GENESIS_BUILD_APPLICATIONS OR GENESIS_BUILD_PYTHON_MODULE OR GENESIS_BUILD_TESTS )
    set (GENESIS_BUILD_SHARED_LIB ON)
endif()

# Using the build options from above, include the cmake sub-scripts
# that the user wants to compile.

if (GENESIS_BUILD_SHARED_LIB OR GENESIS_BUILD_STATIC_LIB)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib/genesis)
endif()

if (GENESIS_BUILD_APPLICATIONS)
    add_subdirectory(${PROJECT_SOURCE_DIR}/apps)
endif()

if (GENESIS_BUILD_PYTHON_MODULE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/python)
endif()

if (GENESIS_BUILD_TESTS)
    # Need to enable testing here, otherwise `./build/make test` will not work.
    enable_testing()

    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

# --------------------------------------------------------------------------------------------------
#   Export to parent scope
# --------------------------------------------------------------------------------------------------

# If Genesis is used as a dependency, we need to export definitions and stuff to the parent scope.
IF( GENESIS_HAS_PARENT_DIR )

    # Set the Genesis version. Can be used for compatibility checks.
    set( GENESIS_VERSION     ${GENESIS_VERSION}     PARENT_SCOPE )

    # Set the path to be used for `include_directories()` in the parent scope.
    set( GENESIS_INCLUDE_DIR ${GENESIS_INCLUDE_DIR} PARENT_SCOPE )

    # Set the definitions to be used for `add_definitions()` in the parent scope.
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} PARENT_SCOPE )

    # Set the compiler and linker flags to be added to the respective `CMAKE_` variables
    # in parent scope.
    set( GENESIS_C_FLAGS          ${GENESIS_C_FLAGS}          PARENT_SCOPE )
    set( GENESIS_CXX_FLAGS        ${GENESIS_CXX_FLAGS}        PARENT_SCOPE )
    set( GENESIS_EXE_LINKER_FLAGS ${GENESIS_EXE_LINKER_FLAGS} PARENT_SCOPE )

    # Set the libraries to be used for `target_link_libraries()` in the parent scope.
    # This is a combination of our own internal libraries (shared and static lib), as well as
    # any needed other external libraries, e.g., Pthreads.
    set( GENESIS_LINK_LIBRARIES        ${GENESIS_INTERNAL_LINK_LIBRARIES} ${GENESIS_LIBRARY} PARENT_SCOPE )

    # We also offer explicit targets for the shared and static library, in case both were build.
    # They also include our internal dependencies (Pthreads etc), just to be sure.
    set( GENESIS_LINK_SHARED_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES} ${GENESIS_SHARED_LIBRARY} PARENT_SCOPE )
    set( GENESIS_LINK_STATIC_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES} ${GENESIS_STATIC_LIBRARY} PARENT_SCOPE )

    # We also export the dependencies to the parent scope,
    # so that it can easily check whether those are available (and used by Genesis).
    if(ZLIB_FOUND)
        set( ZLIB_FOUND ${ZLIB_FOUND} PARENT_SCOPE )
    endif()
    if(THREADS_FOUND)
        set( THREADS_FOUND ${THREADS_FOUND} PARENT_SCOPE )
    endif()
    if(OPENMP_FOUND)
        set( OPENMP_FOUND ${OPENMP_FOUND} PARENT_SCOPE )
    endif()

ENDIF()

# --------------------------------------------------------------------------------------------------
#   Final user output
# --------------------------------------------------------------------------------------------------

# Cmake debug output, useful for user support as well. First, strip/trim extra spaces for nicer output
string(REGEX REPLACE " +" " " CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} " )
string(REGEX REPLACE " +" " " CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} " )
string(REGEX REPLACE " +" " " CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} " )
string(REGEX REPLACE " +" " " GENESIS_CXX_FLAGS " ${GENESIS_CXX_FLAGS} " )
string(REGEX REPLACE " +" " " GENESIS_EXE_LINKER_FLAGS " ${GENESIS_EXE_LINKER_FLAGS} " )
string(REGEX REPLACE " +" " " GENESIS_LINK_LIBRARIES " ${GENESIS_LINK_LIBRARIES} " )

message (STATUS "Final build flags:")
message (STATUS "CMAKE_CXX_FLAGS         : ${CMAKE_CXX_FLAGS}")
IF(CMAKE_BUILD_TYPE MATCHES "DEBUG")
    message (STATUS "CMAKE_CXX_FLAGS_DEBUG   : ${CMAKE_CXX_FLAGS_DEBUG}")
ELSE()
    message (STATUS "CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")
ENDIF()
message (STATUS "GENESIS_CXX_FLAGS       : ${GENESIS_CXX_FLAGS}")
message (STATUS "GENESIS_EXE_LINKER_FLAGS: ${GENESIS_EXE_LINKER_FLAGS}")
message (STATUS "GENESIS_LINK_LIBRARIES  : ${GENESIS_LINK_LIBRARIES}")

# Some very last output, so that CMake projects which use us as a dependency cleary see where
# the Genesis configuration ends.
message (STATUS "${ColorBlue}Finished configuring Genesis${ColorEnd}")
