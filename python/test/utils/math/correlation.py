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

from genesis.genesis.utils import math

class TestGenesisUtilsMathCorrelation(unittest.TestCase):
    def test_pearson_correlation_coeffcient(self):
        a = [ 3.63, 3.02, 3.82, 3.42, 3.59, 2.87, 3.03, 3.46, 3.36, 3.3 ]
        b = [ 53.1, 49.7, 48.4, 54.2, 54.9, 43.7, 47.2, 45.2, 54.4, 50.4 ]
        c = math.pearson_correlation_coefficient(a, b)
        self.assertAlmostEqual( c, 0.47017723296840297 )

    def test_spearmans_rank_correlation_coeffcient(self):
        a = [106, 86, 100, 101, 99, 103, 97, 113, 112, 110]
        b = [7, 0, 27, 50, 28, 29, 20, 12, 6, 17]
        c = math.spearmans_rank_correlation_coefficient(a, b)
        self.assertAlmostEqual( c, -0.17575757575757575 )

    def test_kendalls_tau_correlation_coeffcient(self):
        a = [3, 1, 4, 2, 6, 5]
        b = [1, 2, 3, 4, 5, 6]

        # Simple tst case where all three Tau variants are the same
        ca = math.kendalls_tau_correlation_coefficient(a, b, math.KendallsTauMethod.kTauA)
        cb = math.kendalls_tau_correlation_coefficient(a, b, math.KendallsTauMethod.kTauB)
        cc = math.kendalls_tau_correlation_coefficient(a, b, math.KendallsTauMethod.kTauC)
        self.assertAlmostEqual( ca, 0.466666666666666666 )
        self.assertAlmostEqual( cb, 0.466666666666666666 )
        self.assertAlmostEqual( cc, 0.466666666666666666 )
