#!/usr/bin/python

import os
import re
import xml.etree.ElementTree

from py_binder.boost_writer import *
from py_binder.cpp_entities import *
from py_binder.doxygen_reader import *

#~ cpp over doxygen to boost python binding generator
#~ codygen
#~ copygen
#~ codbpbg

# TODO resolve typdefs
# TODO return policies

# ==================================================================================================
#     Main
# ==================================================================================================

if __name__ == "__main__":
    hierarchy = DoxygenReader.parse ("./xml")
    hierarchy.extract_iterators(True)
    hierarchy.shorten_location_prefix()
    #~ print BoostPythonWriter.generate (hierarchy)
    BoostPythonWriter.generate_files (hierarchy, "./src", "genesis")
