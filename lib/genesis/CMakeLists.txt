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
# Lucas Czech <lczech@carnegiescience.edu>
# Department of Plant Biology, Carnegie Institution For Science
# 260 Panama Street, Stanford, CA 94305, USA

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories (${PROJECT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

IF( GENESIS_UNITY_BUILD )

    # See main CMake file for the monolith macro.
    ASSEMBLE_MONOLITH( ${PROJECT_SOURCE_DIR}/lib/genesis "${GENESIS_UNITY_BUILD}" "lib" genesis_lib_sources )

ELSE()

    # Find all source files. The CMake documentation does not recommend this,
    # but for our purposes, this is good enough for now. Futhremore, we use a make file
    # that offers an update target to get around this limitation.
    file (GLOB_RECURSE genesis_lib_sources   ${PROJECT_SOURCE_DIR}/lib/genesis/*.cpp)
    # file (GLOB_RECURSE genesis_lib_headers   ${PROJECT_SOURCE_DIR}/lib/genesis/*.hpp)
    # file (GLOB_RECURSE genesis_lib_templates ${PROJECT_SOURCE_DIR}/lib/genesis/*.tpp)

ENDIF()

# ------------------------------------------------------------------------------
#   Build Object Library
# ------------------------------------------------------------------------------

# We use an object library first, which is then used by both the shared and static lib,
# in order to only compile the sources once, but have both lib flavors available.
add_library( genesis_lib_obj OBJECT ${genesis_lib_sources} )

# Link against any external libraries, e.g. htslib.
target_link_libraries( genesis_lib_obj PRIVATE ${GENESIS_INTERNAL_LINK_LIBRARIES} )
target_compile_options( genesis_lib_obj PRIVATE -fPIC )
target_include_directories( genesis_lib_obj PUBLIC ${PROJECT_SOURCE_DIR}/lib )

# Explicitly add OpenMP, compatible with Macos/brew installed OpenMP after Cmake 3.12
if(${CMAKE_VERSION} VERSION_GREATER "3.9.0")
    if(OPENMP_FOUND)
        target_link_libraries (genesis_lib_obj PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

# Add htslib as a dependency, so that CMake realizes that it has to be built.
IF(GENESIS_USE_HTSLIB)
    add_dependencies( genesis_lib_obj htslib )
ENDIF()

# Same for samtools. Not used at the moment though.
# IF(GENESIS_USE_SAMTOOLS)
#     add_dependencies( genesis_lib_obj samtools )
# ENDIF()

# Activate LTO if available
if(GENESIS_HAS_LTO)
    set_property(TARGET genesis_lib_obj PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# ------------------------------------------------------------------------------
#   Build Shared Library
# ------------------------------------------------------------------------------

# Build the shared library, unless explicitly requested not to.
# add_library           (genesis_lib_shared SHARED $<TARGET_OBJECTS:genesis_lib_obj>)
add_library           (genesis_lib_shared SHARED)
set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)
if (GENESIS_BUILD_SHARED_LIB)
    message (STATUS "${ColorBlue}Building shared lib${ColorEnd}")
else()
    set_target_properties (genesis_lib_shared PROPERTIES EXCLUDE_FROM_ALL ON)
endif()

# The shared lib uses the object library.
target_link_libraries (genesis_lib_shared PUBLIC genesis_lib_obj)

# Activate LTO if available. This needs to match the object library.
if(GENESIS_HAS_LTO)
    set_property(TARGET genesis_lib_shared PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Add this shared library to our exported libraries, which can then be used by the parent scope.
set( GENESIS_SHARED_LIBRARY genesis_lib_shared PARENT_SCOPE )
set( GENESIS_LIBRARY genesis_lib_shared PARENT_SCOPE )

# ------------------------------------------------------------------------------
#   Build Static Library
# ------------------------------------------------------------------------------

# Build the static library, unless explicitly requested not to.
# add_library           (genesis_lib_static STATIC $<TARGET_OBJECTS:genesis_lib_obj>)
add_library           (genesis_lib_static STATIC)
set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)
if (GENESIS_BUILD_STATIC_LIB)
    message (STATUS "${ColorBlue}Building static lib${ColorEnd}")
else()
    set_target_properties (genesis_lib_static PROPERTIES EXCLUDE_FROM_ALL ON)
endif()

# The static lib uses the object library.
target_link_libraries (genesis_lib_static PUBLIC genesis_lib_obj)

# Activate LTO if available. This needs to match the object library.
if(GENESIS_HAS_LTO)
    set_property(TARGET genesis_lib_static PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Add this static library to our exported libraries, which can then be used by the parent scope.
# In case that we also build the shared lib, GENESIS_LIBRARY is overwritten, which is on
# purpose. This way, if both libraries are available, we prefer to use the static one.
# This is more useful for external projects that use Genesis as a dependency: They probably
# do not want to run into dynamic linker issues when the shared library is not found after
# moving around their binary. So better link statically.
# Also, as the static library has to be actively activated (by setting GENESIS_BUILD_STATIC_LIB),
# there already was a user action once we are here, so the user actually wants the static one.
set( GENESIS_STATIC_LIBRARY genesis_lib_static PARENT_SCOPE )
if (GENESIS_BUILD_STATIC_LIB)
    set( GENESIS_LIBRARY genesis_lib_static PARENT_SCOPE )
endif()
