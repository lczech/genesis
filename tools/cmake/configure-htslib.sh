#!/bin/bash

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

####################################################################################################
#    This script configures htslib
####################################################################################################

# For some complicated reason, there is some weird interaction of escaping strings between our
# cmake setup, the GitHub Actions setup-cpp, and the htslib configuration, see
# https://github.com/samtools/htslib/issues/1527 and https://github.com/aminya/setup-cpp/issues/145
# Here, we try to configure htslib in a script instead of from within the CMake ExternalProject_Add
# command, in the hope that this works more reliably...

# Change to top level, to ensure that the script can be called from any directory.
cd "$(dirname "${BASH_SOURCE[0]}")/../.."
pwd
CMAKE_CURRENT_BINARY_DIR=`pwd`/build

# Now change to the htslib directory that we want to configure
cd build/genesis-htslib-source
pwd

echo "configuring htslib via script"
autoreconf -i
./configure CFLAGS=-fPIC CXXFLAGS=-fPIC --prefix=${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib --libdir=${CMAKE_CURRENT_BINARY_DIR}/genesis-htslib/lib --disable-multi-os-directory --disable-libcurl --without-libdeflate
