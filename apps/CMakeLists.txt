# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2022 Lucas Czech
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

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/apps )

# Set rpath for all targets, so that they work out of the box (libgenesis.so in parent directory),
# as well as when relocating with the lib being in the same directory.
# Also, we need to set two versions of ORIGIN here (one and two dollar signs).
# This is because on some systems it seems to be escaped, on some not...
# See https://cmake.org/pipermail/cmake/2008-January/019290.html
# and https://cmake.org/Wiki/CMake_RPATH_handling for details.
# Tipp: Use `ldd binary` and `readelf -d binary` to check RPATH settings.
set( CMAKE_INSTALL_RPATH "$ORIGIN/..:$$ORIGIN/..:$ORIGIN:$$ORIGIN" )
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files in this directory and compile them into individual binaries, each
# linked to the shared lib. All files should thus contain their own main function.
file( GLOB genesis_apps_sources ${PROJECT_SOURCE_DIR}/apps/*.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/lib )

# ------------------------------------------------------------------------------
#   Build Applications
# ------------------------------------------------------------------------------

if( genesis_apps_sources )

    message( STATUS "${ColorBlue}Building applications:${ColorEnd}" )
    foreach( app_src ${genesis_apps_sources} )
        get_filename_component( app_name ${app_src} NAME_WE )
        message( STATUS "  ${ColorBlue}${app_name}${ColorEnd}" )

        add_executable( ${app_name} ${app_src} )

        target_link_libraries( ${app_name} PUBLIC genesis_lib_shared )

        # explicitly add OpenMP, compatible with Macos/brew installed OpenMP after Cmake 3.12
        if(${CMAKE_VERSION} VERSION_GREATER "3.9.0")
          if(OPENMP_FOUND)
            target_link_libraries (${app_name} PUBLIC OpenMP::OpenMP_CXX)
          endif()
        endif()

        # Link against any external libraries, e.g. Pthreads.
        target_link_libraries (${app_name} PRIVATE ${GENESIS_INTERNAL_LINK_LIBRARIES})
        IF(GENESIS_USE_HTSLIB)
            add_dependencies( ${app_name} htslib )
        ENDIF()

    endforeach()

else()

    message( STATUS "No genesis applications found" )

endif()

# find all subdirs with CMakelists in them and add them
file( GLOB_RECURSE genesis_apps_subdirs */CMakeLists.txt )
foreach( sub_dir_cmake ${genesis_apps_subdirs} )
    get_filename_component( sub_dir ${sub_dir_cmake} DIRECTORY )
    get_filename_component( sub_dir_name ${sub_dir} NAME )
    message( STATUS "${ColorBlue}Building sub-apps: ${sub_dir_name}${ColorEnd}" )
    add_subdirectory(${sub_dir})
endforeach()
