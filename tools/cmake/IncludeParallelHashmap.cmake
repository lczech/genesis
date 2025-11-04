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
#   Setup parallel-hashmap
# ------------------------------------------------------------------------------

# Check if the submodule was initialized; otherwise fetch from GitHub
message (STATUS "Looking for parallel-hashmap")
set(PARALLEL_HASHMAP_SUBMODULE_DIR ${PROJECT_SOURCE_DIR}/external/parallel-hashmap)
if (EXISTS "${PARALLEL_HASHMAP_SUBMODULE_DIR}/CMakeLists.txt")
    message(STATUS "Using parallel-hashmap submodule")
    set(PARALLEL_HASHMAP_SOURCE "${PARALLEL_HASHMAP_SUBMODULE_DIR}")
else()
    message(STATUS "Using parallel-hashmap from GitHub")
    FetchContent_Declare(
        parallel_hashmap
        GIT_REPOSITORY "https://github.com/greg7mdp/parallel-hashmap.git"
        GIT_TAG        "${parallel_hashmap_GIT_TAG}"
    )
    FetchContent_MakeAvailable(parallel_hashmap)
    set(PARALLEL_HASHMAP_SOURCE "${parallel_hashmap_SOURCE_DIR}")
endif()

# Header only, so we just need to include the dir.
include_directories("${PARALLEL_HASHMAP_SOURCE}")

# Make a target to link to, and to propagate the include dir
add_library(parallel_hashmap INTERFACE)
target_include_directories(
    parallel_hashmap INTERFACE
    $<BUILD_INTERFACE:${PARALLEL_HASHMAP_SOURCE}>
)
