#!/usr/bin/python

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

    # Some user information
    print "Using input  xml dir:", xml_dir
    print "Using output src dir:", src_dir
    print

    # Read doxygen files
    hierarchy = DoxygenReader.parse (xml_dir)

    # Modify according to needs
    hierarchy.extract_iterators(True)
    hierarchy.shorten_location_prefix()

    # Debug output
    # hierarchy.dump()

    # Generate boost files
    # print BoostPythonWriter.generate (hierarchy)
    BoostPythonWriter.generate_files (hierarchy, src_dir, "genesis")
