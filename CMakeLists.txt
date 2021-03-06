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
    cmake_policy(SET CMP0025 NEW)
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
# By default, we use a locally installed (in the build directory) version of htslib,
# so that we can check out the exact commit that we want. We also allow to use the system-wide
# installation by setting `GENESIS_SYSTEM_HTSLIB ON`(if set, we assume that htslib is present and
# in a compatible version - we do not check this, but the compilation will just fail if it is not).
# Also, by setting `GENESIS_USE_HTSLIB OFF`, all htslib-related functionality can also be deactivated,
# but those functions then won't be available in the library.
option (GENESIS_USE_HTSLIB          "Use htslib. See also GENESIS_SYSTEM_HTSLIB"         ON)
option (GENESIS_SYSTEM_HTSLIB       "If set, use the system-wide installation of htslib" OFF)
set(HTSLIB_HINT "" CACHE STRING     "Path to htslib if GENESIS_SYSTEM_HTSLIB=ON")

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
ENDIF()

# ----------------------------------------------------------
#     (p)threads
# ----------------------------------------------------------

# We check whether to use threads and OpenMP here,
# so that all binaries are compiled with the same settings.

IF(GENESIS_USE_THREADS)
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

ENDIF()

# ----------------------------------------------------------
#     OpenMP
# ----------------------------------------------------------

IF(GENESIS_USE_OPENMP)
    message (STATUS "Looking for OpenMP")

    # CMake versions before 3.5 do not properly support Clang OpenMP flags.
    # And even with more recent CMake versions, it is not fully working.
    # In order to still enable OpenMP, we use our own patch of the find module.
    # See ./tools/cmake/FindOpenMP_patch.cmake for the modifications we made to support Clang.
    if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )

        find_package( OpenMP_patch )

        # If our patch does not work, try the standard one, just in case.
        if( NOT OPENMP_FOUND )
            unset(OpenMP_CXX_FLAGS)
            unset(OpenMP_CXX_FLAGS CACHE)
            find_package( OpenMP )
        endif()

    else()
        # In all other cases, use the normal find module.
        find_package( OpenMP )
    endif()

    # If found, set all needed compiler flags and also add those flags to the Genesis exported flags.
    if(OPENMP_FOUND)

        message( STATUS "Found OpenMP: ${OpenMP_CXX_FLAGS}" )
        message (STATUS "${ColorGreen}Using OpenMP${ColorEnd}")

        # Set needed definition for Genesis and for the parent scope.
        add_definitions( "-DGENESIS_OPENMP" )
        set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_OPENMP" )

        # Set the flags needed for compiling Genesis.
        set( CMAKE_C_FLAGS            "${CMAKE_C_FLAGS}            ${OpenMP_C_FLAGS}")
        set( CMAKE_CXX_FLAGS          "${CMAKE_CXX_FLAGS}          ${OpenMP_CXX_FLAGS}" )
        set( CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}   ${OpenMP_EXE_LINKER_FLAGS}" )

        # Set the flags that are exported to parent scope so that the parent can also compile using
        # the those flags.
        set( GENESIS_C_FLAGS          "${GENESIS_C_FLAGS}          ${OpenMP_C_FLAGS}" )
        set( GENESIS_CXX_FLAGS        "${GENESIS_CXX_FLAGS}        ${OpenMP_CXX_FLAGS}" )
        set( GENESIS_EXE_LINKER_FLAGS "${GENESIS_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )

    else()
        message( STATUS "${ColorRed}OpenMP not found${ColorEnd}" )

        # Special warning for users that have a Clang version that supports OpenMP, but where
        # the find module still did not succeed.
        if(
            "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"
            AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8
        )
            message (
                STATUS
                "${ColorYellow}You are trying to compile with Clang and OpenMp. "
                "This does not seem to work right now. Try installing `libiomp-dev` first, "
                "or the equivalent for your system.${ColorEnd}"
            )
        endif()

        message( STATUS "To build without OpenMP support, call CMake with -DGENESIS_USE_OPENMP=OFF" )
        message( FATAL_ERROR "Required package OpenMP not found.")
    endif()

ENDIF()

# ----------------------------------------------------------
#     htslib
# ----------------------------------------------------------

# Store the commit hash that we want to check out for htslib.
SET( htslib_COMMIT_HASH "bd133acf51498431a5c7dfd8aa06ce17ec6d3b96" ) #htslib_COMMIT_HASH#

if(GENESIS_USE_HTSLIB)
    message (STATUS "Looking for htslib")

    IF(GENESIS_SYSTEM_HTSLIB)
        message( STATUS "Looking for system-wide htslib installation" )
        IF(HTSLIB_HINT)
            message( STATUS "Using hint path: ${HTSLIB_HINT}" )
        ENDIF()

        # Find system-wide installation of htslib
        find_path(HTSLIB_INCLUDE_DIR NAMES htslib/hts.h HINTS "${HTSLIB_HINT}/include")
        find_library(HTSLIB_LIBRARY  NAMES libhts.a hts HINTS "${HTSLIB_HINT}/lib")
        set( HTSLIB_LINK_DIR ${HTSLIB_LIBRARY} )

        IF(HTSLIB_FOUND)
            message( STATUS "Using system htslib" )
        ENDIF()

    ELSE()
        message( STATUS "Using local htslib" )

        # We download and built on our own, using the correct commit hash to get our exact desired
        # version, and install locally to the build directory.
        ExternalProject_Add(
            htslib
            PREFIX ""

            # Download and Update Steps
            # DOWNLOAD_COMMAND  ""
            GIT_REPOSITORY https://github.com/samtools/htslib.git
            GIT_TAG ${htslib_COMMIT_HASH}
            UPDATE_COMMAND    ""

            # Configure Step. See htslib/INSTALL
            # We need to manually add -fPIC here, as somehow otherwise the local installation
            # won't link properly. Linking will always remain a mystery to me...
            SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib-source
            CONFIGURE_COMMAND
                autoheader
                COMMAND
                autoconf
                COMMAND
                ./configure CFLAGS=-fPIC CXXFLAGS=-fPIC --prefix=${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib --disable-libcurl

            # Build Step
            # BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib
            BUILD_IN_SOURCE true
            BUILD_COMMAND
                $(MAKE)

            # Install Step
            INSTALL_COMMAND $(MAKE) install
        )

        # Set the paths so that those can be included by the targets.
        # We explicitly set the static library here, so that we link against that one.
        set( HTSLIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib )
        set( HTSLIB_INCLUDE_DIR "${HTSLIB_DIR}/include" )
        set( HTSLIB_LINK_DIR    "${HTSLIB_DIR}/lib" )
        set( HTSLIB_LIBRARY     "${HTSLIB_DIR}/lib/libhts.a" )

        # Cannot test here for existing files, as those are not yet built when this is called...
        # IF(
        #     NOT EXISTS ${HTSLIB_INCLUDE_DIR}/htslib/hts.h OR
        #     NOT EXISTS ${HTSLIB_LIBRARY}/libhts.a
        # )
        #     message( FATAL_ERROR "Building htslib failed.")
        # ENDIF()
    ENDIF()

    # Find additional libraries needed by htslib.
    # If that works and the libraries were found, we do not need to do anything,
    # as the htslib compilation will look for them again and link to them correctly.
    # We here merely make sure that all needed libraries are available.

    # find_library(HTSLIB_LZMA     NAMES lzma)
    message( STATUS "Looking for LibLZMA" )
    find_package(LibLZMA)
    IF(LIBLZMA_FOUND)
        message( STATUS "Found LibLZMA: ${LIBLZMA_LIBRARIES}" )
        set( HTSLIB_LZMA ${LIBLZMA_LIBRARIES} )
    ELSE()
        message( STATUS "${ColorRed}LibLZMA not found${ColorEnd}" )
        message (
            STATUS "${ColorYellow}You are trying to compile with htslib, which needs LibLZMA. "
            "This does not seem to work right now. Try installing `liblzma-dev` first, "
            "or the equivalent for your system. To instead build without htslib support, "
            "call CMake with `-DGENESIS_USE_HTSLIB=OFF`.${ColorEnd}"
        )
        set( HTSLIB_LZMA "NOTFOUND" )
    ENDIF()

    # find_library(HTSLIB_BZ2      NAMES bz2)
    message( STATUS "Looking for BZip2" )
    find_package(BZip2)
    IF(BZIP2_FOUND)
        message( STATUS "Found BZip2: ${BZIP2_LIBRARIES}" )
        set( HTSLIB_BZ2 ${BZIP2_LIBRARIES} )
    ELSE()
        message( STATUS "${ColorRed}BZip2 not found${ColorEnd}" )
        message (
            STATUS "${ColorYellow}You are trying to compile with htslib, which needs BZip2. "
            "This does not seem to work right now. Try installing `libbz2-dev` first, "
            "or the equivalent for your system. To instead build without htslib support, "
            "call CMake with `-DGENESIS_USE_HTSLIB=OFF`.${ColorEnd}"
        )
        set( HTSLIB_BZ2 "NOTFOUND" )
    ENDIF()

    # Useful output for debugging
    message( STATUS "HTSLIB_INCLUDE_DIR: ${HTSLIB_INCLUDE_DIR}" )
    message( STATUS "HTSLIB_LINK_DIR:    ${HTSLIB_LINK_DIR}" )
    message( STATUS "HTSLIB_LIBRARY:     ${HTSLIB_LIBRARY}" )
    message( STATUS "HTSLIB_LZMA:        ${HTSLIB_LZMA}" )
    message( STATUS "HTSLIB_BZ2:         ${HTSLIB_BZ2}" )

    IF(
        ${HTSLIB_INCLUDE_DIR} MATCHES "NOTFOUND" OR
        ${HTSLIB_LINK_DIR}    MATCHES "NOTFOUND" OR
        ${HTSLIB_LIBRARY}     MATCHES "NOTFOUND" OR
        ${HTSLIB_LZMA}        MATCHES "NOTFOUND" OR
        ${HTSLIB_BZ2}         MATCHES "NOTFOUND"
    )
        message( FATAL_ERROR "Required package htslib (or some of its dependencies) not found.")
    ELSE()

        # Technically, we can only claim to have "found" the library when using the system-wide
        # installation here, as in the local (default) case it won't yet be compiled...
        # But let's stick to the user output that we used for the other dependencies,
        # and assume that once we are here, the build process will work.
        message( STATUS "Found htslib: ${HTSLIB_LIBRARY}" )
        message (STATUS "${ColorGreen}Using htslib${ColorEnd}")

        # Include the headers and directories, so that they are found by our source.
        include_directories(${HTSLIB_INCLUDE_DIR})
        link_directories(${HTSLIB_LINK_DIR})

        # Set needed definitions and linker flags for genesis and for the parent scope.
        # The targets (our shared and static lib) will use these to link against.
        set( GENESIS_INCLUDE_DIR ${GENESIS_INCLUDE_DIR} ${HTSLIB_INCLUDE_DIR} )
        add_definitions( "-DGENESIS_HTSLIB" )
        set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_HTSLIB" )
        set(
            GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES}
            ${HTSLIB_LIBRARY} z ${HTSLIB_LZMA} ${HTSLIB_BZ2}
        )
        # set(
        #     GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES}
        #     -L/usr/local/lib -lhts ${HTSLIB_LIBRARY} z ${HTSLIB_LZMA} ${HTSLIB_BZ2}
        # )
    ENDIF()

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
