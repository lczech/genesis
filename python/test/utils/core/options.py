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

class TestGenesisUtilsCoreOptions(unittest.TestCase):
    def test_thread_pool(self):
        utils.Options.get().init_global_thread_pool(2)
        self.assertEqual(utils.Options.get().global_thread_pool_size(), 2)

    def test_input_reading_policy(self):
        self.assertEqual(
            utils.Options.get().input_reading_thread_policy(),
            utils.Options.InputReadingThreadPolicy.kTrivialAsync
        )

    def test_allow_file_overwriting(self):
        self.assertEqual( utils.Options.get().allow_file_overwriting(), False )
        utils.Options.get().allow_file_overwriting(True)
        self.assertEqual( utils.Options.get().allow_file_overwriting(), True )
