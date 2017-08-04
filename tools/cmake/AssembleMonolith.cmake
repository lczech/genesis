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
#   Assemble Monolith
# ------------------------------------------------------------------------------

# Helper function that builds monolithic source files by concatenating the original sources.
#
# It takes four parameters:
#
# input_directory: Where to look for source files (currently hardcoded to *.cpp)
# use_modules: Either "FULL" or "MODULES". The function offers two modes for monolithic building.
#     In "FULL" mode, all sources of the given input_directory are combined into one.
#     In "MODULES" mode, each immediate subdirectory of input_directory is treated separately,
#     resulting in one monolithic compilation unit per subdirectory. Files that are directly in
#     input_directory (not in its subdirectories) are simply used as normal single compilation units.
# name: Is used to distinguish between multiple monolithic parts during build (one for the lib,
#     one for test etc). The name needs to be uniq for each call of this function.
# resulting_source_files: Is used to store the resulting sources files. The value of this variable
#     can then ben used as source argument for add_library() and the like.

function( ASSEMBLE_MONOLITH input_directory use_modules name resulting_source_files  )

    # Helper: Return a list of all directories within a given directory, non-recursive.
    function( SUBDIRLIST result indir )
        file( GLOB children RELATIVE ${indir} ${indir}/* )
        set( dirlist "" )
        foreach(child ${children})
            if( IS_DIRECTORY ${indir}/${child} )
                LIST( APPEND dirlist ${child} )
            endif()
        endforeach()
        set( ${result} ${dirlist} PARENT_SCOPE )
    endfunction()

    # Helper: Append the contents of a file to another file.
    function( APPEND_TO_FILE IN_FILE OUT_FILE )
        file( READ ${IN_FILE} CONTENTS )
        file( APPEND ${OUT_FILE} "${CONTENTS}" )
    endfunction()

    # Clean previously assembled monolith files.
    file( REMOVE_RECURSE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name} )

    IF( "${use_modules}" STREQUAL "MODULES" )

        # Process all modules (i.e., subdirs of the input dir).
        SUBDIRLIST( SUBDIRS ${input_directory} )
        foreach( SUBDIR ${SUBDIRS} )

            # Target files for this module.
            SET( ASSEMBLY_FILE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name}/${SUBDIR}.cpp )
            SET( MONOLITH_FILE ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/${SUBDIR}.cpp )

            # Find all sources of the module and concat them into a tmp assembly file.
            file (GLOB_RECURSE subdir_sources ${input_directory}/${SUBDIR}/*.cpp)
            file( WRITE ${ASSEMBLY_FILE} "" )
            foreach( subdir_source ${subdir_sources} )
                APPEND_TO_FILE( ${subdir_source} ${ASSEMBLY_FILE} )
            endforeach()

            # Copy the assemlby file to its destination. Using configure_file here ensures that we
            # only copy if the file changes, so that incremental builds are faster.
            configure_file( ${ASSEMBLY_FILE} ${MONOLITH_FILE} COPYONLY )
        endforeach()

        # Find all resulting source files. We need to treat top-level files in the lib dir
        # extra, as they are not a part of any module.
        file (GLOB top_level_sources ${input_directory}/*.cpp)
        file (GLOB genesis_unity_sources     ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/*.cpp)
        set ( ${resulting_source_files} ${top_level_sources} ${genesis_unity_sources} PARENT_SCOPE )

    ELSEIF( "${use_modules}" STREQUAL "FULL" )

        # Target files.
        SET( ASSEMBLY_FILE ${CMAKE_BINARY_DIR}/genesis_unity_assemble/${name}/all.cpp )
        SET( MONOLITH_FILE ${CMAKE_BINARY_DIR}/genesis_unity_sources/${name}/all.cpp )

        # Find all sources and concat them into a tmp assembly file.
        file (GLOB_RECURSE all_sources ${input_directory}/*.cpp)
        file( WRITE ${ASSEMBLY_FILE} "" )
        foreach( subdir_source ${all_sources} )
            APPEND_TO_FILE( ${subdir_source} ${ASSEMBLY_FILE} )
        endforeach()

        # Copy the assemlby file to its destination. Using configure_file here ensures that we
        # only copy if the file changes, so that incremental builds are faster.
        configure_file( ${ASSEMBLY_FILE} ${MONOLITH_FILE} COPYONLY )

        # Return the assembled file.
        set ( ${resulting_source_files} ${MONOLITH_FILE} PARENT_SCOPE )

    ELSE()
        # Can only happen if some CMake file misuses this function.
        message( FATAL_ERROR
            "${ColorRed}ASSEMBLE_MONOLITH called with wrong arg: ${use_modules}${ColorEnd}"
        )
    ENDIF()
endfunction()
