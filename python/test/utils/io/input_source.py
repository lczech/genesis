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
# from genesis import genesis

class TestGenesisUtilsCoreIoInputSource(unittest.TestCase):
    def setUp(self):
        # Create a temp file with known content
        self.content = "Hello, world!\nThis is a test.\n"
        tmp = tempfile.NamedTemporaryFile(delete=False, mode='w', encoding='utf-8')
        tmp.write(self.content)
        tmp.flush()
        tmp.close()
        self.path = tmp.name

    def tearDown(self):
        os.remove(self.path)

    def test_from_file_with_str(self):
        # 1) Pass in a plain Python string path
        src = utils.from_file(self.path)
        result = utils.read_input_source(src)
        self.assertEqual(result, self.content)

    def test_from_file_with_pathlib(self):
        # 2) Pass in a pathlib.Path
        p = pathlib.Path(self.path)
        src = utils.from_file(p)
        result = utils.read_input_source(src)
        self.assertEqual(result, self.content)

    def test_from_file_with_file_like(self):
        # 3) Pass in a file-like object
        # with open(self.path, 'r', encoding='utf-8') as f:
        with open(self.path, 'r') as f:
            src = utils.from_file(f)
            result = utils.read_input_source(src)
            self.assertEqual(result, self.content)

    def test_from_file_with_stringio(self):
        # 4) Pass in a text-mode, file-like object
        buf = io.StringIO(self.content)
        src = utils.from_file(buf)
        result = utils.read_input_source(src)
        self.assertEqual(result, self.content)

if __name__ == '__main__':
    unittest.main()
