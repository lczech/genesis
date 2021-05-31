# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2021 Lucas Czech
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
#   Extra Warnings
# ------------------------------------------------------------------------------

# Apple osx clang >= 9.3, as well as new gcc versions >= 9 spam a lot of warnings in googletest
# about implicitly-declared types being deprecated. Let's silence them, as we are not interested
# in warnings about googletest. That's something they have to fix!
# Also, we have to use the double cmake version comparison here, as old osx cmake versions
# do not yet have VERSION_GREATER_EQUAL, and crash otherwise...
SET( GTEST_NO_WARN_DEPRECATED_COPY OFF )
IF(
    (APPLE AND
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND
    ((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9.3") OR
    (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9.3")))
    OR
    ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND
    ((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9") OR
    (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9")))
)
    SET( GTEST_NO_WARN_DEPRECATED_COPY ON )
    message (STATUS "Deactivate -Wdeprecated-copy for test binary.")
ENDIF()
# message (STATUS "GTEST_NO_WARN_DEPRECATED_COPY: ${GTEST_NO_WARN_DEPRECATED_COPY}")

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
    # We only want test, not mock.
    set(BUILD_GTEST ON  CACHE BOOL "Builds the googletest subproject")
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
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

    if(GTEST_NO_WARN_DEPRECATED_COPY)
        target_compile_options(gtest      PRIVATE "-Wno-deprecated-copy")
        target_compile_options(gtest_main PRIVATE "-Wno-deprecated-copy")
        # target_compile_options(gmock      PRIVATE "-Wno-deprecated-copy")
        # target_compile_options(gmock_main PRIVATE "-Wno-deprecated-copy")
    endif()

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

IF( GENESIS_UNITY_BUILD )
    # See main CMake file for the monolith macro.
    ASSEMBLE_MONOLITH( ${PROJECT_SOURCE_DIR}/test/src "${GENESIS_UNITY_BUILD}" "test" genesis_test_sources )
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

if(GTEST_NO_WARN_DEPRECATED_COPY)
    target_compile_options(genesis_tests PRIVATE "-Wno-deprecated-copy")
endif()

target_link_libraries (genesis_tests genesis_lib_shared)
target_link_libraries (genesis_tests ${GTEST_BOTH_LIBRARIES})

# Link against any external libraries, e.g. Pthreads.
target_link_libraries (genesis_tests ${GENESIS_INTERNAL_LINK_LIBRARIES})
IF(GENESIS_USE_HTSLIB AND NOT GENESIS_SYSTEM_HTSLIB)
    add_dependencies( genesis_tests htslib )
ENDIF()

# Need to specify the whole path here. For some reason, cmake does not
# extract it from the executable name here...
add_test (genesis_tests ${PROJECT_SOURCE_DIR}/bin/test/genesis_tests)
