# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2016 Lucas Czech
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
#   Setup
# ------------------------------------------------------------------------------

find_package (GTest)

if(NOT GTEST_FOUND)
    message (STATUS "Gtest not found")
    message (WARNING "${ColorRed}Cannot build tests.${ColorEnd}")
    return()
endif()

message (STATUS "Found Gtest")
message (STATUS "${ColorBlue}Building tests${ColorEnd}")

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/bin)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files. The CMake documentation does not recommend this,
# but for our purposes, this is good enough for now.
file (GLOB_RECURSE genesis_test_sources ${PROJECT_SOURCE_DIR}/test/src/*.cpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/test/src)

# ------------------------------------------------------------------------------
#   Build Tests
# ------------------------------------------------------------------------------

include_directories   (${GTEST_INCLUDE_DIRS})

add_executable        (genesis_tests ${genesis_test_sources})
set_target_properties (genesis_tests PROPERTIES OUTPUT_NAME test_suite)

target_link_libraries (genesis_tests genesis_lib_shared)
target_link_libraries (genesis_tests ${GTEST_BOTH_LIBRARIES} -pthread)

# Need to specify the whole path here. For some reason, cmake does not
# extract it from the executable name here...
add_test (test_suite ${PROJECT_SOURCE_DIR}/test/bin/test_suite)
