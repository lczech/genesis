#!/bin/bash

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

####################################################################################################
#    This script updates the generell (all-including) header file lib/genesis.hpp
####################################################################################################

# Change to top level of git repo and then to /lib
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/genesis/
rm -f genesis.hpp

echo "#ifndef GENESIS_H_" >> genesis.hpp
echo "#define GENESIS_H_" >> genesis.hpp
echo "" >> genesis.hpp
echo "/*" >> genesis.hpp
echo "    Genesis - A toolkit for working with phylogenetic data." >> genesis.hpp
echo "    Copyright (C) 2014-2017 Lucas Czech" >> genesis.hpp
echo "" >> genesis.hpp
echo "    This program is free software: you can redistribute it and/or modify" >> genesis.hpp
echo "    it under the terms of the GNU General Public License as published by" >> genesis.hpp
echo "    the Free Software Foundation, either version 3 of the License, or" >> genesis.hpp
echo "    (at your option) any later version." >> genesis.hpp
echo "" >> genesis.hpp
echo "    This program is distributed in the hope that it will be useful," >> genesis.hpp
echo "    but WITHOUT ANY WARRANTY; without even the implied warranty of" >> genesis.hpp
echo "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" >> genesis.hpp
echo "    GNU General Public License for more details." >> genesis.hpp
echo "" >> genesis.hpp
echo "    You should have received a copy of the GNU General Public License" >> genesis.hpp
echo "    along with this program.  If not, see <http://www.gnu.org/licenses/>." >> genesis.hpp
echo "" >> genesis.hpp
echo "    Contact:" >> genesis.hpp
echo "    Lucas Czech <lucas.czech@h-its.org>" >> genesis.hpp
echo "    Exelixis Lab, Heidelberg Institute for Theoretical Studies" >> genesis.hpp
echo "    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany" >> genesis.hpp
echo "*/" >> genesis.hpp
echo "" >> genesis.hpp
echo "/**" >> genesis.hpp
echo " * @brief This header includes all other genesis headers (*.hpp)." >> genesis.hpp
echo " * This makes it easy to integrate the code as a library, as all" >> genesis.hpp
echo " * symbols of genesis are available after including this header." >> genesis.hpp
echo " *" >> genesis.hpp
echo " * You do not need to edit this file manually. Simply run the script" >> genesis.hpp
echo " * make_genesis_header.sh in ./tools to update this file." >> genesis.hpp
echo " */" >> genesis.hpp
echo "" >> genesis.hpp

for d in `ls -v -d */`
do
    for f in `find ${d} -name "*.hpp" | sort`
    do
        echo "#include \"genesis/${f}\"" >> genesis.hpp
    done
    echo "" >> genesis.hpp
done

echo "#endif // include guard" >> genesis.hpp
