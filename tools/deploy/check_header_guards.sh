#!/bin/bash

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

####################################################################################################
#    This script checks whether the include guards of the headers in ./lib are what they
#    should be according to their path and file name.
####################################################################################################

# Change to top level of git repo and then to /lib
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/

for header in `find . -name "*.hpp"`; do
    # Extract wanted guard name from file name.
    guard="${header/.\//genesis_}"
    guard="${guard/.hpp/_h_}"
    guard="${guard//\//_}"
    guard="${guard^^}"

    # Extract actual names from file.
    line_ifn=`head -n 1 ${header} | cut -d " " -f 2`
    line_def=`head -n 2 ${header} | tail -n 1 | cut -d " " -f 2`

    # Check first line (ifndef).
    # We skip the (all-including) genesis header, because it is special.
    if [[ ${line_ifn} != ${guard} && ${header} != "./genesis.hpp" ]]; then
        echo "In ${header}:"
        echo "   Actual: ${line_ifn}"
        echo "   Wanted: ${guard}"
    fi

    # Check second line (define)
    if [[ ${line_ifn} != ${line_def} ]]; then
        echo "In ${header}:"
        echo "   Condition:  ${line_ifn}"
        echo "   Definition: ${line_def}"
    fi
done
