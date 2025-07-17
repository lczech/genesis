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
import unittest
from pathlib import Path
from genesis.genesis.utils import core
import test_config

class TestGenesisUtilsCoreFs(unittest.TestCase):
    def test_current_path(self):
        # print(core.current_path())
        self.assertTrue(core.current_path())

        gp = Path(core.current_path()).absolute()
        op = Path(os.getcwd()).absolute()
        self.assertEqual(gp, op)

    def test_is_file(self):
        infile = os.path.join(test_config.test_data_dir, "utils/csv/table.csv")
        self.assertTrue( core.is_file(infile) )
        self.assertFalse( core.is_file("/road/to/nowhere") )

    def test_file_read_lines(self):
        infile = os.path.join(test_config.test_data_dir, "utils/csv/table.csv")
        lines = core.file_read_lines( infile )
        self.assertEqual( len(lines), 11 )
