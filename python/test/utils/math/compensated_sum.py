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

class TestGenesisUtilsMathCompensatedSum(unittest.TestCase):
    def test_kahan_sum(self):
        s = utils.math.CompensatedSum_genesis_utils_math_KahanSummation_t()
        for i in range(0, 100):
            s += 0.1
            # s.add(0.1)
        self.assertEqual( s.get(), 10.0 )
        self.assertEqual( float(s), 10.0 )

    def test_neumaier_sum(self):
        s = utils.math.CompensatedSum_genesis_utils_math_NeumaierSummation_t()
        for i in range(0, 100):
            s += 0.1
            # s.add(0.1)
        self.assertEqual( s.get(), 10.0 )
        self.assertEqual( float(s), 10.0 )

    def test_klein_sum(self):
        s = utils.math.CompensatedSum_genesis_utils_math_KleinSummation_t()
        for i in range(0, 100):
            s += 0.1
            # s.add(0.1)
        self.assertEqual( s.get(), 10.0 )
        self.assertEqual( float(s), 10.0 )
