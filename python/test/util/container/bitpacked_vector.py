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
from genesis.genesis.util import container

class TestGenesisutilContainerBitpackedVector(unittest.TestCase):
    def test_direct_access_64bit(self):
        bp = container.BitpackedVector_uint64_t(10, 64)
        bp.set(0, 1234567890123456789)
        bp.set(1, 9876543210987654321)
        self.assertEqual(bp.get(0), 1234567890123456789)
        self.assertEqual(bp.get(1), 9876543210987654321)

    def test_direct_access_32bit(self):
        bp = container.BitpackedVector_uint64_t(10, 32)
        bp.set(0, 1234567890)
        bp.set(1, 987654321)
        self.assertEqual(bp.get(0), 1234567890)
        self.assertEqual(bp.get(1), 987654321)

    def test_bit_packed_7bit(self):
        bp = container.BitpackedVector_uint64_t(10, 7)
        bp.set(0, 15)
        bp.set(1, 100)
        bp.set(2, 7)
        self.assertEqual(bp.get(0), 15)
        self.assertEqual(bp.get(1), 100)
        self.assertEqual(bp.get(2), 7)

    def test_bit_packed_16bit(self):
        bp = container.BitpackedVector_uint64_t(10, 16)
        bp.set(0, 65535)
        bp.set(1, 32768)
        bp.set(2, 12345)
        self.assertEqual(bp.get(0), 65535)
        self.assertEqual(bp.get(1), 32768)
        self.assertEqual(bp.get(2), 12345)

    def test_edge_min_max(self):
        bp = container.BitpackedVector_uint64_t(10, 7)
        bp.set(0, 0)
        self.assertEqual(bp.get(0), 0)
        bp.set(0, 127)
        self.assertEqual(bp.get(0), 127)

    def test_out_of_bounds_index(self):
        bp = container.BitpackedVector_uint64_t(10, 7)
        bp.set(0, 10)
        self.assertEqual(bp.get(0), 10)
        with self.assertRaises(IndexError):
            bp.set(10, 20)
        with self.assertRaises(IndexError):
            bp.get(10)

    def test_constructor_invalid_bit_width(self):
        with self.assertRaises(ValueError):
            container.BitpackedVector_uint64_t(10, 0)
        with self.assertRaises(ValueError):
            container.BitpackedVector_uint64_t(10, 65)

    def test_value_out_of_range(self):
        bp = container.BitpackedVector_uint64_t(10, 7)
        with self.assertRaises(ValueError):
            bp.set(0, 200)

    def test_direct_smaller_type(self):
        bp = container.BitpackedVector_uint64_t(10, 32)
        bp.set(0, 1234567890)
        bp.set(1, 987654321)
        self.assertEqual(bp.get(0), 1234567890)
        self.assertEqual(bp.get(1), 987654321)

    def test_full_range_byte(self):
        n = 1000
        for bw in range(1, 9):
            bp = container.BitpackedVector_uint64_t(n, bw)
            limit = 1 << bw
            for i in range(n):
                bp.set(i, i % limit)
            for i in range(n):
                self.assertEqual(bp.get(i), i % limit)
