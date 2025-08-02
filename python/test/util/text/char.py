# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech
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
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

import unittest
import tempfile
import os
import pathlib
import io

from genesis.genesis import util

class TestGenesisutilTextChar(unittest.TestCase):
    def test_char_to_hex_valid(self):
        for i in range( 0, 128):
            # print(chr(i), "-->", util.text.char_to_hex(chr(i)))
            self.assertTrue( util.text.char_to_hex(chr(i)) )
            self.assertGreater( len(util.text.char_to_hex(chr(i))), 0 )

    def test_char_to_hex_invalid(self):
        self.assertRaises(
            ValueError,
            util.text.char_to_hex,
            "string"
        )
