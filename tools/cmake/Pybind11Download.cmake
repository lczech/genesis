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

#     This script is used for downloading Pybind11 if it is not found.
#     It is used by CMakeLists.txt - see there for details.

# ------------------------------------------------------------------------------
#   Pybind11 Download
# ------------------------------------------------------------------------------

# This min requirement is less than what we expect in the main CMakeList file,
# so we should be good. We state it here for re-use of this script.
cmake_minimum_required( VERSION 2.8.2 )

project( pybind11-download NONE )

include(ExternalProject)

# The download progress is ugly and not needed. Since CMake 3.1, we can disable it.
IF( ${CMAKE_VERSION} VERSION_GREATER 3.1 )
    SET( CMAKE_DOWNLOAD_PROGRESS "DOWNLOAD_NO_PROGRESS 1" )
ENDIF()

# TODO The last release of Pybind has a problem with LTO flags, so we pull a recent non-release commit of Pybind instead.
# TODO Once this fix is part of a release, change to a proper release commit here.

# Download a fixed release instead of the current master,
# so that we know that it works for us.
ExternalProject_Add( pybind11
    # URL https://github.com/pybind/pybind11/archive/v2.0.1.zip
    URL https://github.com/pybind/pybind11/archive/5fe9908b7a34e4f4e320e319a528c4f1bdf6a8e6.zip
    ${CMAKE_DOWNLOAD_PROGRESS}
    SOURCE_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
    BINARY_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
)

# Alternative version that needs Git to be installed (thus, we do not use it by default).
# We use Pybind11 release 2.0.1, which points to the commit 07de0d8627101be53986e841cd4e21ee38c2498a

# ExternalProject_Add( pybind11
#   GIT_REPOSITORY    https://github.com/pybind/pybind11.git
#   GIT_TAG           07de0d8627101be53986e841cd4e21ee38c2498a
#   SOURCE_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
#   BINARY_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ""
#   INSTALL_COMMAND   ""
#   TEST_COMMAND      ""
# )
