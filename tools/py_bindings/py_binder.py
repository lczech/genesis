#!/usr/bin/python

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

import os
import re
import sys

import xml.etree.ElementTree

from py_binder.boost_writer import *
from py_binder.cpp_entities import *
from py_binder.doxygen_reader import *

# Naming ideas for this tool:
# cpp over doxygen to boost python binding generator
# codygen
# copygen
# codbpbg

# TODO resolve typdefs
# TODO return policies

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":

    # Parse command line arguments
    xml_dir = "./xml"
    src_dir = "./src"
    if len(sys.argv) >= 2:
        xml_dir = sys.argv[1].strip("/")
    if len(sys.argv) == 3:
        src_dir = sys.argv[2].strip("/")
    if len(sys.argv) >  3:
        print "Usage:", sys.argv[0], "[input_xml_dir=\"./xml\"] [output_src_dir=\"./src\"]"
        sys.exit()

    # Print some user information
    print "Using input  xml dir:", xml_dir
    print "Using output src dir:", src_dir
    print

    # Read doxygen files into global namespace object
    ns_global = DoxygenReader.parse_xml_dir (xml_dir)

    # Modify according to needs
    ns_global.extract_iterators(True)
    ns_global.shorten_location_prefix()

    # Debug output
    # ns_global.dump(detail=3)

    # Generate boost files
    # print BoostPythonWriter.generate (ns_global)
    BoostPythonWriter.generate_files (ns_global, src_dir, "genesis")
    print "Finished."
