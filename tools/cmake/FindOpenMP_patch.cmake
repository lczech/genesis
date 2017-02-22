#.rst:
# FindOpenMP
# ----------
#
# Finds OpenMP support
#
# This module can be used to detect OpenMP support in a compiler.  If
# the compiler supports OpenMP, the flags required to compile with
# OpenMP support are returned in variables for the different languages.
# The variables may be empty if the compiler does not need a special
# flag to support OpenMP.
#
# The following variables are set:
#
# ::
#
#    OpenMP_C_FLAGS - flags to add to the C compiler for OpenMP support
#    OpenMP_CXX_FLAGS - flags to add to the CXX compiler for OpenMP support
#    OpenMP_Fortran_FLAGS - flags to add to the Fortran compiler for OpenMP support
#    OPENMP_FOUND - true if openmp is detected
#
#
#
# Supported compilers can be found at
# http://openmp.org/wp/openmp-compilers/

#=============================================================================
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2017 Kitware, Inc. and Contributors
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Kitware, Inc. nor the names of Contributors
#   may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ------------------------------------------------------------------------------
#
# The following individuals and institutions are among the Contributors:
#
# * Aaron C. Meadows <cmake@shadowguarddev.com>
# * Aleksey Avdeev <solo@altlinux.ru>
# * Alexander Neundorf <neundorf@kde.org>
# * Alexander Smorkalov <alexander.smorkalov@itseez.com>
# * Alexey Sokolov <sokolov@google.com>
# * Alex Turbov <i.zaufi@gmail.com>
# * Andreas Pakulat <apaku@gmx.de>
# * Andreas Schneider <asn@cryptomilk.org>
# * André Rigland Brodtkorb <Andre.Brodtkorb@ifi.uio.no>
# * Axel Huebl, Helmholtz-Zentrum Dresden - Rossendorf
# * Benjamin Eikel
# * Bjoern Ricks <bjoern.ricks@gmail.com>
# * Brad Hards <bradh@kde.org>
# * Christopher Harvey
# * Christoph Grüninger <foss@grueninger.de>
# * Clement Creusot <creusot@cs.york.ac.uk>
# * Daniel Blezek <blezek@gmail.com>
# * Daniel Pfeifer <daniel@pfeifer-mail.de>
# * Enrico Scholz <enrico.scholz@informatik.tu-chemnitz.de>
# * Eran Ifrah <eran.ifrah@gmail.com>
# * Esben Mose Hansen, Ange Optimization ApS
# * Geoffrey Viola <geoffrey.viola@asirobots.com>
# * Google Inc
# * Gregor Jasny
# * Helio Chissini de Castro <helio@kde.org>
# * Ilya Lavrenov <ilya.lavrenov@itseez.com>
# * Insight Software Consortium <insightsoftwareconsortium.org>
# * Jan Woetzel
# * Kelly Thompson <kgt@lanl.gov>
# * Konstantin Podsvirov <konstantin@podsvirov.pro>
# * Mario Bensi <mbensi@ipsquad.net>
# * Mathieu Malaterre <mathieu.malaterre@gmail.com>
# * Matthaeus G. Chajdas
# * Matthias Kretz <kretz@kde.org>
# * Matthias Maennich <matthias@maennich.net>
# * Miguel A. Figueroa-Villanueva
# * Mike Jackson
# * Mike McQuaid <mike@mikemcquaid.com>
# * Nicolas Bock <nicolasbock@gmail.com>
# * Nicolas Despres <nicolas.despres@gmail.com>
# * Nikita Krupen'ko <krnekit@gmail.com>
# * OpenGamma Ltd. <opengamma.com>
# * Per Øyvind Karlsen <peroyvind@mandriva.org>
# * Peter Collingbourne <peter@pcc.me.uk>
# * Petr Gotthard <gotthard@honeywell.com>
# * Philip Lowman <philip@yhbt.com>
# * Philippe Proulx <pproulx@efficios.com>
# * Raffi Enficiaud, Max Planck Society
# * Raumfeld <raumfeld.com>
# * Roger Leigh <rleigh@codelibre.net>
# * Rolf Eike Beer <eike@sf-mail.de>
# * Roman Donchenko <roman.donchenko@itseez.com>
# * Roman Kharitonov <roman.kharitonov@itseez.com>
# * Ruslan Baratov
# * Sebastian Holtermann <sebholt@xwmw.org>
# * Stephen Kelly <steveire@gmail.com>
# * Sylvain Joubert <joubert.sy@gmail.com>
# * Thomas Sondergaard <ts@medical-insight.com>
# * Tobias Hunger <tobias.hunger@qt.io>
# * Todd Gamblin <tgamblin@llnl.gov>
# * Tristan Carel
# * University of Dundee
# * Vadim Zhukov
# * Will Dicharry <wdicharry@stellarscience.com>
#
# See version control history for details of individual contributions.
#
# The above copyright and license notice applies to distributions of
# CMake in source and binary form.  Third-party software packages supplied
# with CMake under compatible licenses provide their own copyright notices
# documented in corresponding subdirectories or source files.
#
# ------------------------------------------------------------------------------
#
# CMake was initially developed by Kitware with the following sponsorship:
#
#  * National Library of Medicine at the National Institutes of Health
#    as part of the Insight Segmentation and Registration Toolkit (ITK).
#
#  * US National Labs (Los Alamos, Livermore, Sandia) ASC Parallel
#    Visualization Initiative.
#
#  * National Alliance for Medical Image Computing (NAMIC) is funded by the
#    National Institutes of Health through the NIH Roadmap for Medical Research,
#    Grant U54 EB005149.
#
# * Kitware, Inc.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)
#=============================================================================

# This file is copied from
# https://cmake.org/gitweb?p=cmake.git;a=blob;f=Modules/FindOpenMP.cmake;h=ee4bdd6c873b0dd8d88c79a5589b56f997010d09;hb=1549927d
# and modified in the placed indicated with my name: Lucas Czech

# Make it possible to track issues when users compile with this patch:
message (STATUS "Using find_package( OpenMP ) patch")

#=============================================================================

set(_OPENMP_REQUIRED_VARS)
set(CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
set(CMAKE_REQUIRED_QUIET ${OpenMP_FIND_QUIETLY})

function(_OPENMP_FLAG_CANDIDATES LANG)
  set(OpenMP_FLAG_CANDIDATES
    #Empty, if compiler automatically accepts openmp
    " "
    #GNU
    "-fopenmp"

    # 2017-02-22 Lucas Czech: Added `-fopenmp=libiomp5` flag for Clang.
    # It is not the first to be search though, but it works.

    #Clang
    "-fopenmp=libiomp5"
    "-fopenmp=libomp"

    #Microsoft Visual Studio
    "/openmp"
    #Intel windows
    "-Qopenmp"
    #PathScale, Intel
    "-openmp"
    #Sun
    "-xopenmp"
    #HP
    "+Oopenmp"
    #IBM XL C/c++
    "-qsmp"
    #Portland Group, MIPSpro
    "-mp"
  )

  set(OMP_FLAG_GNU "-fopenmp")
  set(OMP_FLAG_Clang "-fopenmp=libomp")
  set(OMP_FLAG_HP "+Oopenmp")
  if(WIN32)
    set(OMP_FLAG_Intel "-Qopenmp")
  elseif(CMAKE_${LANG}_COMPILER_ID STREQUAL "Intel" AND
         "${CMAKE_${LANG}_COMPILER_VERSION}" VERSION_LESS "15.0.0.20140528")
    set(OMP_FLAG_Intel "-openmp")
  else()
    set(OMP_FLAG_Intel "-qopenmp")
  endif()
  set(OMP_FLAG_MIPSpro "-mp")
  set(OMP_FLAG_MSVC "/openmp")
  set(OMP_FLAG_PathScale "-openmp")
  set(OMP_FLAG_PGI "-mp")
  set(OMP_FLAG_SunPro "-xopenmp")
  set(OMP_FLAG_XL "-qsmp")
  set(OMP_FLAG_Cray " ")

  # Move the flag that matches the compiler to the head of the list,
  # this is faster and doesn't clutter the output that much. If that
  # flag doesn't work we will still try all.
  if(OMP_FLAG_${CMAKE_${LANG}_COMPILER_ID})
    list(REMOVE_ITEM OpenMP_FLAG_CANDIDATES "${OMP_FLAG_${CMAKE_${LANG}_COMPILER_ID}}")
    list(INSERT OpenMP_FLAG_CANDIDATES 0 "${OMP_FLAG_${CMAKE_${LANG}_COMPILER_ID}}")
  endif()

  set(OpenMP_${LANG}_FLAG_CANDIDATES "${OpenMP_FLAG_CANDIDATES}" PARENT_SCOPE)
endfunction()

# sample openmp source code to test
set(OpenMP_C_TEST_SOURCE
"
#include <omp.h>
int main() {
#ifdef _OPENMP
  return 0;
#else
  breaks_on_purpose
#endif
}
")

# same in Fortran
set(OpenMP_Fortran_TEST_SOURCE
  "
      program test
      use omp_lib
      integer :: n
      n = omp_get_num_threads()
      end program test
  "
  )

# check c compiler
if(CMAKE_C_COMPILER_LOADED)
  # if these are set then do not try to find them again,
  # by avoiding any try_compiles for the flags
  if(OpenMP_C_FLAGS)
    unset(OpenMP_C_FLAG_CANDIDATES)
  else()
    _OPENMP_FLAG_CANDIDATES("C")

    # 2017-02-22 Lucas Czech: Cannot use an absolute path in this patch.
    # include(${CMAKE_CURRENT_LIST_DIR}/CheckCSourceCompiles.cmake)
    include(CheckCSourceCompiles)

  endif()

  foreach(FLAG IN LISTS OpenMP_C_FLAG_CANDIDATES)
    set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${FLAG}")
    unset(OpenMP_FLAG_DETECTED CACHE)
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Try OpenMP C flag = [${FLAG}]")
    endif()
    check_c_source_compiles("${OpenMP_C_TEST_SOURCE}" OpenMP_FLAG_DETECTED)
    set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
    if(OpenMP_FLAG_DETECTED)
      set(OpenMP_C_FLAGS_INTERNAL "${FLAG}")
      break()
    endif()
  endforeach()

  set(OpenMP_C_FLAGS "${OpenMP_C_FLAGS_INTERNAL}"
    CACHE STRING "C compiler flags for OpenMP parallization")

  list(APPEND _OPENMP_REQUIRED_VARS OpenMP_C_FLAGS)
  unset(OpenMP_C_FLAG_CANDIDATES)
endif()

# check cxx compiler
if(CMAKE_CXX_COMPILER_LOADED)
  # if these are set then do not try to find them again,
  # by avoiding any try_compiles for the flags
  if(OpenMP_CXX_FLAGS)
    unset(OpenMP_CXX_FLAG_CANDIDATES)
  else()
    _OPENMP_FLAG_CANDIDATES("CXX")

    # 2017-02-22 Lucas Czech: Cannot use an absolute path in this patch.
    # include(${CMAKE_CURRENT_LIST_DIR}/CheckCXXSourceCompiles.cmake)
    include(CheckCXXSourceCompiles)

    # use the same source for CXX as C for now
    set(OpenMP_CXX_TEST_SOURCE ${OpenMP_C_TEST_SOURCE})
  endif()

  foreach(FLAG IN LISTS OpenMP_CXX_FLAG_CANDIDATES)
    set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${FLAG}")
    unset(OpenMP_FLAG_DETECTED CACHE)
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Try OpenMP CXX flag = [${FLAG}]")
    endif()
    check_cxx_source_compiles("${OpenMP_CXX_TEST_SOURCE}" OpenMP_FLAG_DETECTED)
    set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
    if(OpenMP_FLAG_DETECTED)
      set(OpenMP_CXX_FLAGS_INTERNAL "${FLAG}")
      break()
    endif()
  endforeach()

  set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS_INTERNAL}"
    CACHE STRING "C++ compiler flags for OpenMP parallization")

  list(APPEND _OPENMP_REQUIRED_VARS OpenMP_CXX_FLAGS)
  unset(OpenMP_CXX_FLAG_CANDIDATES)
  unset(OpenMP_CXX_TEST_SOURCE)
endif()

# check Fortran compiler
if(CMAKE_Fortran_COMPILER_LOADED)
  # if these are set then do not try to find them again,
  # by avoiding any try_compiles for the flags
  if(OpenMP_Fortran_FLAGS)
    unset(OpenMP_Fortran_FLAG_CANDIDATES)
  else()
    _OPENMP_FLAG_CANDIDATES("Fortran")

    # 2017-02-22 Lucas Czech: Cannot use an absolute path in this patch.
    # include(${CMAKE_CURRENT_LIST_DIR}/CheckFortranSourceCompiles.cmake)
    include(CheckFortranSourceCompiles)

  endif()

  foreach(FLAG IN LISTS OpenMP_Fortran_FLAG_CANDIDATES)
    set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${FLAG}")
    unset(OpenMP_FLAG_DETECTED CACHE)
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Try OpenMP Fortran flag = [${FLAG}]")
    endif()
    check_fortran_source_compiles("${OpenMP_Fortran_TEST_SOURCE}" OpenMP_FLAG_DETECTED)
    set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
    if(OpenMP_FLAG_DETECTED)
      set(OpenMP_Fortran_FLAGS_INTERNAL "${FLAG}")
      break()
    endif()
  endforeach()

  set(OpenMP_Fortran_FLAGS "${OpenMP_Fortran_FLAGS_INTERNAL}"
    CACHE STRING "Fortran compiler flags for OpenMP parallization")

  list(APPEND _OPENMP_REQUIRED_VARS OpenMP_Fortran_FLAGS)
  unset(OpenMP_Fortran_FLAG_CANDIDATES)
  unset(OpenMP_Fortran_TEST_SOURCE)
endif()

set(CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})

if(_OPENMP_REQUIRED_VARS)

  # 2017-02-22 Lucas Czech: Cannot use an absolute path in this patch.
  # include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
  include(FindPackageHandleStandardArgs)

  find_package_handle_standard_args(OpenMP
                                    REQUIRED_VARS ${_OPENMP_REQUIRED_VARS})

  mark_as_advanced(${_OPENMP_REQUIRED_VARS})

  unset(_OPENMP_REQUIRED_VARS)
else()
  message(SEND_ERROR "FindOpenMP requires C or CXX language to be enabled")
endif()
