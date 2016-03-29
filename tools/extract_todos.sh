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
#    This script scans all C++ files in lib/, python/src/ and test/src/ for comments
#    that start with TODO or FIXME and writes them to a file ./TODO for easy access.
####################################################################################################

# Change to top level of git repo.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`

rm -f TODO

for file in `find lib/ python/src test/src -type f`; do
    fgrep -Hn -e "TODO"  ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tTODO\t/gI" >> TODO_tmp
    fgrep -Hn -e "FIXME" ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tFIXME\t/gI" >> TODO_tmp
done

cat TODO_tmp | column -t -s $'\t' > TODO
rm TODO_tmp
