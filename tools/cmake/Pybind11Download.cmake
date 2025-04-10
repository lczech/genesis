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
# Lucas Czech <lczech@carnegiescience.edu>
# Department of Plant Biology, Carnegie Institution For Science
# 260 Panama Street, Stanford, CA 94305, USA

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
cmake_minimum_required(VERSION 3.8)

project( pybind11-download NONE )

include(ExternalProject)

# The download progress is ugly and not needed. Since CMake 3.1, we can disable it.
IF( ${CMAKE_VERSION} VERSION_GREATER 3.1 )
    SET( CMAKE_DOWNLOAD_PROGRESS "DOWNLOAD_NO_PROGRESS 1" )
ENDIF()

# Download a fixed release instead of the current master,
# so that we know that it works for us.
ExternalProject_Add( pybind11
    URL https://github.com/pybind/pybind11/archive/v2.2.4.zip
    # URL https://github.com/pybind/pybind11/archive/9a19306fbf30642ca331d0ec88e7da54a96860f9.zip
    SOURCE_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
    BINARY_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
    ${CMAKE_DOWNLOAD_PROGRESS}
)

# Alternative version that needs Git to be installed (thus, we do not use it by default).

# ExternalProject_Add( pybind11
#   GIT_REPOSITORY    https://github.com/pybind/pybind11.git
#   GIT_TAG           9a19306fbf30642ca331d0ec88e7da54a96860f9
#   SOURCE_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
#   BINARY_DIR        "${PROJECT_SOURCE_DIR}/tools/pybind11"
#   CONFIGURE_COMMAND ""
#   BUILD_COMMAND     ""
#   INSTALL_COMMAND   ""
#   TEST_COMMAND      ""
# )
