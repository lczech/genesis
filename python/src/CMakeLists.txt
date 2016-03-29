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

if (USE_SHARED_BOOST)
    set (Boost_USE_STATIC_LIBS OFF)
else()
    set (Boost_USE_STATIC_LIBS ON)
endif()

find_package (PythonLibs 2.7)
find_package (Boost      1.41.0 COMPONENTS python)

if (NOT PYTHONLIBS_FOUND OR NOT Boost_PYTHON_FOUND)
    message (STATUS "Python Lib or Boost Python not found")
    message (WARNING "${ColorRed}Cannot build Python module.${ColorEnd}")
    return()
endif()

message (STATUS "Found Python Lib and Boost Python")
message (STATUS "${ColorBlue}Building Python module${ColorEnd}")
message (STATUS "Using boost shared libs: " ${USE_SHARED_BOOST} )

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/python/module)

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

# The boost headers contain some warnings about unused variables, which does not
# look nice in the build process. For some reason, everything works find without
# this line, so we just leave it out - for now at least.
# include_directories   (${Boost_INCLUDE_DIRS})
link_directories      (${Boost_LIBRARY_DIRS})

include_directories   (${PYTHON_INCLUDE_DIRS})
link_directories      (${PYTHON_LIBRARIES})

add_library           (genesis_python_module MODULE ${genesis_python_sources})

target_link_libraries (genesis_python_module genesis_lib_shared)
target_link_libraries (genesis_python_module ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME genesis)
set_target_properties (genesis_python_module PROPERTIES PREFIX "")
# set_target_properties (genesis_python_module PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/module
# )

# add_custom_command (OUTPUT python_init COMMAND
#     touch ${PROJECT_SOURCE_DIR}/module/__init__.py
# )

# If the user specified to use a shared object for boost instead of statically
# linking against it, we want to add a proper rpath to the library, so that the
# boost lib will be found if put in the same directory as the genesis lib.
if (USE_SHARED_BOOST)
    set_target_properties (genesis_python_module PROPERTIES INSTALL_RPATH "$$ORIGIN")
endif()
