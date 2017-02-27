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
#   Info
# ------------------------------------------------------------------------------

#     This script is used for downloading Google Test if it is not found.
#     It is used by CMakeLists.txt - see there for details.

# ------------------------------------------------------------------------------
#   GTest Download
# ------------------------------------------------------------------------------

# This min requirement is less than what we expect in the main CMakeList file,
# so we should be good. We state it here for re-use of this script.
cmake_minimum_required( VERSION 2.8.2 )

project( googletest-download NONE )

include(ExternalProject)

# The download progress is ugly and not needed. Since CMake 3.1, we can disable it.
IF( ${CMAKE_VERSION} VERSION_GREATER 3.1 )
    SET( CMAKE_DOWNLOAD_PROGRESS "DOWNLOAD_NO_PROGRESS 1" )
ENDIF()

# Download a fixed release instead of the current master,
# so that we know that it works for us.
ExternalProject_Add( googletest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    ${CMAKE_DOWNLOAD_PROGRESS}
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
)

# Alternative version that needs Git to be installed (thus, we do not use it by default).
# We use Google Test release 1.8.0, which points to the commit ec44c6c1675c25b9827aacd08c02433cccde7780

# ExternalProject_Add(googletest
#   GIT_REPOSITORY    https://github.com/google/googletest.git
#   GIT_TAG           ec44c6c1675c25b9827aacd08c02433cccde7780
#   SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
#   BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ""
#   INSTALL_COMMAND   ""
#   TEST_COMMAND      ""
# )
