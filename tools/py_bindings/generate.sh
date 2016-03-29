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
#    This script first runs doxygen to create an xml representation of the code and then
#    uses the python script to generate bindings code from this.
####################################################################################################

# Change to top level of git repo and then to the py bindings dir.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd tools/py_bindings/

function print_separator() {
    echo
    echo -en "\e[32m==============================================="
    echo     "====================================================="
    echo -e  "          ${1}"
    echo -en "====================================================="
    echo -e  "===============================================\e[0m"
    echo
}

# Run doxygen
print_separator "Running doxygen"
rm -rf xml/
doxygen > /dev/null

# Run the script
print_separator "Running bindings script"
rm -rf src/
./py_binder.py
