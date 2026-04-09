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

import os
import io
import pathlib
import tempfile
import unittest
import inspect

from genesis.genesis import util
import test_config

# ==================================================================
#     Output Target Standard
# ==================================================================

def unique_temp_name_by_line() -> str:
    """
    Generate a unique temp filename that embeds the caller line number for uniqueness.
    """
    dir = tempfile.gettempdir()

    # Peek at the caller frame
    caller_frame = inspect.currentframe().f_back
    lineno       = caller_frame.f_lineno
    filename     = os.path.basename(caller_frame.f_code.co_filename)

    # Build a unique prefix per line in the code
    site_prefix = f"L{lineno}_"

    # atomic create & delete => no race when generating
    fd, path = tempfile.mkstemp(prefix=site_prefix, dir=dir)
    os.close(fd)
    os.unlink(path)
    return path

# ==================================================================
#     Output Target Standard
# ==================================================================

class TestGenesisutilIoOutputTarget(unittest.TestCase):
    def setUp(self):
        # Prepare some known content and a temp file path
        self.content = "Hello, world!\nThis is a test.\n"
        self.path = ""

    def tearDown(self):
        # Clean up the temp file if it still exists
        if os.path.isfile(self.path):
            os.remove(self.path)

    # ---------------------------------------------
    #     to_file
    # ---------------------------------------------

    def test_to_file_with_str_arg(self):
        # 1) Pass in a plain Python string path, in the function call,
        #    so that the target is destroyed and hence flushed.
        self.path = unique_temp_name_by_line()
        util.io.write_output_target(self.content, util.io.to_file(self.path))
        with open(self.path, 'r', encoding='utf-8') as f:
            result = f.read()
        self.assertEqual(result, self.content)

    def test_to_file_with_str_flush(self):
        # 2) Pass in a plain Python string path, then flush the target,
        #    so that the contents are written to the output.
        self.path = unique_temp_name_by_line()
        target = util.io.to_file(self.path)
        util.io.write_output_target(self.content, target)
        target.flush()
        with open(self.path, 'r', encoding='utf-8') as f:
            result = f.read()
        self.assertEqual(result, self.content)

    def test_to_file_with_pathlib(self):
        # 3) Pass in a pathlib.Path
        self.path = unique_temp_name_by_line()
        p = pathlib.Path(self.path)
        util.io.write_output_target(self.content, util.io.to_file(p))
        self.assertEqual(p.read_text(encoding='utf-8'), self.content)

    def test_to_file_with_file_like(self):
        # 4) Pass in an already-open file-like object
        self.path = unique_temp_name_by_line()
        with open(self.path, 'w', encoding='utf-8') as f:
            target = util.io.to_file(f)
            util.io.write_output_target(self.content, target)
            target.flush()
        with open(self.path, 'r', encoding='utf-8') as f:
            result = f.read()
        self.assertEqual(result, self.content)

    def test_to_file_with_stringio(self):
        # 5) Pass in an in-memory StringIO as a file-like
        buf = io.StringIO()
        target = util.io.to_file(buf)
        util.io.write_output_target(self.content, target)
        target.flush()
        self.assertEqual(buf.getvalue(), self.content)

    def test_to_file_with_bytesio(self):
        # 6) Pass in an in-memory BytesIO as a file-like
        buf = io.BytesIO()
        target = util.io.to_file(buf)
        util.io.write_output_target(self.content, target)
        target.flush()
        self.assertEqual(buf.getvalue(), self.content.encode())

    # ---------------------------------------------
    #     to_string
    # ---------------------------------------------

    def test_to_string_target(self):
        # 5) Use the convenience to_string() target
        target = util.io.to_string()
        util.io.write_output_target(self.content, target)
        target.flush()

        # Two ways of getting the string out of the target
        self.assertEqual(target.get(), self.content)
        self.assertEqual(str(target), self.content)

    def test_to_string_stringio(self):
        # 6) Use a StringIO object as target
        buf = io.StringIO()
        target = util.io.to_string(buf)
        util.io.write_output_target(self.content, target)
        target.flush()
        self.assertEqual(buf.getvalue(), self.content)

    def test_to_string_bytesio(self):
        # 6) Use a BytesIO object as target
        buf = io.BytesIO()
        target = util.io.to_string(buf)
        util.io.write_output_target(self.content, target)
        target.flush()
        self.assertEqual(buf.getvalue(), self.content.encode())

# ==================================================================
#     Output Target Context
# ==================================================================

class TestGenesisutilIoOutputContext(unittest.TestCase):
    def setUp(self):
        # Prepare some known content and a temp file path
        self.content = "Hello, world!\nThis is a test.\n"
        self.path = ""

    def tearDown(self):
        # Clean up the temp file if it still exists
        if os.path.isfile(self.path):
            os.remove(self.path)

    # ---------------------------------------------
    #     to_file
    # ---------------------------------------------

    def test_to_file_with_str_arg(self):
        # 1) Pass in a plain Python string path, in the function call,
        #    so that the target is destroyed and hence flushed.
        self.path = unique_temp_name_by_line()
        with util.io.to_file(self.path) as target:
            util.io.write_output_target(self.content, target)
        with open(self.path, 'r', encoding='utf-8') as f:
            result = f.read()
        self.assertEqual(result, self.content)

    def test_to_file_with_pathlib(self):
        # 2) Pass in a pathlib.Path
        self.path = unique_temp_name_by_line()
        p = pathlib.Path(self.path)
        with util.io.to_file(p) as target:
            util.io.write_output_target(self.content, target)
        self.assertEqual(p.read_text(encoding='utf-8'), self.content)

    def test_to_file_with_file_like(self):
        # 3) Pass in an already-open file-like object
        self.path = unique_temp_name_by_line()
        with open(self.path, 'w', encoding='utf-8') as f, util.io.to_file(f) as target:
            util.io.write_output_target(self.content, target)
        with open(self.path, 'r', encoding='utf-8') as f:
            result = f.read()
        self.assertEqual(result, self.content)

    def test_to_file_with_stringio(self):
        # 4) Pass in an in-memory StringIO as a file-like
        buf = io.StringIO()
        with util.io.to_file(buf) as target:
            util.io.write_output_target(self.content, target)
        self.assertEqual(buf.getvalue(), self.content)

    def test_to_file_with_bytesio(self):
        # 5) Pass in an in-memory BytesIO as a file-like
        buf = io.BytesIO()
        with util.io.to_file(buf) as target:
            util.io.write_output_target(self.content, target)
        self.assertEqual(buf.getvalue(), self.content.encode())

    # ---------------------------------------------
    #     to_string
    # ---------------------------------------------

    def test_to_string_target(self):
        # 5) Use the convenience to_string() target
        with util.io.to_string() as target:
            util.io.write_output_target(self.content, target)

            # Two ways of getting the string out of the target
            self.assertEqual(target.get(), self.content)
            self.assertEqual(str(target), self.content)

    def test_to_string_stringio(self):
        # 6) Use a StringIO object as target
        buf = io.StringIO()
        with util.io.to_string(buf) as target:
            util.io.write_output_target(self.content, target)
        self.assertEqual(buf.getvalue(), self.content)

    def test_to_string_bytesio(self):
        # 6) Use a BytesIO object as target
        buf = io.BytesIO()
        with util.io.to_string(buf) as target:
            util.io.write_output_target(self.content, target)
        self.assertEqual(buf.getvalue(), self.content.encode())

# ==================================================================
#     Output Target Gzip
# ==================================================================

class TestGenesisutilIoOutputGzip(unittest.TestCase):
    def test_file_compress_file(self):
        # Get some data
        raw_file = os.path.join(test_config.test_data_dir, "sequence/dna_10.fasta")
        raw_data = util.core.file_read( raw_file )

        # Prepare a new temp file
        tmp_file = unique_temp_name_by_line()

        # Write to the file
        gzip_level = util.io.GzipCompressionLevel.kDefaultCompression
        with util.io.to_file(tmp_file, gzip_level, False) as target:
            util.io.write_output_target(raw_data, target)

        # Read it back in again and see if it worked
        zip_data = util.io.read_input_source( util.io.from_file( tmp_file ))
        self.assertEqual(raw_data, zip_data)

        # Clean up the tmp file
        if os.path.isfile(tmp_file):
            os.remove(tmp_file)

    def test_file_compress_stream(self):
        # Get some data
        raw_file = os.path.join(test_config.test_data_dir, "sequence/dna_10.fasta")
        raw_data = util.core.file_read( raw_file )

        # Prepare a new temp file. Here, we are actually opening up a new file,
        # and use it to stream our data to.
        # tmp_file = unique_temp_name_by_line()
        tmp_file = tempfile.NamedTemporaryFile(delete=False)

        # Write it out to a zipped file.
        gzip_level = util.io.GzipCompressionLevel.kDefaultCompression
        with util.io.to_file(tmp_file, gzip_level) as target:
            util.io.write_output_target(raw_data, target)

        # Read it back in again and see if it worked
        zip_data = util.io.read_input_source( util.io.from_file( tmp_file.name ))
        self.assertEqual(raw_data, zip_data)

        # Clean up the tmp file
        if os.path.isfile(tmp_file.name):
            os.remove(tmp_file.name)

    def test_file_compress_stream_func(self):
        # Get some data
        raw_file = os.path.join(test_config.test_data_dir, "sequence/dna_10.fasta")
        raw_data = util.core.file_read( raw_file )

        # Alternative approach with a function, which in our tests has triggered segfauls before.
        def inner():
            # Prepare a new temp file, similar to above
            tmp_file = tempfile.NamedTemporaryFile(delete=False)

            # Write it out to a zipped file.
            gzip_level = util.io.GzipCompressionLevel.kDefaultCompression
            with util.io.to_file(tmp_file, gzip_level) as target:
                util.io.write_output_target(raw_data, target)
            return tmp_file.name
        tmp_name = inner()

        # Read it back in again and see if it worked
        zip_data = util.io.read_input_source( util.io.from_file( tmp_name ))
        self.assertEqual(raw_data, zip_data)

        # Clean up the tmp file
        if os.path.isfile(tmp_name):
            os.remove(tmp_name)
