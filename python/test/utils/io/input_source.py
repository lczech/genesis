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
import test_config

# ==================================================================
#     Input Source Standard
# ==================================================================

class TestGenesisUtilsIoInputSource(unittest.TestCase):
    def setUp(self):
        # Create a temp file with known content
        self.content = "Hello, world!\nThis is a test.\n"
        tmp = tempfile.NamedTemporaryFile(delete=False, mode='w', encoding='utf-8')
        tmp.write(self.content)
        tmp.flush()
        tmp.close()
        self.path = tmp.name

    def tearDown(self):
        if os.path.isfile(self.path):
            os.remove(self.path)

    # ---------------------------------------------
    #     from_file
    # ---------------------------------------------

    def test_from_file_with_str(self):
        # 1) Pass in a plain Python string path
        src = utils.io.from_file(self.path)
        result = utils.io.read_input_source(src)
        self.assertEqual(result, self.content)

    def test_from_file_with_pathlib(self):
        # 2) Pass in a pathlib.Path
        p = pathlib.Path(self.path)
        src = utils.io.from_file(p)
        result = utils.io.read_input_source(src)
        self.assertEqual(result, self.content)
        self.assertEqual(result, self.content)

    def test_from_file_with_file_like(self):
        # 3) Pass in a file-like object
        # with open(self.path, 'r', encoding='utf-8') as f:
        with open(self.path, 'r') as f:
            src = utils.io.from_file(f)
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

    def test_from_file_with_stringio(self):
        # Pass in a text-mode, file-like object
        buf = io.StringIO(self.content)
        src = utils.io.from_file(buf)
        result = utils.io.read_input_source(src)
        self.assertEqual(result, self.content)

    # ---------------------------------------------
    #     from_string
    # ---------------------------------------------

    def test_from_string_var(self):
        string = utils.io.from_string(self.content)
        result = utils.io.read_input_source(string)
        self.assertEqual(result, self.content)

    def test_from_string_move(self):
        string = utils.io.from_string(str(self.content))
        result = utils.io.read_input_source(string)
        self.assertEqual(result, self.content)

    def test_from_string_buffer(self):
        # bytes literal --> buffer‑protocol path
        data = self.content.encode('ascii')
        src = utils.io.from_string(data)
        result = utils.io.read_input_source(src)
        self.assertEqual(result, self.content)

# ==================================================================
#     Input Source Context
# ==================================================================

class TestGenesisUtilsIoInputContext(unittest.TestCase):
    def setUp(self):
        # Create a temp file with known content
        self.content = "Hello, world!\nThis is a test.\n"
        tmp = tempfile.NamedTemporaryFile(delete=False, mode='w', encoding='utf-8')
        tmp.write(self.content)
        tmp.flush()
        tmp.close()
        self.path = tmp.name

    def tearDown(self):
        if os.path.isfile(self.path):
            os.remove(self.path)

    # ---------------------------------------------
    #     from_file
    # ---------------------------------------------

    def test_from_file_with_str(self):
        # 1) Pass in a plain Python string path
        with utils.io.from_file(self.path) as src:
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

    def test_from_file_with_pathlib(self):
        # 2) Pass in a pathlib.Path
        p = pathlib.Path(self.path)
        with utils.io.from_file(p) as src:
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

    def test_from_file_with_file_like(self):
        # 3) Pass in a file-like object
        # with open(self.path, 'r', encoding='utf-8') as f:
        with open(self.path, 'r') as f, utils.io.from_file(f) as src:
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

    def test_from_file_with_stringio(self):
        # 4) Pass in a text-mode, file-like object
        buf = io.StringIO(self.content)
        with utils.io.from_file(buf) as src:
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

    # ---------------------------------------------
    #     from_string
    # ---------------------------------------------

    def test_from_string_var(self):
        with utils.io.from_string(self.content) as string:
            result = utils.io.read_input_source(string)
            self.assertEqual(result, self.content)

    def test_from_string_move(self):
        with utils.io.from_string(str(self.content)) as string:
            result = utils.io.read_input_source(string)
            self.assertEqual(result, self.content)

    def test_from_string_buffer(self):
        # bytes literal --> buffer‑protocol path
        data = self.content.encode('ascii')
        with utils.io.from_string(data) as src:
            result = utils.io.read_input_source(src)
            self.assertEqual(result, self.content)

# ==================================================================
#     Input Source Gzip
# ==================================================================

class TestGenesisUtilsIoInputGzip(unittest.TestCase):
    def test_file_decompress(self):
        raw_file = os.path.join(test_config.test_data_dir, "sequence/dna_10.fasta")
        zip_file = os.path.join(test_config.test_data_dir, "sequence/dna_10.fasta.gz")

        raw_data = utils.core.file_read( raw_file )
        zip_data = utils.io.read_input_source( utils.io.from_file( zip_file ))
        self.assertEqual(raw_data, zip_data)
