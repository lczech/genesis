# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech
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

cmake_minimum_required(VERSION 3.15)

# ================================================================================================
#   Setup pybind11
# ================================================================================================

# We use a specific tag, to ensure compatibility
message (STATUS "Looking for pybind11")
SET( pybind11_GIT_TAG "v2.13.6" ) #pybind11_GIT_TAG#

# Get the pybind sources either from the submodule,
# or if that is not available, fetch it from GitHub
set(PYBIND11_SUBMODULE_DIR ${PROJECT_SOURCE_DIR}/external/pybind11)
if (EXISTS "${PYBIND11_SUBMODULE_DIR}/CMakeLists.txt")
    message(STATUS "Using pybind11 submodule")
    set(PYBIND11_SOURCE "${PYBIND11_SUBMODULE_DIR}")
else()
    message(STATUS "Fetching pybind11 from GitHub")
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY "https://github.com/RosettaCommons/pybind11.git"
        GIT_TAG        "${pybind11_GIT_TAG}"
    )
    FetchContent_MakeAvailable(pybind11)
    set(PYBIND11_SOURCE "${pybind11_SOURCE_DIR}")
endif()

# Sanity check: by now ${pybind11_SOURCE_DIR} contains the full pybind11 tree
if( EXISTS "${PYBIND11_SOURCE}/CMakeLists.txt")
    message(STATUS "PYBIND11_SOURCE: ${PYBIND11_SOURCE}")
else()
    message (STATUS "${ColorRed}Cannot build Python module${ColorEnd}")
    message(FATAL_ERROR "pybind11 not found in ${PYBIND11_SOURCE}")
endif()

# Make the headers available for our C++ includes
include_directories("${PYBIND11_SOURCE}/include")

# Make the pybind module available
cmake_minimum_required(VERSION 3.5)
add_subdirectory("${PYBIND11_SOURCE}" "{CMAKE_CURRENT_BINARY_DIR}/pybind11")

# User output for debugging etc
message(STATUS  "pybind11_FOUND:        ${pybind11_FOUND}" )
message(STATUS  "pybind11_VERSION:      ${pybind11_VERSION} ${pybind11_VERSION_TYPE}" )
message(STATUS  "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS} ; ${pybind11_INCLUDE_DIR}" )
message(STATUS  "pybind11_DEFINITIONS:  ${pybind11_DEFINITIONS}" )
message(STATUS  "pybind11_LIBRARIES:    ${pybind11_LIBRARIES}" )
message (STATUS "${ColorBlue}Building Python module${ColorEnd}")

# ================================================================================================
#   Build Bindings
# ================================================================================================

# Find all binding sources, in the generated ones and in our custom ones
IF( GENESIS_UNITY_BUILD )
    # See main CMake file for the monolith macro.
    ASSEMBLE_MONOLITH(
        "${PROJECT_SOURCE_DIR}/python/bindings"
        "${GENESIS_UNITY_BUILD}"
        "python-bindings"
        genesis_python_bindings_sources
    )
    ASSEMBLE_MONOLITH(
        "${PROJECT_SOURCE_DIR}/python/custom"
        "${GENESIS_UNITY_BUILD}"
        "python-custom"
        genesis_python_custom_sources
    )
ELSE()
    file (GLOB_RECURSE genesis_python_bindings_sources ${PROJECT_SOURCE_DIR}/python/bindings/*.cpp)
    file (GLOB_RECURSE genesis_python_custom_sources   ${PROJECT_SOURCE_DIR}/python/custom/*.cpp)
ENDIF()

include_directories(${PROJECT_SOURCE_DIR}/python)
include_directories(${PROJECT_SOURCE_DIR}/lib)

# include_directories(binder/source)
# include_directories(/usr/include/pybind11)
# include_directories(/usr/include/python3.12)

# ------------------------------------------------------------------------------
#   Module
# ------------------------------------------------------------------------------

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)

# Add the Pybind module, which is the equivalent of add_library(...).
pybind11_add_module(
    genesis_python_module MODULE
    ${genesis_python_bindings_sources}
    ${genesis_python_custom_sources}
)

# Link against the object library, so that all code is made part of the module.
target_link_libraries( genesis_python_module PRIVATE genesis_lib_obj )
# target_link_libraries( genesis_python_module PRIVATE genesis_lib_shared )
# add_dependencies( genesis_python_module pybind11 )

# Link against any external libraries used by genesis.
target_link_libraries (genesis_python_module PRIVATE ${GENESIS_INTERNAL_LINK_LIBRARIES})

# Link against Python and pybind11 for module init functionality
target_link_libraries( genesis_python_module PRIVATE pybind11::module )
# target_link_libraries( genesis_python_module PRIVATE Python3::Python )

# No need to do this manually, Pybind takes care of this. Keep it for reference.
# target_link_libraries( genesis_python_module ${PYTHON_LIBRARIES} )

# No need to link to Python - this is done by Pybind. We keep it here for reference.
# include_directories   (${PYTHON_INCLUDE_DIRS})
# link_directories      (${PYTHON_LIBRARIES})

# ------------------------------------------------------------------------------
#   Output
# ------------------------------------------------------------------------------

# Path to store the Python module.
set_target_properties(genesis_python_module PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/python"
)
# set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/python)

# Make sure that the module file is named according to Python standards
# (that is, no leading "lib", but don't change automatic suffix for shared libraries, e.g., ".so").
# Remove the default "lib" prefix and enforce .so suffix so Python can import it directly.
set_target_properties( genesis_python_module PROPERTIES OUTPUT_NAME genesis )
set_target_properties( genesis_python_module PROPERTIES PREFIX "" )
set_target_properties( genesis_python_module PROPERTIES SUFFIX ".so" )
# set_target_properties( genesis_python_module PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}" )

# Set rpath for all targets, so that they work out of the box if we want to link
# against libgenesis.so in parent directory, as well as when relocating with
# the lib being in the same directory.
# Also, we need to set two versions of ORIGIN here (one and two dollar signs).
# This is because on some systems it seems to be escaped, on some not...
# See https://cmake.org/pipermail/cmake/2008-January/019290.html
# and https://cmake.org/Wiki/CMake_RPATH_handling for details.
# Tipp: Use `ldd binary` and `readelf -d binary` to check RPATH settings.
# set( CMAKE_INSTALL_RPATH "$ORIGIN/..:$$ORIGIN/..:$ORIGIN:$$ORIGIN" )
# set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )

# Optionally set versioning for the library (if needed)
# set_target_properties(genesis_python_module PROPERTIES SOVERSION "1")

# add_custom_command (OUTPUT python_init COMMAND
#     touch ${PROJECT_SOURCE_DIR}/bin/python/__init__.py
# )
