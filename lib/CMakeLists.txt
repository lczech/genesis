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
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/bin)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files. The CMake documentation does not recommend this,
# but for our purposes, this is good enough for now.
file (GLOB_RECURSE genesis_lib_sources   ${PROJECT_SOURCE_DIR}/lib/*.cpp)
# file (GLOB_RECURSE genesis_lib_headers   ${PROJECT_SOURCE_DIR}/lib/*.hpp)
# file (GLOB_RECURSE genesis_lib_templates ${PROJECT_SOURCE_DIR}/lib/*.tpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
#   Build Libraries
# ------------------------------------------------------------------------------

if (BUILD_SHARED_LIB)
    message (STATUS "${ColorBlue}Building shared lib${ColorEnd}")
    add_library           (genesis_lib_shared SHARED ${genesis_lib_sources})
    set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)

    if(THREADS_FOUND)
        target_link_libraries (genesis_lib_shared ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()

if (BUILD_STATIC_LIB)
    message (STATUS "${ColorBlue}Building static lib${ColorEnd}")
    add_library           (genesis_lib_static STATIC ${genesis_lib_sources})
    set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)

    if(THREADS_FOUND)
        target_link_libraries (genesis_lib_static ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()
