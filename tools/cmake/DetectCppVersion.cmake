# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2024 Lucas Czech
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
#   Detect CPP Version
# ------------------------------------------------------------------------------

# This file is included from the main CMakeLists.txt in order to figure out
# the highest standard of C++ that the compiler is capable of.
# This is meant as an automatic way to compile with the latest features if possible.

cmake_minimum_required(VERSION 3.8)
message (STATUS "Detecting C++ Standard")

# List of C++ standards and corresponding flags for testing, starting from the highest
set(CPP_VERSIONS_AND_FLAGS
    "23@-std=c++23"
    "20@-std=c++20"
    "17@-std=c++17"
    "14@-std=c++14"
    "11@-std=c++11"
)

# Function to detect the highest available C++ standard
function(detect_highest_cpp_version HIGHEST_SUPPORTED_CXX_VERSION)
    # Create a minimal C++ source code snippet for testing.
    # We also include some headers here as a super ugly ad-hoc patch for some bugs.
    # The chrono header is broken in clang 10 to 14, see https://stackoverflow.com/a/63985023/4184258
    # The regex header has issues in clang 6 to 8.
    # In both cases, compiling with an earlier version of C++ as a fallback seems to work,
    # so when including them here, compilation fails for the later versions, which is what we want.
    file(
        WRITE "${CMAKE_BINARY_DIR}/test_cpp_version.cpp"
        "#include <iostream>\n \
        #include <chrono>\n \
        #include <regex>\n \
        int main() { std::cout << \"C++ version check\" << std::endl; return 0; }\n"
    )

    # Iterate over the list of C++ versions and their corresponding and check if it is supported
    foreach(version_and_flag ${CPP_VERSIONS_AND_FLAGS})
        # Split the version_and_flag string into the version and the flag.
        # Tricky to get CMake to iterate two elements at a time... Hacky, but works.
        string(REPLACE "@" ";" version_and_flag_split ${version_and_flag})
        list(GET version_and_flag_split 0 version)
        list(GET version_and_flag_split 1 flag)
        message(STATUS "Checking support for C++${version} with flag ${flag}")

        # Try to set the standard to the current version
        # set(CMAKE_CXX_STANDARD ${version})
        # set(CMAKE_CXX_STANDARD_REQUIRED ON)
        # set(CMAKE_CXX_EXTENSIONS OFF)

        # Create a directory to store the test binary for the current version
        set(TEST_BINARY_DIR "${CMAKE_BINARY_DIR}/test_cpp_${version}")

        # Try to compile the minimal source file with the corresponding compiler flag
        try_compile(
            COMPILER_SUPPORTS_CXX${version}
            ${TEST_BINARY_DIR}
            SOURCES "${CMAKE_BINARY_DIR}/test_cpp_version.cpp"
            # CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=${flag}"
            COMPILE_DEFINITIONS "${flag}"
        )
        # CMAKE_FLAGS "-DCMAKE_CXX_STANDARD=${version}"
        #             "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
        #             "-DCMAKE_CXX_EXTENSIONS=OFF"
        # CXX_STANDARD ${version}

        # If supported, set this as the highest supported standard
        if (COMPILER_SUPPORTS_CXX${version})
            message(STATUS "Compiler supports C++${version}")
            set(${HIGHEST_SUPPORTED_CXX_VERSION} ${version} PARENT_SCOPE)
            return()
        else()
            message(STATUS "Compiler does not support C++${version}")
        endif()
    endforeach()

    message(FATAL_ERROR "No supported C++ standard found")
endfunction()

# Function to check if user-specified version is valid or auto-detect
function(detect_and_set_cpp_version)
    if (GENESIS_CPP_VERSION STREQUAL "auto")
        message(STATUS "Automatic C++ version detection enabled")
        detect_highest_cpp_version(HIGHEST_SUPPORTED_CXX_VERSION)

        # Cannot set CMAKE_CXX_STANDARD here, as CMake might not have built-in support
        # yet for all C++ versions that we are testing above. So we go the manual route...
        # set(CMAKE_CXX_STANDARD ${HIGHEST_SUPPORTED_CXX_VERSION} PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${HIGHEST_SUPPORTED_CXX_VERSION}" PARENT_SCOPE)
        SET(GENESIS_CPP_VERSION ${HIGHEST_SUPPORTED_CXX_VERSION})
    else()
        # Check if the provided version is valid and supported
        set(version_valid OFF)
        foreach(version_and_flag ${CPP_VERSIONS_AND_FLAGS})
            list(GET version_and_flag 0 version)
            if (GENESIS_CPP_VERSION STREQUAL version)
                message(STATUS "Using user-specified C++ version: C++${GENESIS_CPP_VERSION}")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${GENESIS_CPP_VERSION}" PARENT_SCOPE)
                set(version_valid ON)
                break() # Exit the loop once a valid version is found
            endif()
        endforeach()

        # If no valid version was found, throw an error
        if (NOT version_valid)
            message(FATAL_ERROR
                "Invalid C++ version specified: ${GENESIS_CPP_VERSION}. \
                Supported versions are: 11, 14, 17, 20, 23"
            )
        endif()
    endif()

    # Display final chosen C++ versionc
    message(STATUS "${ColorGreen}C++ standard set to C++${GENESIS_CPP_VERSION}${ColorEnd}")
endfunction()
