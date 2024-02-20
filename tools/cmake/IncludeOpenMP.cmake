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
#   Use OpenMP
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# if OpenMP is available, and if so, set its paths and flags.

message (STATUS "Looking for OpenMP")

# CMake versions before 3.5 do not properly support Clang OpenMP flags.
# And even with more recent CMake versions, it is not fully working.
# In order to still enable OpenMP, we use our own patch of the find module.
# See ./tools/cmake/FindOpenMP_patch.cmake for the modifications we made to support Clang.
if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )

    # For some clang mac versions, we need to add a linker flag, see
    # https://open-box.readthedocs.io/en/latest/installation/openmp_macos.html and
    # https://mac.r-project.org/openmp/ for details. Let's hope that works universally...
    IF(APPLE)
        set( TMP_CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" )
        set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lomp" )
    ENDIF()

    message (STATUS "Using find_package( OpenMP ) patch")
    find_package( OpenMP_patch )

    # If our first patch does not work, try the other one, based on a newer CMake FindOpenMP file.
    # if( NOT OPENMP_FOUND )
    #     message (STATUS "find_package( OpenMP ) patch did not succeed - trying patch2 instead")
    #     unset(OpenMP_CXX_FLAGS)
    #     unset(OpenMP_CXX_FLAGS CACHE)
    #     find_package( OpenMP_patch2 )
    # endif()

    # If our patch does not work, try the standard one, just in case.
    if( NOT OPENMP_FOUND )
        message (STATUS "find_package( OpenMP ) patch did not succeed - trying base script instead")
        unset(OpenMP_CXX_FLAGS)
        unset(OpenMP_CXX_FLAGS CACHE)
        find_package( OpenMP )
    endif()

    IF(APPLE)
        # Reset so that we don't accidentally add the wrong flags.
        set( CMAKE_EXE_LINKER_FLAGS "${TMP_CMAKE_EXE_LINKER_FLAGS}" )
    ENDIF()

else()
    # In all other cases, use the normal find module.
    find_package( OpenMP )
endif()

# If found, set all needed compiler flags and also add those flags to the Genesis exported flags.
if(OPENMP_FOUND)

    # With clang++ 11, we get:
    # > Target "genesis_lib_shared" links to target "OpenMP::OpenMP_CXX" but the target was not found.
    # To solve this, (for CMake < 3.9 ?!), we need to make the target ourselves,
    # adapted from https://cliutils.gitlab.io/modern-cmake/chapters/packages/OpenMP.html
    if(NOT TARGET OpenMP::OpenMP_CXX)
        find_package(Threads REQUIRED)
        add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
        set_property(
            TARGET OpenMP::OpenMP_CXX
            PROPERTY INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS}
        )
        # Only works if the same flag is passed to the linker;
        # use CMake 3.9+ otherwise (Intel, AppleClang)
        set_property(
            TARGET OpenMP::OpenMP_CXX
            PROPERTY INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} Threads::Threads
        )
    endif()

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
        AND NOT APPLE
    )
        message (
            STATUS
            "${ColorYellow}You are trying to compile with Clang and OpenMP. "
            "This does not seem to work right now. Try installing `libomp-dev` or `libiomp-dev` "
            "first, or the equivalent for your system.${ColorEnd}"
        )
    endif()

    if( APPLE AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
        message (
            STATUS
            "${ColorYellow}You are trying to compile with Apple Clang and OpenMP. "
            "This does not seem to work right now, and is known to be notoriously difficult to get "
            "to work. We have deactivated OpenMP for now, as it seems near impossible to support "
            "all MacOS versions and their respective OpenMP issues. If you want the speed "
            "improvements that OpenMP gives, try installing the necessary OpenMP libraries for your "
            "system first. Alternatively, we recommend compiling with gcc instead.${ColorEnd}"
        )
        message(
            STATUS
            "To silence this message and build without OpenMP support, call CMake with "
            "-DGENESIS_USE_OPENMP=OFF"
        )
    else()
        message( STATUS "To build without OpenMP support, call CMake with -DGENESIS_USE_OPENMP=OFF" )
        message( FATAL_ERROR "Required package OpenMP not found.")
    endif()
endif()
