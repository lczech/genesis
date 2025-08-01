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

class TestGenesisUtilsMathRanking(unittest.TestCase):
    def test_ranking_sorted(self):
        # Example from https://rosettacode.org/wiki/Ranking_methods
        numbers = [ 39, 41, 41, 41, 42, 42, 44 ]
        self.assertAlmostEqual( [ 1, 2, 2, 2, 5, 5, 7 ], math.ranking_standard( numbers ))
        self.assertAlmostEqual( [ 1, 4, 4, 4, 6, 6, 7 ], math.ranking_modified( numbers ))
        self.assertAlmostEqual( [ 1, 2, 2, 2, 3, 3, 4 ], math.ranking_dense( numbers ))
        self.assertAlmostEqual( [ 1, 2, 3, 4, 5, 6, 7 ], math.ranking_ordinal( numbers ))
        self.assertAlmostEqual(
            [ 1.0, 3.0, 3.0, 3.0, 5.5, 5.5, 7.0 ],
            math.ranking_fractional( numbers )
        )

        # Example from https://en.wikipedia.org/wiki/Ranking
        self.assertAlmostEqual(
            [ 1.5, 1.5, 3.0, 4.5, 4.5, 6.0, 8.0, 8.0, 8.0 ],
            math.ranking_fractional([ 1.0, 1.0, 2.0, 3.0, 3.0, 4.0, 5.0, 5.0, 5.0 ])
        )

    def test_ranking_unsorted(self):
        # Example from https://rosettacode.org/wiki/Ranking_methods
        numbers = [ 41, 39, 42, 41, 44, 42, 41 ]
        self.assertAlmostEqual( [ 2, 1, 5, 2, 7, 5, 2 ], math.ranking_standard( numbers ))
        self.assertAlmostEqual( [ 4, 1, 6, 4, 7, 6, 4 ], math.ranking_modified( numbers ))
        self.assertAlmostEqual( [ 2, 1, 3, 2, 4, 3, 2 ], math.ranking_dense( numbers ))
        self.assertAlmostEqual( [ 2, 1, 5, 3, 7, 6, 4 ], math.ranking_ordinal( numbers ))
        self.assertAlmostEqual(
            [ 3.0, 1.0, 5.5, 3.0, 7.0, 5.5, 3.0 ],
            math.ranking_fractional( numbers )
        )

        # Example from https://en.wikipedia.org/wiki/Ranking
        self.assertAlmostEqual(
            [ 4.5, 6.0, 8.0, 1.5, 3.0, 4.5, 8.0, 8.0, 1.5 ],
            math.ranking_fractional([ 3.0, 4.0, 5.0, 1.0, 2.0, 3.0, 5.0, 5.0, 1.0 ])
        )
