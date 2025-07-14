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
from genesis.genesis import utils

class TestGenesisUtilsCoreInfo(unittest.TestCase):
    def test_info(self):
        self.assertIsInstance(utils.info_stdout_is_terminal(), bool)
        self.assertIsInstance(utils.info_stderr_is_terminal(), bool)
        self.assertIsInstance(utils.info_print_hardware(), str)

        self.assertIsInstance(utils.guess_number_of_threads(), int)
        self.assertGreater(utils.guess_number_of_threads(), 0)

    def test_compiler(self):
        # print(utils.info_print_compiler())
        self.assertTrue(utils.info_print_compiler())

    def test_hardware(self):
        # print(utils.info_print_hardware())
        self.assertTrue(utils.info_print_hardware())
