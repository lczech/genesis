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

# Check for Python libs. Pybind does not check for the python dev packages, so we need to do this.
message (STATUS "Looking for PythonLibs")
find_package (PythonLibs 2.7)

if( NOT PYTHONLIBS_FOUND )
    message (STATUS "PythonLibs not found")
    message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
    message (STATUS "${ColorYellow}Try installing the 'python-dev' or 'python2.7-dev' packages "
    "first.${ColorEnd}")
    return()
endif()

message (STATUS "Found PythonLibs: ${PYTHONLIBS_VERSION_STRING}")

# ------------------------------------------------------------------------------
#   Setup Pybind11
# ------------------------------------------------------------------------------

# CMake version check first.
if( ${CMAKE_VERSION} VERSION_LESS 2.8.12 )
    message (STATUS "Pybind11 requires at least CMake version 2.8.12")
    message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
    return()
endif()

# As Pybind11 is a header-only library, we simply check for its existence.
message (STATUS "Looking for Pybind11")

IF( NOT EXISTS ${PROJECT_SOURCE_DIR}/tools/pybind11/include/pybind11/pybind11.h )
    message (STATUS "Pybind11 not found")
    message (STATUS "Will now download Pybind11")

    # If Pybind11 was not found, we download and unpack it (at configure time). This roughly follows
    # https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
    # See also the CMakeLists.txt file for the tests - there we use this technique for GTest.

    configure_file(
        ${PROJECT_SOURCE_DIR}/tools/cmake/Pybind11Download.cmake
        ${CMAKE_BINARY_DIR}/pybind11-download/CMakeLists.txt
    )

    execute_process( COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11-download
    )

    if(result)
        message (STATUS "Cannot configure Pybind11: ${result}")
        message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    endif()

    execute_process( COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11-download
    )

    if(result)
        message (STATUS "Cannot build Pybind11: ${result}")
        message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    endif()

    # If the header still does not exists, something went wrong.
    IF( NOT EXISTS ${PROJECT_SOURCE_DIR}/tools/pybind11/include/pybind11/pybind11.h )
        message (STATUS "Pybind11 not found")
        message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
        return()
    ENDIF()

    message (STATUS "Finished downloading Pybind11")
ENDIF()

# Include Pybind11, as documented in http://pybind11.readthedocs.io/en/master/compiling.html#building-with-cmake
# We use the two-args version of add_subdirectory here, because it is not an actual subdirectory.
add_subdirectory( ${PROJECT_SOURCE_DIR}/tools/pybind11 ${CMAKE_CURRENT_BINARY_DIR}/pybind11 )

# Get the Pybind version, which is currently not exported directly...
SET(PYBIND11_VERSION ${PYBIND11_VERSION_MAJOR}.${PYBIND11_VERSION_MINOR}.${PYBIND11_VERSION_PATCH})

message (STATUS "Found Pybind11: ${PYBIND11_VERSION}")
message (STATUS "${ColorBlue}Building Python module${ColorEnd}")

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

# Path to store the Python module.
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/python)

# Set rpath for all targets, so that they work out of the box (libgenesis.so in parent directory),
# as well as when relocating with the lib being in the same directory.
# Also, we need to set two versions of ORIGIN here (one and two dollar signs).
# This is because on some systems it seems to be escaped, on some not...
# See https://cmake.org/pipermail/cmake/2008-January/019290.html
# and https://cmake.org/Wiki/CMake_RPATH_handling for details.
# Tipp: Use `ldd binary` and `readelf -d binary` to check RPATH settings.
set( CMAKE_INSTALL_RPATH "$ORIGIN/..:$$ORIGIN/..:$ORIGIN:$$ORIGIN" )
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )

# No need to link to Python - this is done by Pybind. We keep it here for reference.
# include_directories   (${PYTHON_INCLUDE_DIRS})
# link_directories      (${PYTHON_LIBRARIES})

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

IF( UNITY_BUILD )
    # See main CMake file for the monolith macro.
    ASSEMBLE_MONOLITH( ${PROJECT_SOURCE_DIR}/python/src "${UNITY_BUILD}" "python" genesis_python_sources )
ELSE()
    file (GLOB_RECURSE genesis_python_sources ${PROJECT_SOURCE_DIR}/python/src/*.cpp)
ENDIF()

include_directories (${PROJECT_SOURCE_DIR}/python)
include_directories (${PROJECT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
#   Build Python Module
# ------------------------------------------------------------------------------

# Add the Pybind module, which is the equivalent of add_library(...).
pybind11_add_module( genesis_python_module MODULE ${genesis_python_sources} )

# Make sure that all Pybind things are processed before.
add_dependencies( genesis_python_module pybind11 )

# Link against our library.
target_link_libraries (genesis_python_module PRIVATE genesis_lib_shared)

# No need to do this manually, Pybind takes care of this. Keep it for reference.
# target_link_libraries (genesis_python_module ${PYTHON_LIBRARIES})

# Make sure that the module file is named according to Python standards
# (that is, no leading "lib", but don't change automatic suffix for shared libraries, e.g., ".so").
set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME genesis)
set_target_properties (genesis_python_module PROPERTIES PREFIX "")
set_target_properties (genesis_python_module PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")

# add_custom_command (OUTPUT python_init COMMAND
#     touch ${PROJECT_SOURCE_DIR}/bin/python/__init__.py
# )

if(THREADS_FOUND)
    target_link_libraries (genesis_python_module PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()
