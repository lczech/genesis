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
#   Setup Python
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

message (STATUS "Looking for PythonLibs")
find_package (PythonLibs 2.7)

if( NOT PYTHONLIBS_FOUND )
    message (STATUS "PythonLibs not found")
    message (WARNING "${ColorRed}Cannot build Python module${ColorEnd}")
    return()
endif()

message (STATUS "Found PythonLibs")

# ------------------------------------------------------------------------------
#   Setup Pybind11
# ------------------------------------------------------------------------------

# As Pybind11 is a header-only library, we simply check for its existence.
message (STATUS "Looking for Pybind11")

IF( NOT EXISTS ${PROJECT_SOURCE_DIR}/python/pybind11/include/pybind11/pybind11.h )
    message (STATUS "Pybind11 not found")
    message (STATUS "Will now download Pybind11")

    # If Pybind11 was not found, we download and unpack it (at configure time). This roughly follows
    # https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
    # See also the CMakeLists.txt file for the tests - there we use this technique for GTest.

    configure_file(
        ${PROJECT_SOURCE_DIR}/python/Pybind11Download.cmake
        ${CMAKE_BINARY_DIR}/pybind11-download/CMakeLists.txt
    )

    execute_process( COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11-download
    )

    if(result)
        message (STATUS "Cannot configure Pybind11: ${result}")
        message (WARNING "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    endif()

    execute_process( COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11-download
    )

    if(result)
        message (STATUS "Cannot build Pybind11: ${result}")
        message (WARNING "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    endif()

    # If the header still does not exists, something went wrong.
    IF( NOT EXISTS ${PROJECT_SOURCE_DIR}/python/pybind11/include/pybind11/pybind11.h )
        message (STATUS "Pybind11 not found")
        message (WARNING "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    ENDIF()

    message (STATUS "Finished downloading Pybind11")
ENDIF()

# Include Pybind11, as recommended in http://pybind11.readthedocs.io/en/master/compiling.html#building-with-cmake
add_subdirectory(pybind11)

message (STATUS "Found Pybind11")
message (STATUS "${ColorBlue}Building Python module${ColorEnd}")

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/python/module)

# Need to set rpath for all targets for it to work.
# Also, we need to set two versions of ORIGIN here (one and two dollar signs).
# This is because on some systems it seems to be escaped, on some not...
set( CMAKE_INSTALL_RPATH "$ORIGIN:$$ORIGIN" )
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files. The CMake documentation does not recommend this,
# but for our purposes, this is good enough for now.
file (GLOB_RECURSE genesis_python_sources ${PROJECT_SOURCE_DIR}/python/src/*.cpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR})

# ------------------------------------------------------------------------------
#   Build Python Module
# ------------------------------------------------------------------------------

pybind11_add_module( genesis_python_module ${genesis_python_sources} )

# include_directories   (${PYTHON_INCLUDE_DIRS})
# link_directories      (${PYTHON_LIBRARIES})
#
# add_library           (genesis_python_module MODULE ${genesis_python_sources})
#
# add_dependencies( genesis_python_module pybind11 )

target_link_libraries (genesis_python_module genesis_lib_shared)
# target_link_libraries (genesis_python_module ${PYTHON_LIBRARIES})

set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME genesis)
set_target_properties (genesis_python_module PROPERTIES PREFIX "")
set_target_properties (genesis_python_module PROPERTIES INSTALL_RPATH "$ORIGIN:$$ORIGIN")

# set_target_properties (genesis_python_module PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/module
# )

# add_custom_command (OUTPUT python_init COMMAND
#     touch ${PROJECT_SOURCE_DIR}/module/__init__.py
# )

if(THREADS_FOUND)
    target_link_libraries (genesis_python_module ${CMAKE_THREAD_LIBS_INIT})
endif()
