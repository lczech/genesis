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
# along with this program.  If not, see <http:#www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

import unittest
from genesis.genesis.util import color

import random
import sys

class TestGenesisutilColorColor(unittest.TestCase):
    def assertColorEqual(self, expected, actual):
        """
        Compare two Color objects by their byte components.
        """
        self.assertEqual(expected.r_byte(), actual.r_byte())
        self.assertEqual(expected.g_byte(), actual.g_byte())
        self.assertEqual(expected.b_byte(), actual.b_byte())

    def test_gradient(self):
        # Heat gradient mapping
        ranges_heat = {
            0.0: color.color_from_bytes(0, 255, 0),
            0.5: color.color_from_bytes(255, 255, 0),
            1.0: color.color_from_bytes(255,   0, 0),
        }
        self.assertColorEqual(color.color_from_bytes(0, 255, 0), color.gradient(ranges_heat,  0.0))
        self.assertColorEqual(color.color_from_bytes(128,255,0), color.gradient(ranges_heat,  0.25))
        self.assertColorEqual(color.color_from_bytes(255,255,0), color.gradient(ranges_heat,  0.5))
        self.assertColorEqual(color.color_from_bytes(255,128,0), color.gradient(ranges_heat,  0.75))
        self.assertColorEqual(color.color_from_bytes(255,0,0),   color.gradient(ranges_heat,  1.0))
        # Clamping outside range
        self.assertColorEqual(color.color_from_bytes(0,255,0),  color.gradient(ranges_heat, -1.0))
        self.assertColorEqual(color.color_from_bytes(255,0,0),  color.gradient(ranges_heat,  2.0))

        # Longer gradient
        ranges_long = {
            0.0:  color.color_from_bytes(0, 0,   0),
            0.25: color.color_from_bytes(0, 255, 0),
            0.5:  color.color_from_bytes(255,255,0),
            1.0:  color.color_from_bytes(255,0,255),
        }
        checks = [
            (0.0,  (0,   0,   0)),
            (0.1,  (0, 102,   0)),
            (0.25, (0, 255,   0)),
            (0.3,  (51,255,   0)),
            (0.5,  (255,255, 0)),
            (0.6,  (255,204,51)),
            (0.75, (255,128,128)),
            (0.9,  (255,51,204)),
            (1.0,  (255,0,255)),
        ]
        for x, (r, g, b) in checks:
            col = color.gradient(ranges_long, x)
            self.assertColorEqual(color.color_from_bytes(r, g, b), col)

    def test_heat_gradient(self):
        self.assertColorEqual(color.color_from_bytes(0,255,0),   color.heat_gradient(0.0))
        self.assertColorEqual(color.color_from_bytes(128,255,0), color.heat_gradient(0.25))
        self.assertColorEqual(color.color_from_bytes(255,255,0), color.heat_gradient(0.5))
        self.assertColorEqual(color.color_from_bytes(255,128,0), color.heat_gradient(0.75))
        self.assertColorEqual(color.color_from_bytes(255,0,0),   color.heat_gradient(1.0))
        # Clamping outside range
        self.assertColorEqual(color.color_from_bytes(0,255,0),   color.heat_gradient(-1.0))
        self.assertColorEqual(color.color_from_bytes(255,0,0),   color.heat_gradient( 2.0))

    def test_to_and_from_hex(self):
        # Formatting
        self.assertEqual("#0033ff",   color.color_to_hex(color.color_from_bytes(0, 51, 255)))
        self.assertEqual("#4201fe",   color.color_to_hex(color.color_from_bytes(66, 1, 254)))
        self.assertEqual("000000",    color.color_to_hex(color.color_from_bytes(0,0,0), prefix=""))
        self.assertEqual("0XC0FFEE",  color.color_to_hex(color.color_from_bytes(192,255,238), prefix="0X", uppercase=True))
        # Parsing valid
        self.assertColorEqual(color.color_from_bytes(0,0,0),       color.color_from_hex("#000000"))
        self.assertColorEqual(color.color_from_bytes(171,205,239), color.color_from_hex("#abcdef"))
        self.assertColorEqual(color.color_from_bytes(255,255,255), color.color_from_hex("#fFFFff"))
        self.assertColorEqual(color.color_from_bytes(192,255,238), color.color_from_hex("c0ffee", prefix=""))
        # Malformed -> ValueError
        with self.assertRaises(ValueError): color.color_from_hex("")
        with self.assertRaises(ValueError): color.color_from_hex("abc0123")
        with self.assertRaises(ValueError): color.color_from_hex("#0000001")
        with self.assertRaises(ValueError): color.color_from_hex("#abcdez")

    def test_names(self):
        self.assertTrue(color.is_color_name_web("Dark Sea Green"))
        self.assertTrue(color.is_color_name_web("ForestGreen"))
        self.assertTrue(color.is_color_name_web("lawn_green"))
        self.assertTrue(color.is_color_name_web("lime green"))
        self.assertFalse(color.is_color_name_web("heavy metal"))
        self.assertFalse(color.is_color_name_web("applepie"))
        # Named colors
        self.assertColorEqual(color.color_from_bytes(199,21,133), color.color_from_name_web("MediumVioletRed"))
        self.assertColorEqual(color.color_from_bytes(205,92,92),  color.color_from_name_web("indian red"))
        with self.assertRaises(ValueError): color.color_from_name_web("")
        with self.assertRaises(ValueError): color.color_from_name_web("boot polish")

    def test_palette_spectral(self):
        cmap = color.ColorMap(color.color_list_spectral())
        norm = color.ColorNormalizationDiverging(-1.0, 1.0)
        checks = [
            (-1.0, (158, 1, 66)),
            (-0.5, (249,142,82)),
            (-0.4, (253,174,97)),
            ( 0.0, (255,255,191)),
            ( 0.4, (171,221,164)),
            ( 0.5, (137,208,165)),
            ( 1.0, (94, 79,162)),
        ]
        for x, (r,g,b) in checks:
            self.assertColorEqual(color.color_from_bytes(r,g,b), cmap(norm, x))

    def test_norm_boundary(self):
        norm = color.ColorNormalizationBoundary(3.0, 8.0, 5)
        expected_bounds = [3.0,4.0,5.0,6.0,7.0,8.0]
        self.assertEqual(expected_bounds, norm.boundaries())
        # interval
        tests_int = [
            (0.0, -1), (2.9, -1), (3.0,0), (3.1,0), (3.9,0),
            (4.0,1), (4.1,1), (7.0,4), (7.1,4), (7.9,4), (8.0,4), (8.5,5)
        ]
        for val, idx in tests_int:
            self.assertEqual(idx, norm.interval(val))
        # norm mapping
        tests_norm = [
            (0.0, -1.0), (2.9,-1.0), (3.0,0.0), (3.1,0.0), (3.9,0.0),
            (4.0,0.25), (4.1,0.25), (7.0,1.0), (7.1,1.0), (7.9,1.0), (8.0,1.0), (8.5,2.0)
        ]
        for val, expected in tests_norm:
            self.assertAlmostEqual(expected, norm(val), places=6)
