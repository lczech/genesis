# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2022 Lucas Czech
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
#   Use samtools
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to build samtools.

message (STATUS "Looking for samtools")

# We download and built on our own, using the correct commit hash to get our exact desired
# version, and install locally to the build directory, using the local htslib as well.
ExternalProject_Add(
    samtools
    PREFIX ""

    # Download and Update Steps
    # DOWNLOAD_COMMAND  ""
    # GIT_REPOSITORY https://github.com/samtools/samtools.git
    # GIT_TAG ${samtools_COMMIT_HASH}
    URL https://github.com/samtools/samtools/releases/download/${GENESIS_SAMTOOLS_VERSION}/samtools-${GENESIS_SAMTOOLS_VERSION}.tar.bz2
    DOWNLOAD_NO_PROGRESS TRUE
    UPDATE_COMMAND    ""

    # Configure Step. See samtools/INSTALL
    # We need to manually add -fPIC here, as somehow otherwise the local installation
    # won't link properly. Linking will always remain a mystery to me...
    # Furthermore, we need to have the source files in a subdirectory named "samtools",
    # so that this name can be used for include directives from our cpp sources - because samtools
    # itself does not have its headers in a subdirectory of that name. Bodging, as always... :-(
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-samtools-source/samtools
    CONFIGURE_COMMAND
        autoheader
        COMMAND
        autoconf -Wno-syntax
        COMMAND
        ./configure CFLAGS=-fPIC CXXFLAGS=-fPIC --prefix=${CMAKE_CURRENT_BINARY_DIR}/genesis-samtools --with-htslib=${HTSLIB_DIR}
        #--without-curses

    # Build Step
    # BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-samtools
    BUILD_IN_SOURCE true
    BUILD_COMMAND
        $(MAKE)

    # Install Step
    INSTALL_COMMAND $(MAKE) install
)

# Set the paths so that those can be included by the targets.
# We explicitly set the static libraries here, so that we link against that one.
set( SAMTOOLS_DIR ${CMAKE_CURRENT_BINARY_DIR}/genesis-samtools-source )
set( SAMTOOLS_INCLUDE_DIR "${SAMTOOLS_DIR}" )
set( SAMTOOLS_LINK_DIR    "${SAMTOOLS_DIR}" )
set( SAMTOOLS_SOURCE_DIR  "${SAMTOOLS_DIR}/samtools" )
set( SAMTOOLS_LIBRARY     "${SAMTOOLS_DIR}/samtools/libbam.a" "${SAMTOOLS_DIR}/samtools/libst.a" )

# Useful output for debugging
message( STATUS "SAMTOOLS_INCLUDE_DIR: ${SAMTOOLS_INCLUDE_DIR}" )
message( STATUS "SAMTOOLS_LINK_DIR:    ${SAMTOOLS_LINK_DIR}" )
message( STATUS "SAMTOOLS_LIBRARY:     ${SAMTOOLS_LIBRARY}" )
# message( STATUS "SAMTOOLS_LZMA:        ${SAMTOOLS_LZMA}" )
# message( STATUS "SAMTOOLS_BZ2:         ${SAMTOOLS_BZ2}" )

# Technically, we cannot claim to have "found" the library here, as it won't yet be compiled...
# But let's stick to the user output that we used for the other dependencies,
# and assume that once we are here, the build process will work.
message( STATUS "Found samtools: ${SAMTOOLS_LIBRARY}" )
message (STATUS "${ColorGreen}Using samtools${ColorEnd}")

# Include the headers and directories, so that they are found by our source.
include_directories(${SAMTOOLS_INCLUDE_DIR})
link_directories(${SAMTOOLS_LINK_DIR})

# Make sure that htslib is built first.
add_dependencies( samtools htslib )

# Set needed definitions and linker flags for genesis and for the parent scope.
# The targets (our shared and static lib) will use these to link against.
set( GENESIS_INCLUDE_DIR ${GENESIS_INCLUDE_DIR} ${SAMTOOLS_INCLUDE_DIR} )
add_definitions( "-DGENESIS_SAMTOOLS" )
set( GENESIS_DEFINITIONS ${GENESIS_DEFINITIONS} " -DGENESIS_SAMTOOLS" )
set(
    GENESIS_INTERNAL_LINK_LIBRARIES ${GENESIS_INTERNAL_LINK_LIBRARIES}
    ${SAMTOOLS_LIBRARY} z -lz
)
