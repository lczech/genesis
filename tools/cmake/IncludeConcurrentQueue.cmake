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
#   Setup concurrentqueue
# ------------------------------------------------------------------------------

# Check if the submodule was initialized; otherwise fetch from GitHub
message (STATUS "Looking for concurrentqueue")
set(CONCURRENTQUEUE_SUBMODULE_DIR ${PROJECT_SOURCE_DIR}/external/concurrentqueue)
if (EXISTS "${CONCURRENTQUEUE_SUBMODULE_DIR}/CMakeLists.txt")
    message(STATUS "Using concurrentqueue submodule")
    set(CONCURRENTQUEUE_SOURCE "${CONCURRENTQUEUE_SUBMODULE_DIR}")
else()
    message(STATUS "Using concurrentqueue from GitHub")
    FetchContent_Declare(
        moodycamel_concurrentqueue
        GIT_REPOSITORY "https://github.com/cameron314/concurrentqueue.git"
        GIT_TAG        "${concurrentqueue_GIT_TAG}"
        SOURCE_DIR     "${CMAKE_BINARY_DIR}/concurrentqueue"
    )
    FetchContent_MakeAvailable(moodycamel_concurrentqueue)
    set(CONCURRENTQUEUE_SOURCE "${moodycamel_concurrentqueue_SOURCE_DIR}")
endif()

# Copy headers under a moodycamel/ folder in the build tree;
# this lets us do: #include "moodycamel/blockingconcurrentqueue.h"
# Bit cunbersome, but seems cleaner to use than without the path prefix.
set(GEN_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/include")
set(CONCURRENTQUEUE_HEADER_SUBDIR "${GEN_INCLUDE_DIR}/moodycamel")
file(MAKE_DIRECTORY "${CONCURRENTQUEUE_HEADER_SUBDIR}")
file(GLOB MOODY_HEADERS "${CONCURRENTQUEUE_SOURCE}/*.h" )
foreach(_hdr IN LISTS MOODY_HEADERS)
    get_filename_component(_name "${_hdr}" NAME)
    configure_file(
        "${_hdr}"
        "${CONCURRENTQUEUE_HEADER_SUBDIR}/${_name}"
        COPYONLY
    )
endforeach()

add_library(moodycamel_concurrentqueue INTERFACE)
target_include_directories(moodycamel_concurrentqueue
    INTERFACE
    $<BUILD_INTERFACE:${GEN_INCLUDE_DIR}>
)
include_directories( ${GEN_INCLUDE_DIR} )

# alias it so folks can write moodycamel::concurrentqueue
# add_library(moodycamel::concurrentqueue ALIAS moodycamel_concurrentqueue)

# Usage:
#   target_link_libraries(my_lib PRIVATE moodycamel::ConcurrentQueue)
#   #include <moodycamel/blockingconcurrentqueue.h>

# ------------------------------------------------------------------------------
#   Setup concurrentqueue
# ------------------------------------------------------------------------------

# Alternative which does not use a subdirectory `moodycamel` for the headers,
# so that the cpp include would just use `#include "blockingconcurrentqueue.h"`
# It's okay, but not ideal if we want clarity. Hence the above fancier version.

# # Check if the submodule was initialized; otherwise fetch from GitHub
# message (STATUS "Looking for concurrentqueue")
# set(CONCURRENTQUEUE_SUBMODULE_DIR ${PROJECT_SOURCE_DIR}/external/concurrentqueue)
# if (EXISTS "${CONCURRENTQUEUE_SUBMODULE_DIR}/CMakeLists.txt")
#     message(STATUS "Using concurrentqueue submodule")
#     add_subdirectory(
#         "${CONCURRENTQUEUE_SUBMODULE_DIR}"
#         "${CMAKE_BINARY_DIR}/concurrentqueue-install"
#     )
#     set(CONCURRENTQUEUE_SOURCE_DIR "${CONCURRENTQUEUE_SUBMODULE_DIR}")
# else()
#     message(STATUS "concurrentqueue submodule not found")
#     message(STATUS "Fetching concurrentqueue from GitHub")
#     FetchContent_Declare(
#         concurrentqueue
#         GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
#         GIT_TAG        ${concurrentqueue_GIT_TAG}
#     )
#     # FetchContent_GetProperties(concurrentqueue)
#     # if (NOT concurrentqueue_POPULATED)
#     #     # does the clone at configure time
#     #     FetchContent_Populate(concurrentqueue)
#     # endif()

#     # copy into SOURCE_DIR if missing, then add_subdirectory() for you
#     FetchContent_MakeAvailable(concurrentqueue)
#     set(CONCURRENTQUEUE_SOURCE_DIR ${concurrentqueue_SOURCE_DIR})
#     message (STATUS "Finished downloading concurrentqueue")
# endif()

# # Sanity check
# if (NOT EXISTS "${CONCURRENTQUEUE_SOURCE_DIR}/CMakeLists.txt")
#     message(
#         FATAL_ERROR
#         "concurrentqueue source not found in ${CONCURRENTQUEUE_SOURCE_DIR}"
#     )
# endif()

# add_library(moodycamel::ConcurrentQueue ALIAS concurrentqueue)
# message (STATUS "Found concurrentqueue")

# # add_library(concurrentqueue INTERFACE)
# # target_include_directories(concurrentqueue INTERFACE
# #     $<BUILD_INTERFACE:${CONCURRENTQUEUE_SOURCE_DIR}>
# # )

# # add_subdirectory( "${CONCURRENTQUEUE_SOURCE_DIR}" )
# # include_directories( "${CONCURRENTQUEUE_SOURCE_DIR}" )

# # Usage
# # target_link_libraries(my_lib PRIVATE moodycamel::ConcurrentQueue)
