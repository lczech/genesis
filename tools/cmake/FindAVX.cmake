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

# FindAVX.cmake - Find AVX and AVX2 support
# This module defines the following variables:
#   AVX_FOUND          - Set to TRUE if AVX is supported
#   AVX2_FOUND         - Set to TRUE if AVX2 is supported
#   AVX_COMPILE_FLAGS  - The compiler flags for AVX support
#   AVX2_COMPILE_FLAGS - The compiler flags for AVX2 support

# Include necessary modules
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)

# ==================================================================================================
#   Check for AVX
# ==================================================================================================

# Initialize variables to indicate AVXX2 support
set(AVX_FOUND FALSE)
set(AVX_COMPILE_FLAGS "")

# List of AVX compiler flags to test
set(AVX_FLAGS_LIST " " "-mavx" "/arch:AVX" "-march=core-avx-i")

# Function to check if AVX is supported
function(check_avx_support)
  set(source_code "
      #include <immintrin.h>
      int main()
      {
        __m256 a, b, c;
        const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        float dst[8];
        a = _mm256_loadu_ps( src );
        b = _mm256_loadu_ps( src );
        c = _mm256_add_ps( a, b );
        _mm256_storeu_ps( dst, c );
        for( int i = 0; i < 8; i++ ){
          if( ( src[i] + src[i] ) != dst[i] ){
            return -1;
          }
        }
        return 0;
      }")

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/check_avx.cpp "${source_code}")

    foreach(flag ${AVX_FLAGS_LIST})
        TRY_COMPILE(
            COMPILE_RESULT
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/check_avx.cpp
            COMPILE_DEFINITIONS ${flag}
        )

        if(COMPILE_RESULT)
            TRY_RUN(
                RUN_RESULT
                COMPILE_RESULT
                ${CMAKE_CURRENT_BINARY_DIR}
                ${CMAKE_CURRENT_BINARY_DIR}/check_avx.cpp
                COMPILE_DEFINITIONS ${flag}
            )
            if("${COMPILE_RESULT}" AND (RUN_RESULT EQUAL 0))
                set(AVX_FOUND TRUE PARENT_SCOPE)
                set(AVX_COMPILE_FLAGS ${flag} PARENT_SCOPE)
                break()
            endif()
        endif()
    endforeach()
endfunction()

# Check for AVX support
check_avx_support()

# Provide feedback to the user and export variables
if(AVX_FOUND)
  message(STATUS "AVX support: enabled with flags ${AVX_COMPILE_FLAGS}")
  # set(AVX_COMPILE_FLAGS ${AVX_COMPILE_FLAGS} PARENT_SCOPE)
else()
  message(STATUS "AVX support: not found")
endif()

# ==================================================================================================
#   Check for AVX2
# ==================================================================================================

# Initialize variables to indicate AVX2 support
set(AVX2_FOUND FALSE)
set(AVX2_COMPILE_FLAGS "")

# List of AVX compiler flags to test
set(AVX2_FLAGS_LIST " " "-mavx2" "/arch:AVX2" "-march=core-avx2")

# Function to check if AVX2 is supported
function(check_avx2_support)
  set(source_code "
      #include <immintrin.h>
      int main()
      {
        __m256i a, b, c;
        const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        int dst[8];
        a =  _mm256_loadu_si256( (__m256i*)src );
        b =  _mm256_loadu_si256( (__m256i*)src );
        c = _mm256_add_epi32( a, b );
        _mm256_storeu_si256( (__m256i*)dst, c );
        for( int i = 0; i < 8; i++ ){
          if( ( src[i] + src[i] ) != dst[i] ){
            return -1;
          }
        }
        return 0;
      }")

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/check_avx2.cpp "${source_code}")

    foreach(flag ${AVX2_FLAGS_LIST})
        TRY_COMPILE(
            COMPILE_RESULT
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/check_avx2.cpp
            COMPILE_DEFINITIONS ${flag}
        )

        if(COMPILE_RESULT)
            TRY_RUN(
                RUN_RESULT
                COMPILE_RESULT
                ${CMAKE_CURRENT_BINARY_DIR}
                ${CMAKE_CURRENT_BINARY_DIR}/check_avx2.cpp
                COMPILE_DEFINITIONS ${flag}
            )
            if("${COMPILE_RESULT}" AND (RUN_RESULT EQUAL 0))
                set(AVX2_FOUND TRUE PARENT_SCOPE)
                set(AVX2_COMPILE_FLAGS ${flag} PARENT_SCOPE)
                break()
            endif()
        endif()
    endforeach()
endfunction()

# Check for AVX2 support
check_avx2_support()

# Provide feedback to the user and export variables
if(AVX2_FOUND)
  message(STATUS "AVX2 support: enabled with flags ${AVX2_COMPILE_FLAGS}")
  # set(AVX2_COMPILE_FLAGS ${AVX2_COMPILE_FLAGS} PARENT_SCOPE)
else()
  message(STATUS "AVX2 support: not found")
endif()
