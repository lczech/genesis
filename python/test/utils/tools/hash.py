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

class TestGenesisUtilsHash(unittest.TestCase):
    def test_md5(self):
        hash = utils.MD5.read_hex(
            utils.io.from_string( "abc" )
        )
        self.assertEqual( hash, "900150983cd24fb0d6963f7d28e17f72" )

    def test_hash_hex_md5(self):
        hash = utils.hash_hex(
            utils.io.from_string( "abc" ),
            utils.HashingFunctions.kMD5
        )
        self.assertEqual( hash, "900150983cd24fb0d6963f7d28e17f72" )

    def test_sha1(self):
        hash = utils.SHA1.read_hex(
            utils.io.from_string( "abc" )
        )
        self.assertEqual( hash, "a9993e364706816aba3e25717850c26c9cd0d89d" )

    def test_hash_hex_sha1(self):
        hash = utils.hash_hex(
            utils.io.from_string( "abc" ),
            utils.HashingFunctions.kSHA1
        )
        self.assertEqual( hash, "a9993e364706816aba3e25717850c26c9cd0d89d" )

    def test_sha256(self):
        hash = utils.SHA256.read_hex(
            utils.io.from_string( "abc" )
        )
        self.assertEqual( hash, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" )

    def test_hash_hex_sha256(self):
        hash = utils.hash_hex(
            utils.io.from_string( "abc" ),
            utils.HashingFunctions.kSHA256
        )
        self.assertEqual( hash, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" )

if __name__ == '__main__':
    unittest.main()
