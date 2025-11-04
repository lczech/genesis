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

# ------------------------------------------------------------------------------
#   Setup CLI11
# ------------------------------------------------------------------------------

# Check if the submodule was initialized; otherwise fetch from GitHub
message (STATUS "Looking for CLI11")
set(CLI11_SUBMODULE_DIR ${PROJECT_SOURCE_DIR}/external/CLI11)
if (EXISTS "${CLI11_SUBMODULE_DIR}/CMakeLists.txt")
    message(STATUS "Using CLI11 submodule")
    set(CLI11_SOURCE "${CLI11_SUBMODULE_DIR}")
else()
    message(STATUS "Using CLI11 from GitHub")
    FetchContent_Declare(
        CLI11
        GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
        GIT_TAG        "${CLI11_GIT_TAG}"
    )
    FetchContent_MakeAvailable(CLI11)
    set(CLI11_SOURCE "${CLI11_SOURCE_DIR}")
endif()

# Header only, so we just need to include the dir.
include_directories("${CLI11_SOURCE}/include")
# Include as: #include "CLI/CLI.hpp"

# Make a target to link to, and to propagate the include dir
add_library(CLI11 INTERFACE)
target_include_directories(
    CLI11 INTERFACE
    $<BUILD_INTERFACE:${CLI11_SOURCE}/include>
)
