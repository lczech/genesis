#!/bin/bash

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH
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
#    Init
####################################################################################################

# Change to top level of git repo and then to /lib
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/genesis/

####################################################################################################
#    Prepare Header and Footer Text
####################################################################################################

read -d '' HEADER_TEXT <<"EOF"
#ifndef REPLACE_GUARD
#define REPLACE_GUARD

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-REPLACE_YEAR Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief REPLACE_BRIEF
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */
EOF

read -d '' FOOTER_TEXT <<"EOF"
#endif // include guard
EOF

####################################################################################################
#    Update the generell (all-including) header file genesis.hpp
####################################################################################################

# Start from scratch
rm -f genesis.hpp

# Write header
GUARD="GENESIS_H_"
YEAR=`date +"%Y"`
BRIEF="This header includes all other genesis headers (*.hpp)."
echo "${HEADER_TEXT}"                |
    sed "s/REPLACE_GUARD/${GUARD}/g" |
    sed "s/REPLACE_YEAR/${YEAR}/g"   |
    sed "s/REPLACE_BRIEF/${BRIEF}/g" >> genesis.hpp
echo "" >> genesis.hpp

# Write content
for d in `ls -v -d */`
do
    # Simple verison: Just include the module headers
    d=${d%/}
    echo "#include \"genesis/${d}.hpp\"" >> genesis.hpp

    # Full version: Include each header by itself
    # for f in `find ${d} -name "*.hpp" | sort`
    # do
    #     echo "#include \"genesis/${f}\"" >> genesis.hpp
    # done
    # echo "" >> genesis.hpp
done

# Write footer
echo "" >> genesis.hpp
echo "${FOOTER_TEXT}" >> genesis.hpp

####################################################################################################
#    Update the header files for the modules
####################################################################################################

# Walk directories
for d in `ls -v -d */`
do
    # Remove slash
    d=${d%/}

    # Start from scratch
    rm -f ${d}.hpp

    # Write header
    GUARD="GENESIS_${d^^}_H_"
    YEAR=`date +"%Y"`
    BRIEF="This header includes all headers in '${d}'."
    echo "${HEADER_TEXT}"                |
        sed "s/REPLACE_GUARD/${GUARD}/g" |
        sed "s/REPLACE_YEAR/${YEAR}/g"   |
        sed "s/REPLACE_BRIEF/${BRIEF}/g" >> ${d}.hpp
    echo "" >> ${d}.hpp

    # Write content
    for f in `find ${d}/ -name "*.hpp" | sort`
    do
        echo "#include \"genesis/${f}\"" >> ${d}.hpp
    done
    echo "" >> ${d}.hpp

    # Write footer
    echo "${FOOTER_TEXT}" >> ${d}.hpp

done
