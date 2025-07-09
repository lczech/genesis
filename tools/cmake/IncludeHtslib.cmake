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

# ==================================================================================================
#   Init
# ==================================================================================================

# This file is included from the main CMakeLists.txt in order to build htslib.
message (STATUS "Looking for htslib")
cmake_minimum_required( VERSION 3.14 )
include(ExternalProject)
include(FetchContent)

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
        "and `libdeflate-dev` first, or the equivalent for your system. We are compiling without "
        "libdeflate support for now.${ColorEnd}"
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

# Locate or fetch the htslib sources.
# If we have a local submodule, we clone from there, which is a bit doubled effort,
# but enables us to cleanly build out of source. Otherwise, cmake and/or htslib will start
# polluting the submodule directory. Easiest that way for now; can be refined later.
set(HTSLIB_GIT_SUBMODULE_DIR "${PROJECT_SOURCE_DIR}/external/htslib")
if (EXISTS "${HTSLIB_GIT_SUBMODULE_DIR}/configure.ac")
    message(STATUS "Using htslib submodule")
    set(HTSLIB_REPOSITORY "file://${HTSLIB_GIT_SUBMODULE_DIR}")
else()
    message(STATUS "htslib submodule not found")
    message(STATUS "Will fetch htslib from GitHub")
    set(HTSLIB_REPOSITORY "https://github.com/samtools/htslib.git")
endif()

# Now fetch the htslib code, either from the submodule, or fresh from GitHub
FetchContent_Declare(
    htslib
    GIT_REPOSITORY ${HTSLIB_REPOSITORY}
    GIT_TAG        ${htslib_GIT_TAG}
    # SOURCE_DIR     ${CMAKE_BINARY_DIR}/htslib-source
)
FetchContent_GetProperties(htslib)
if (NOT htslib_POPULATED)
    FetchContent_Populate(htslib)
endif()
set(HTSLIB_SOURCE_DIR ${htslib_SOURCE_DIR})

# Sanity check: by now ${HTSLIB_SOURCE_DIR} contains the full htslib tree
if( EXISTS "${HTSLIB_SOURCE_DIR}/configure.ac")
    message(STATUS "HTSLIB_SOURCE_DIR:    ${HTSLIB_SOURCE_DIR}")
else()
    message(FATAL_ERROR "htslib source not found in ${HTSLIB_SOURCE_DIR}")
endif()

# Set up htslib build and install directories
# set(HTSLIB_SOURCE_DIR   ${CMAKE_BINARY_DIR}/htslib-source)
set(HTSLIB_INSTALL_DIR  ${CMAKE_BINARY_DIR}/htslib-install)

# Build and install via ExternalProject
ExternalProject_Add(
    htslib
    PREFIX ""
    GIT_REPOSITORY    ${HTSLIB_REPOSITORY}
    GIT_TAG           ${htslib_GIT_TAG}

    SOURCE_DIR   ${HTSLIB_SOURCE_DIR}
    INSTALL_DIR  ${HTSLIB_INSTALL_DIR}
    DOWNLOAD_COMMAND  ""
    UPDATE_COMMAND   ""

    # building from git needs autoreconf → configure → make → install
    # (release tarballs ship with generated configure scripts,
    # but git clones require autoreconf)
    CONFIGURE_COMMAND
        # ${CMAKE_COMMAND} -E chdir ${HTSLIB_SOURCE_DIR}
        autoreconf -i
        COMMAND
        # autoheader
        # COMMAND
        # autoconf
        # COMMAND
        # We need to manually add -fPIC here, as somehow otherwise the local installation
        # won't link properly. Linking will always remain a mystery to me...
        # Need some special care to fix https://github.com/lczech/grenedalf/issues/12,
        # see https://stackoverflow.com/a/59536947 for the solution.
        # ${CMAKE_COMMAND} -E chdir ${HTSLIB_SOURCE_DIR}
        ./configure
            CFLAGS=-fPIC CXXFLAGS=-fPIC
            --prefix=${HTSLIB_INSTALL_DIR}
            --libdir=${HTSLIB_INSTALL_DIR}/lib
            --disable-multi-os-directory
            ${HTSLIB_Deflate_configure}
            # --disable-libcurl
            --disable-ref-cache

    # Didn't get these to work in the above configure... So we amend them here...
    # I think it's because htslib does some internal flag stuff as well that is not
    # cleanly implemented, see https://github.com/samtools/htslib/issues/1527
    # $(MAKE) CFLAGS+=${HTSLIB_COMPILER_FLAGS} CXXFLAGS+=${HTSLIB_COMPILER_FLAGS}
    # Nope... the above failes under clang?! Let's just deactivate for now.
    BUILD_IN_SOURCE true
    BUILD_COMMAND    $(MAKE) -j${CMAKE_BUILD_PARALLEL_LEVEL}
    INSTALL_COMMAND  $(MAKE) install

    LOG_DOWNLOAD  ON
    LOG_CONFIGURE ON
    LOG_BUILD     ON
    LOG_INSTALL   ON
)

# Set the paths so that those can be included by the targets.
# We explicitly set the static library here, so that we link against that one.
set( HTSLIB_DIR         "${HTSLIB_INSTALL_DIR}" )
set( HTSLIB_INCLUDE_DIR "${HTSLIB_DIR}/include" )
set( HTSLIB_LINK_DIR    "${HTSLIB_DIR}/lib" )
set( HTSLIB_LIBRARY     "${HTSLIB_DIR}/lib/libhts.a" )

# # We need to add a dummy IMPORTED_LIBRARY target with no validation,
# # due to the CMake configure/build separation... otherwise, it will
# # complain that the target is not existing, as it's not build yet.
# add_library(htslib_imported STATIC IMPORTED GLOBAL)
# set_target_properties(htslib_imported PROPERTIES
#     IMPORTED_LOCATION             ${HTSLIB_INSTALL_DIR}/lib/libhts.a
#     INTERFACE_INCLUDE_DIRECTORIES ${HTSLIB_INSTALL_DIR}/include
# )
# add_dependencies(htslib_imported htslib)

# # Then, add a user-facing INTERFACE library, which we can actuall link against.
# add_library(HTSlib::htslib ALIAS htslib_imported)
# # add_library(HTSlib::htslib INTERFACE)
# # target_link_libraries(HTSlib::htslib INTERFACE htslib_imported)
# # target_include_directories(HTSlib::htslib INTERFACE
# #     $<BUILD_INTERFACE:${_HTS_INSTALL}/include>
# # )

# # Previous test with a direct target
# # # Expose an IMPORTED target you can link against
# # add_library(HTSlib::htslib STATIC IMPORTED GLOBAL)
# # set_target_properties(HTSlib::htslib PROPERTIES
# #     IMPORTED_LOCATION             ${HTSLIB_INSTALL_DIR}/lib/libhts.a
# #     INTERFACE_INCLUDE_DIRECTORIES ${HTSLIB_INSTALL_DIR}/include
# # )
# # add_dependencies(HTSlib::htslib htslib)

# # Usage example
# # target_link_libraries(my_lib PRIVATE HTSlib::htslib)

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
