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

from genesis.genesis import utils

class TestGenesisUtilsTextConvert(unittest.TestCase):
    def test_char_to_hex_valid(self):
        self.assertTrue( utils.text.convert_to_bool( "1" ))
        self.assertTrue( utils.text.convert_to_bool( "ON" ))
        self.assertTrue( utils.text.convert_to_bool( "yes" ))
        self.assertTrue( utils.text.convert_to_bool( "true" ))
        self.assertTrue( utils.text.convert_to_bool( "True" ))

        self.assertFalse( utils.text.convert_to_bool( "0" ))
        self.assertFalse( utils.text.convert_to_bool( "OFF" ))
        self.assertFalse( utils.text.convert_to_bool( "no" ))
        self.assertFalse( utils.text.convert_to_bool( "false" ))
        self.assertFalse( utils.text.convert_to_bool( "False" ))

        self.assertRaises(
            RuntimeError,
            utils.text.convert_to_bool,
            "foo"
        )

if __name__ == '__main__':
    unittest.main()
