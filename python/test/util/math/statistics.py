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

from genesis.genesis.util import math

class TestGenesisutilMathStatistics(unittest.TestCase):
    def test_arithmetic_mean(self):
        self.assertAlmostEqual( 0.0,  math.arithmetic_mean([ ]))
        self.assertAlmostEqual( 7.0,  math.arithmetic_mean([ 4.0, 6.0, 11.0 ]))

    def test_weighted_arithmetic_mean(self):
        self.assertAlmostEqual(  0.0,  math.weighted_arithmetic_mean([], []) )
        self.assertAlmostEqual( 86.0,  math.weighted_arithmetic_mean([ 80.0, 90.0 ], [ 20.0, 30.0 ]) )

    def test_geometric_mean(self):
        self.assertAlmostEqual( 0.0,  math.geometric_mean([ ]))
        self.assertAlmostEqual( 4.0,  math.geometric_mean([ 2.0, 8.0 ]))

        self.assertRaises(
            ValueError,
            math.geometric_mean,
            [ 0.0 ]
        )
        self.assertRaises(
            ValueError,
            math.geometric_mean,
            [ -1.0 ]
        )

    def test_weighted_geometric_mean(self):
        self.assertAlmostEqual( 0.0,  math.weighted_geometric_mean([], []) )
        self.assertAlmostEqual( 1.0,  math.weighted_geometric_mean([ 1.0 ], [ 1.0 ]) )
        self.assertAlmostEqual( 6.0,  math.weighted_geometric_mean([ 6.0, 6.0 ], [ 3.0, 8.0 ]) )

        self.assertRaises(
            ValueError,
            math.weighted_geometric_mean,
            [  0.0 ], [ 1.0 ]
        )
        self.assertRaises(
            ValueError,
            math.weighted_geometric_mean,
            [ -1.0 ], [ 1.0 ]
        )
        self.assertRaises(
            RuntimeError,
            math.weighted_geometric_mean,
            [ 5.0 ], [ 1.0, 2.0 ]
        )
        self.assertRaises(
            RuntimeError,
            math.weighted_geometric_mean,
            [ 5.0, 2.0 ], [ 1.0 ]
        )

    def test_harmonic_mean(self):
        self.assertAlmostEqual(
            2.0, math.harmonic_mean([ 1.0, 4.0, 4.0 ],      math.HarmonicMeanZeroPolicy.kIgnore)
        )
        self.assertAlmostEqual(
            2.0, math.harmonic_mean([ 1.0, 4.0, 4.0, 0.0 ], math.HarmonicMeanZeroPolicy.kIgnore)
        )

    def test_weighted_harmonic_mean(self):
        self.assertAlmostEqual( 2.0,  math.weighted_harmonic_mean(
        [ 1.0, 4.0, 4.0 ], [ 1.0, 1.0, 1.0 ],
            math.HarmonicMeanZeroPolicy.kIgnore
        ))
        self.assertAlmostEqual( 2.0,  math.weighted_harmonic_mean(
            [ 1.0, 4.0, 4.0, 0.0 ], [ 1.0, 1.0, 1.0, 1.0 ],
            math.HarmonicMeanZeroPolicy.kIgnore
        ))

        self.assertRaises(
            ValueError,
            math.weighted_harmonic_mean,
            [  0.0 ], [ 1.0 ]
        )
        self.assertRaises(
            ValueError,
            math.weighted_harmonic_mean,
            [ -1.0 ], [ 1.0 ]
        )
        self.assertRaises(
            RuntimeError,
            math.weighted_harmonic_mean,
            [ 5.0 ], [ 1.0, 2.0 ]
        )
        self.assertRaises(
            RuntimeError,
            math.weighted_harmonic_mean,
            [ 5.0, 2.0 ], [ 1.0 ]
        )

    # def test_norms(self):
    #     vals = [ 1.0, 2.0, 3.0 ]
    #     self.assertAlmostEqual( 6.0,  math.manhattan_norm(vals))
    #     self.assertAlmostEqual( 3.0,  math.maximum_norm(vals))
    #     self.assertAlmostEqual( 3.7416573867739413,   math.euclidean_norm(vals))
    #     self.assertAlmostEqual( 0.78566403520078676,  math.aitchison_norm(vals))

    # def test_median(self):
    #     self.assertAlmostEqual( 1.5,  math.median([ 0, 1, 2, 3 ]))
