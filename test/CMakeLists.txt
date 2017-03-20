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
#   Setup GTest
# ------------------------------------------------------------------------------

# Prefer to use our local installation of GTest, if there is one.
SET( GTEST_ROOT ${PROJECT_SOURCE_DIR}/tools/googletest )

# Find GTest!
message (STATUS "Looking for GTest")
find_package (GTest)

if(NOT GTEST_FOUND)
    message (STATUS "GTest not found")
    message (STATUS "Will now download GTest")

    # If GTest was not found, we download and unpack it (at configure time). This roughly follows
    # https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project

    configure_file(
        ${PROJECT_SOURCE_DIR}/tools/cmake/GTestDownload.cmake
        ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt
    )

    execute_process( COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
    )

    if(result)
        message (STATUS "Cannot configure GTest: ${result}")
        message (STATUS "${ColorRed}Cannot build tests${ColorEnd}")
        return()
    endif()

    execute_process( COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
    )

    if(result)
        message (STATUS "Cannot build GTest: ${result}")
        message (STATUS "${ColorRed}Cannot build tests${ColorEnd}")
        return()
    endif()

    # Prevent overriding the parent project's compiler/linker settings on Windows.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines the gtest and gtest_main targets.
    add_subdirectory( ${CMAKE_BINARY_DIR}/googletest-src  ${CMAKE_BINARY_DIR}/googletest-build )

    # The gtest/gtest_main targets carry header search path dependencies automatically when using
    # CMake 2.8.11 or later. Otherwise we have to add them here ourselves.
    if (${CMAKE_VERSION} VERSION_LESS 2.8.11)
        include_directories( SYSTEM "${gtest_SOURCE_DIR}/include")
    endif()

    # Set targets to what they would be if we already had GTest. This way, we can use the same
    # target_link_libraries command.
    SET( GTEST_BOTH_LIBRARIES gtest_main gtest )
    SET( GTEST_LIBRARIES gtest )

    message (STATUS "Finished downloading GTest")
endif()

message (STATUS "Found GTest: ${GTEST_LIBRARIES}")
message (STATUS "${ColorBlue}Building tests${ColorEnd}")

# We include the GTest dirs as SYSTEM, as they are currently producing ugly warnings that spam
# our build output unnecessarily.
include_directories( SYSTEM ${GTEST_INCLUDE_DIRS} )

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/test)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

IF( UNITY_BUILD )
    # See main CMake file for the monolith macro.
    ASSEMBLE_MONOLITH( ${PROJECT_SOURCE_DIR}/test/src "${UNITY_BUILD}" "test" genesis_test_sources )
ELSE()
    file (GLOB_RECURSE genesis_test_sources ${PROJECT_SOURCE_DIR}/test/src/*.cpp)
ENDIF()

include_directories (${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR}/test)

# ------------------------------------------------------------------------------
#   Build Tests
# ------------------------------------------------------------------------------

add_executable        (genesis_tests ${genesis_test_sources})
set_target_properties (genesis_tests PROPERTIES OUTPUT_NAME genesis_tests)

target_link_libraries (genesis_tests genesis_lib_shared)
target_link_libraries (genesis_tests ${GTEST_BOTH_LIBRARIES})

if(THREADS_FOUND)
    target_link_libraries (genesis_tests ${CMAKE_THREAD_LIBS_INIT})
endif()

# Need to specify the whole path here. For some reason, cmake does not
# extract it from the executable name here...
add_test (genesis_tests ${PROJECT_SOURCE_DIR}/bin/test/genesis_tests)
