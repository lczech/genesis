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

# --------------------------------------------------------------------------------------------------
#   CMake Init
# --------------------------------------------------------------------------------------------------

# Require the minimum version of any system that we test on - which is the version used by Travis.
# Thus, we can be sure that this works (as long as they don't change it...).
cmake_minimum_required (VERSION 2.8.7 FATAL_ERROR)
message (STATUS "CMake version ${CMAKE_VERSION}")

set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

if (NOT WIN32)
    # Fun with colors!
    string(ASCII 27 Esc)

    set(ColorBlue   "${Esc}[34m") # Build information.
    set(ColorGreen  "${Esc}[32m") # Optional additions.
    set(ColorYellow "${Esc}[33m") # Tipps for the user.
    set(ColorRed    "${Esc}[31m") # Warnings and Errors.
    set(ColorEnd    "${Esc}[0m")
endif()

# --------------------------------------------------------------------------------------------------
#   Project Definitions and Variables
# --------------------------------------------------------------------------------------------------

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
message (STATUS "${ColorBlue}Genesis version: ${GENESIS_VERSION}${ColorEnd}")

# Figure out whether Genesis is compiled stand-alone or as a library/dependency.
# We use two complementary variable, because we want to use them as default for options
# (see later on), which do not accept a `NOT`...
# Also, we need to re-set GENESIS_HAS_PARENT_DIR to an actual bool value.
get_directory_property( GENESIS_HAS_PARENT_DIR PARENT_DIRECTORY )
if( GENESIS_HAS_PARENT_DIR )
    set( GENESIS_HAS_PARENT_DIR ON )
    set( GENESIS_MASTER_PROJECT OFF )
    message (STATUS "Scope: library")
else()
    set( GENESIS_HAS_PARENT_DIR OFF )
    set( GENESIS_MASTER_PROJECT ON )
    message (STATUS "Scope: stand-alone")
endif()

# --------------------------------------------------------------------------------------------------
#   Build Options
# --------------------------------------------------------------------------------------------------

# Options for which library version to build. Default: Build shared for stand-alone version,
# and build static when being used as a library.
option (GENESIS_BUILD_SHARED_LIB    "Build the shared library version of Genesis." ${GENESIS_MASTER_PROJECT})
option (GENESIS_BUILD_STATIC_LIB    "Build the static library version of Genesis." ${GENESIS_HAS_PARENT_DIR})

# If we are the master project, we want to build the apps by default.
option (GENESIS_BUILD_APPLICATIONS  "Build Genesis applications."        ${GENESIS_MASTER_PROJECT})

# Currently, the Python module and tests are deactivated by default.
option (GENESIS_BUILD_PYTHON_MODULE "Build the Genesis python module."   OFF)
option (GENESIS_BUILD_TESTS         "Build the Genesis test suites."     OFF)

# We default to using threads (if available, which will be checked later).
option (GENESIS_USE_THREADS         "Use a threading library (mainly, pthreads)."  ON)
option (GENESIS_USE_OPENMP          "Use OpenMP, if available."                    ON)

# Define build type option with list of valid values. Default is RELEASE.
set( GENESIS_BUILD_TYPE RELEASE CACHE STRING
    "Build type. Either RELEASE or DEBUG."
)
set_property( CACHE GENESIS_BUILD_TYPE PROPERTY STRINGS RELEASE DEBUG )

# Define unity build option with list of valid values. Default is full unity build.
set( GENESIS_UNITY_BUILD FULL CACHE STRING
    "Build modules monolithically - trading memory for speed. This also enables poor-mans LTO."
)
set_property( CACHE GENESIS_UNITY_BUILD PROPERTY STRINGS FULL MODULES OFF )

# The applications, the python module and the tests all prefer the shared lib,
# so we need to build it if one of those is activated.
if( GENESIS_BUILD_APPLICATIONS OR GENESIS_BUILD_PYTHON_MODULE OR GENESIS_BUILD_TESTS )
    set (GENESIS_BUILD_SHARED_LIB ON)
endif()

# --------------------------------------------------------------------------------------------------
#   Build Type
# --------------------------------------------------------------------------------------------------

# Use our build type option as default for the CMake build type.
set (CMAKE_BUILD_TYPE ${GENESIS_BUILD_TYPE} )

# Using the environment variable `GENESIS_DEBUG`, it is possible to change from release mode
# (default) to debug mode, even without changing the option GENESIS_BUILD_TYPE.  This is useful for
# developers, and can be activated for example in bash  via `export GENESIS_DEBUG=1` prior to
# calling CMake. It also activates test building automatically.
if( $ENV{GENESIS_DEBUG} )

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

# Set the warn flags to a very high level - except unknown pragmas, as this causes needless
# warnings with OpenMP and other pragma-based techniques.
set (WARN_FLAGS              "-Wall -Wextra -Wno-unknown-pragmas -pedantic -pedantic-errors")
set (CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11 ${WARN_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG   "-O2 -g -ggdb3 -DDEBUG -D_GLIBCXX_DEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O2           -DNDEBUG")

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

# --------------------------------------------------------------------------------------------------
#   Threads and OpenMP
# --------------------------------------------------------------------------------------------------

# We check whether to use threads and OpenMP here,
# so that all binaries are compiled with the same settings.

IF(GENESIS_USE_THREADS)
    message (STATUS "Looking for Threads")
    set (CMAKE_THREAD_PREFER_PTHREAD ON)
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads)
ENDIF()

if(THREADS_FOUND)
    message( STATUS "Found Threads: ${CMAKE_THREAD_LIBS_INIT}" )
    message (STATUS "${ColorGreen}Using Threads${ColorEnd}")

    # Set needed definition and linker flags for Genesis and for the parent scope.
    add_definitions( "-DGENESIS_PTHREADS" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_PTHREADS" )
    set( GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

elseif(GENESIS_USE_THREADS)
    message (STATUS "Threads not found")
endif()

IF(GENESIS_USE_OPENMP)
    message (STATUS "Looking for OpenMP")

    # CMake versions before 3.5 do not properly support Clang OpenMP flags.
    # And even with more recent CMake versions, it is not fully working.
    # In order to still enable OpenMP, we use our own patch of the find module.
    # See ./tools/cmake/FindOpenMP_patch.cmake for the modifications we made to support Clang.
    if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
        set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/tools/cmake" )
        find_package( OpenMP_patch )

        # If our patch does not work, try the standard one, just in case.
        if( NOT OPENMP_FOUND )
            find_package( OpenMP )
        endif()

    else()
        # In all other cases, use the normal find module.
        find_package( OpenMP )
    endif()
ENDIF()

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

elseif(
    GENESIS_USE_OPENMP
    AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"
    AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8
)
    # Special warning for users that have a Clang version that supports OpenMP, but where
    # the find module still did not succeed.
    message (STATUS "OpenMP not found")
    message (STATUS "${ColorYellow}You are trying to compile with Clang and OpenMp. "
    "This does not seem to work right now. Try installing 'libiomp-dev' first.${ColorEnd}")
elseif(GENESIS_USE_OPENMP)
    message (STATUS "OpenMP not found")
endif()

# --------------------------------------------------------------------------------------------------
#   Unity Build
# --------------------------------------------------------------------------------------------------

# Helper that builds monolithic source files by concatenating the original sources.
#
# It takes four parameters:
#
# input_directory: Where to look for source files (currently hardcoded to *.cpp)
# use_modules: Either "FULL" or "MODULES". The function offers two modes for monolithic building.
#     In "FULL" mode, all sources of the given input_directory are combined into one.
#     In "MODULES" mode, each immediate subdirectory of input_directory is treated separately,
#     resulting in one monolithic compilation unit per subdirectory. Files that are directly in
#     input_directory (not in its subdirectories) are simply used as normal single compilation units.
# name: Is used to distinguish between multiple monolithic parts during build (one for the lib,
#     one for test etc). The name needs to be uniq for each call of this function.
# resulting_source_files: Is used to store the resulting sources files. The value of this variable
#     can then ben used as source argument for add_library() and the like.
function( ASSEMBLE_MONOLITH input_directory use_modules name resulting_source_files  )

    # Helper: Return a list of all directories within a given directory, non-recursive.
    function( SUBDIRLIST result indir )
        file( GLOB children RELATIVE ${indir} ${indir}/* )
        set( dirlist "" )
        foreach(child ${children})
            if( IS_DIRECTORY ${indir}/${child} )
                LIST( APPEND dirlist ${child} )
            endif()
        endforeach()
        set( ${result} ${dirlist} PARENT_SCOPE )
    endfunction()

    # Helper: Append the contents of a file to another file.
    function( APPEND_TO_FILE IN_FILE OUT_FILE )
        file( READ ${IN_FILE} CONTENTS )
        file( APPEND ${OUT_FILE} "${CONTENTS}" )
    endfunction()

    # Clean previously assembled monolith files.
    file( REMOVE_RECURSE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name} )

    IF( "${use_modules}" STREQUAL "MODULES" )

        # Process all modules (i.e., subdirs of the input dir).
        SUBDIRLIST( SUBDIRS ${input_directory} )
        foreach( SUBDIR ${SUBDIRS} )

            # Target files for this module.
            SET( ASSEMBLY_FILE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name}/${SUBDIR}.cpp )
            SET( MONOLITH_FILE ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/${SUBDIR}.cpp )

            # Find all sources of the module and concat them into a tmp assembly file.
            file (GLOB_RECURSE subdir_sources ${input_directory}/${SUBDIR}/*.cpp)
            file( WRITE ${ASSEMBLY_FILE} "" )
            foreach( subdir_source ${subdir_sources} )
                APPEND_TO_FILE( ${subdir_source} ${ASSEMBLY_FILE} )
            endforeach()

            # Copy the assemlby file to its destination. Using configure_file here ensures that we
            # only copy if the file changes, so that incremental builds are faster.
            configure_file( ${ASSEMBLY_FILE} ${MONOLITH_FILE} COPYONLY )
        endforeach()

        # Find all resulting source files. We need to treat top-level files in the lib dir
        # extra, as they are not a part of any module.
        file (GLOB top_level_sources ${input_directory}/*.cpp)
        file (GLOB genesis_unity_sources     ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/*.cpp)
        set ( ${resulting_source_files} ${top_level_sources} ${genesis_unity_sources} PARENT_SCOPE )

    ELSEIF( "${use_modules}" STREQUAL "FULL" )

        # Target files.
        SET( ASSEMBLY_FILE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name}/all.cpp )
        SET( MONOLITH_FILE ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/all.cpp )

        # Find all sources and concat them into a tmp assembly file.
        file (GLOB_RECURSE all_sources ${input_directory}/*.cpp)
        file( WRITE ${ASSEMBLY_FILE} "" )
        foreach( subdir_source ${all_sources} )
            APPEND_TO_FILE( ${subdir_source} ${ASSEMBLY_FILE} )
        endforeach()

        # Copy the assemlby file to its destination. Using configure_file here ensures that we
        # only copy if the file changes, so that incremental builds are faster.
        configure_file( ${ASSEMBLY_FILE} ${MONOLITH_FILE} COPYONLY )

        # Return the assembled file.
        set ( ${resulting_source_files} ${MONOLITH_FILE} PARENT_SCOPE )

    ELSE()
        # Can only happen if some CMake file misuses this function.
        message( FATAL_ERROR
            "${ColorRed}ASSEMBLE_MONOLITH called with wrong arg: ${use_modules}${ColorEnd}"
        )
    ENDIF()
endfunction()

# --------------------------------------------------------------------------------------------------
#   Sub-Scripts
# --------------------------------------------------------------------------------------------------

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

ENDIF()
