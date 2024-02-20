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

# ==================================================================================================
#   Init
# ==================================================================================================

# This file is included from the main CMakeLists.txt in order to build htslib.
message (STATUS "Looking for htslib")
cmake_minimum_required( VERSION 3.1 )
include(ExternalProject)

# ==================================================================================================
#   Check for autotools
# ==================================================================================================

# Check that autoconf is available, and warn if version has a potential issue.
# See https://github.com/asdf-vm/asdf-erlang/issues/195, https://github.com/kerl/kerl/issues/359,
# https://stackoverflow.com/a/67470521/4184258
find_program(AUTOCONF_RUNTIME autoconf)
if( AUTOCONF_RUNTIME STREQUAL "AUTOCONF_RUNTIME-NOTFOUND" )

    message (
        STATUS "${ColorRed}You are trying to compile with htslib, which needs autotools/autoconf. "
        "This does not seem to to be availabe on your system. Try installing "
        "`autoconf automake autotools-dev` first. To instead build without htslib "
        "support, call CMake with `-DGENESIS_USE_HTSLIB=OFF`.${ColorEnd}"
    )
    message( FATAL_ERROR "Required autotools for building htslib not found.")

else()

    # Run the version command of autotools. Whoever programmed that command did not follow any
    # good practice. It outputs a multi line thing with lots of information that we do not need.
    execute_process(
        COMMAND "${AUTOCONF_RUNTIME}" --version
        OUTPUT_VARIABLE AUTOCONF_VERSION_OUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # CMake is such a mess... regex `^$` denote the whole string in cmake, instead of individual lines...
    # So we need to take it apart line by line and only use the first one,
    # see https://cmake.cmake.narkive.com/f05xOtbE/regex-and-do-not-match-on-multi-line-input
    STRING(REGEX REPLACE "\r?\n" ";" AUTOCONF_VERSION_LINES "${AUTOCONF_VERSION_OUT}")
    list (GET AUTOCONF_VERSION_LINES 0 AUTOCONF_VERSION_OUT)

    # Get the autoconf version, and compare it to the known bug versions.
    # Apparenlty, we need to check first, and then match again, as we cannot use the match
    # to check whether it has matched at all...
    IF(AUTOCONF_VERSION_OUT MATCHES "^autoconf .* ([^ ]*)$")
        string(
            REGEX MATCH "^autoconf .* ([^ ]*)$"
            AUTOCONF_VERSION_MATCH "${AUTOCONF_VERSION_OUT}"
        )
        set( AUTOCONF_VERSION "${CMAKE_MATCH_1}")
        message( STATUS "Found autoconf: ${AUTOCONF_VERSION}" )

        # Update on the below: Outdated, as we now use a newer version of htslib,
        # which properly supports more recent autoconf versions as well.
        # Versions greater than 2.69 cause problems. Of course, we cannot use VERSION_GREATER,
        # as that was introduced in CMake 3.7 only, so we need to work around that, too...
        # SET(AUTOCONF_MAX_VERSION "2.69")
        # if( NOT (
        #     ( AUTOCONF_VERSION VERSION_LESS ${AUTOCONF_MAX_VERSION} ) OR
        #     ( AUTOCONF_VERSION VERSION_EQUAL ${AUTOCONF_MAX_VERSION} )
        # ))
        #     message (
        #         STATUS "${ColorYellow}You are trying to compile with htslib, using autotools/autoconf "
        #         "version ${AUTOCONF_VERSION}, which is greater than ${AUTOCONF_MAX_VERSION}. "
        #         "There have been issues with these later versions of autotools. If you experience "
        #         "these issues, please downgrade autotools to ${AUTOCONF_MAX_VERSION}. "
        #         "To instead build without htslib support, "
        #         "call CMake with `-DGENESIS_USE_HTSLIB=OFF`.${ColorEnd}"
        #     )
        # endif()
    ENDIF()

endif()

# ==================================================================================================
#   External dependencies
# ==================================================================================================

# Find additional libraries needed by htslib.
# If that works and the libraries were found, we do not need to do anything,
# as the htslib compilation will look for them again and link to them correctly.
# We here merely make sure that all needed libraries are available.

# ----------------------------------------------------
#   Find LibLZMA
# ----------------------------------------------------

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

# ----------------------------------------------------
#   Find BZip2
# ----------------------------------------------------

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

# ----------------------------------------------------
#   Find Deflate
# ----------------------------------------------------

message( STATUS "Looking for Deflate" )
find_package( Deflate )
IF(Deflate_FOUND)
    message( STATUS "Found Deflate: ${Deflate_LIBRARIES}" )
    set( HTSLIB_Deflate ${Deflate_LIBRARIES} )
    set( HTSLIB_Deflate_configure "" )
ELSE()
    message( STATUS "${ColorYellow}Deflate not found${ColorEnd}" )
    message (
        STATUS "${ColorYellow}You are trying to compile with htslib, which optionally needs "
        "libdeflate. This does not seem to work right now. Try installing `libdeflate-tools` "
        "first, or the equivalent for your system. We are compiling without libdeflate support "
        "for now.${ColorEnd}"
    )
    set( HTSLIB_Deflate "" )
    set( HTSLIB_Deflate_configure "--without-libdeflate" )
ENDIF()

# ==================================================================================================
#   Add htslib
# ==================================================================================================

# With recent gcc, we get a couple of warnings about unused variables. Nothing too bad though.
# Still, let's stop that from spamming our build output - not our problem to fix!
# SET( HTSLIB_COMPILER_FLAGS "" )
# if(
#     (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND
#     ((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9") OR
#     (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9"))
# )
#     # message (STATUS "Deactivate -Wmaybe-uninitialized for htslib binary.")
#
#     # We set the warning deactivation flags here, but unfortunately also need to set the -fPIC flag
#     # here already, as it's otherwise not used... https://github.com/samtools/htslib/issues/1527
#     SET( HTSLIB_COMPILER_FLAGS "-fPIC -Wno-unused-but-set-variable -Wno-unused-result" )
# endif()

# URL timestamp extraction, see https://cmake.org/cmake/help/latest/policy/CMP0135.html
# Introduced in CMake 3.24, but older CMake versions fail if present, so we need to check first...
# Why is that so complicated? Can't CMake just ignore unknown policies?!
if( NOT ( CMAKE_VERSION VERSION_LESS 3.24 ))
    cmake_policy(SET CMP0135 NEW)
endif()

# We download and built on our own, using the correct commit hash to get our exact desired
# version, and install locally to the build directory.
ExternalProject_Add(
    htslib
    PREFIX ""

    # Activate logging for better user debugging
    LOG_DOWNLOAD  TRUE
    LOG_CONFIGURE TRUE
    LOG_BUILD     TRUE
    LOG_INSTALL   TRUE
    STAMP_DIR     ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib-stamp

    # Download and Update Steps
    # DOWNLOAD_COMMAND  ""
    # GIT_REPOSITORY https://github.com/samtools/htslib.git
    # GIT_TAG ${htslib_COMMIT_HASH}
    URL https://github.com/samtools/htslib/releases/download/${GENESIS_HTSLIB_VERSION}/htslib-${GENESIS_HTSLIB_VERSION}.tar.bz2
    DOWNLOAD_NO_PROGRESS TRUE
    UPDATE_COMMAND    ""

    # Configure Step. See htslib/INSTALL
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib-source
    CONFIGURE_COMMAND
        autoreconf -i
        # COMMAND
        # autoheader
        # COMMAND
        # autoconf
        COMMAND
        # We need to manually add -fPIC here, as somehow otherwise the local installation
        # won't link properly. Linking will always remain a mystery to me...
        # Need some special care to fix https://github.com/lczech/grenedalf/issues/12,
        # see https://stackoverflow.com/a/59536947 for the solution.
        ./configure CFLAGS=-fPIC CXXFLAGS=-fPIC --prefix=${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib --libdir=${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib/lib --disable-multi-os-directory --disable-libcurl ${HTSLIB_Deflate_configure}

    # Build Step
    # BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib
    BUILD_IN_SOURCE true
    BUILD_COMMAND
        # Didn't get these to work in the above configure... So we amend them here...
        # I think it's because htslib does some internal flag stuff as well that is not
        # cleanly implemented, see https://github.com/samtools/htslib/issues/1527
        # $(MAKE) CFLAGS+=${HTSLIB_COMPILER_FLAGS} CXXFLAGS+=${HTSLIB_COMPILER_FLAGS}
        # Nope... the above failes under clang?! Let's just deactivate for now.
        $(MAKE)

    # Install Step
    INSTALL_COMMAND $(MAKE) install
)

# Set the paths so that those can be included by the targets.
# We explicitly set the static library here, so that we link against that one.
set( HTSLIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib" )
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

# ==================================================================================================
#   Finalize
# ==================================================================================================

# Useful output for debugging
message( STATUS "HTSLIB_INCLUDE_DIR:   ${HTSLIB_INCLUDE_DIR}" )
message( STATUS "HTSLIB_LINK_DIR:      ${HTSLIB_LINK_DIR}" )
message( STATUS "HTSLIB_LIBRARY:       ${HTSLIB_LIBRARY}" )
message( STATUS "HTSLIB_LZMA:          ${HTSLIB_LZMA}" )
message( STATUS "HTSLIB_BZ2:           ${HTSLIB_BZ2}" )
if(DEFINED Deflate_INCLUDE_DIRS)
    message( STATUS "HTSLIB_Deflate:       ${HTSLIB_Deflate}" )
    message( STATUS "Deflate_INCLUDE_DIRS: ${Deflate_INCLUDE_DIRS}" )
endif()

IF(
    ${HTSLIB_INCLUDE_DIR} MATCHES "NOTFOUND" OR
    ${HTSLIB_LINK_DIR}    MATCHES "NOTFOUND" OR
    ${HTSLIB_LIBRARY}     MATCHES "NOTFOUND" OR
    ${HTSLIB_LZMA}        MATCHES "NOTFOUND" OR
    ${HTSLIB_BZ2}         MATCHES "NOTFOUND"
)
    message( FATAL_ERROR "Required package htslib (or some of its dependencies) not found.")
ELSE()

    # Technically, we cannot claim to have "found" the library here, as it won't yet be compiled...
    # But let's stick to the user output that we used for the other dependencies,
    # and assume that once we are here, the build process will work.
    message( STATUS "Found htslib: ${HTSLIB_LIBRARY}" )
    message (STATUS "${ColorGreen}Using htslib${ColorEnd}")

    # Include the headers and directories, so that they are found by our source.
    include_directories(${HTSLIB_INCLUDE_DIR})
    link_directories(${HTSLIB_LINK_DIR})

    if(DEFINED Deflate_INCLUDE_DIRS)
        include_directories(${Deflate_INCLUDE_DIRS})
        set( GENESIS_INCLUDE_DIR ${GENESIS_INCLUDE_DIR} ${Deflate_INCLUDE_DIRS} )
    endif()

    # Set needed definitions and linker flags for genesis and for the parent scope.
    # The targets (our shared and static lib) will use these to link against.
    set( GENESIS_INCLUDE_DIR ${GENESIS_INCLUDE_DIR} ${HTSLIB_INCLUDE_DIR} )
    add_definitions( "-DGENESIS_HTSLIB" )
    set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_HTSLIB" )
    set(
        GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES}
        ${HTSLIB_LIBRARY} z -lz ${HTSLIB_LZMA} ${HTSLIB_BZ2} ${HTSLIB_Deflate}
    )
    # set(
    #     GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES}
    #     -L/usr/local/lib -lhts ${HTSLIB_LIBRARY} z ${HTSLIB_LZMA} ${HTSLIB_BZ2}
    # )
ENDIF()
