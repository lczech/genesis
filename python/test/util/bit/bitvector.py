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
from genesis.genesis.util import bit

import random
import sys

class TestGenesisutilBitBitvector(unittest.TestCase):
    def test_operators(self):
        bv0 = bit.Bitvector( "010101010101" )
        bv1 = bit.Bitvector( "111000111000" )
        bv2 = bit.Bitvector( "111100001111" )

        bvz = bit.Bitvector( 12, False )
        bvo = bit.Bitvector( 12, True )

        self.assertEqual(12, bv0.size() )
        self.assertEqual(12, bv1.size() )
        self.assertEqual(12, bv2.size() )
        self.assertEqual(12, len(bv0) )
        self.assertEqual(12, len(bv1) )
        self.assertEqual(12, len(bv2) )

        self.assertEqual(bv0, (bv0 & bv0))
        self.assertEqual(bv1, (bv1 & bv1))
        self.assertEqual(bv2, (bv2 & bv2))

        self.assertEqual(bv0, (bv0 | bv0))
        self.assertEqual(bv1, (bv1 | bv1))
        self.assertEqual(bv2, (bv2 | bv2))

        self.assertEqual(bvz, (bv0 ^ bv0))
        self.assertEqual(bvz, (bv1 ^ bv1))
        self.assertEqual(bvz, (bv2 ^ bv2))

        self.assertEqual(bit.Bitvector( "101010101010" ), ~bv0)
        self.assertEqual(bit.Bitvector( "000111000111" ), ~bv1)
        self.assertEqual(bit.Bitvector( "000011110000" ), ~bv2)

        self.assertEqual(6, bit.pop_count(bv0))
        self.assertEqual(6, bit.pop_count(bv1))
        self.assertEqual(8, bit.pop_count(bv2))

        self.assertEqual(6, bit.pop_count(~bv0))
        self.assertEqual(6, bit.pop_count(~bv1))
        self.assertEqual(4, bit.pop_count(~bv2))

        self.assertTrue(bv0 != bv1)
        self.assertTrue(bv0 != bv2)
        self.assertTrue(bv1 != bv2)
        self.assertTrue(not (bv0 == bv1))
        self.assertTrue(not (bv0 == bv2))
        self.assertTrue(not (bv1 == bv2))

    def test_access(self):
        bv = bit.Bitvector( "010101010101" )

        # get
        self.assertFalse( bv[0] )
        self.assertTrue(  bv[1] )

        # set
        bv[0] = True
        bv[1] = False
        self.assertTrue(  bv[0] )
        self.assertFalse( bv[1] )

    def test_copy(self):
        bv_20 = bit.Bitvector( 20 )
        bv_10 = bit.Bitvector( 10, bv_20 )

        self.assertEqual( 10, bv_10.size() )
        bv_cp = bit.Bitvector( 20, bv_20 )
        self.assertEqual( 20, bv_cp.size() )

    def test_streams(self):
        size = 50
        bv = bit.Bitvector( size )
        for i in range(0, size):
            p = random.randint(0,size-1)
            bv.flip( p )

    def test_bool_vec(self):
        v = bit.make_bool_vector_from_indices( [1, 3, 5] )
        e = [False, True, False, True, False, True]
        self.assertEqual( v, e )

        v = bit.make_bool_vector_from_indices( [1, 3, 5], 8 )
        e = [False, True, False, True, False, True, False, False]
        self.assertEqual( v, e )

        self.assertRaises(
            ValueError,
            bit.make_bool_vector_from_indices,
            [1, 3, 5], 5
        )

    def test_bool_extended_operators(self):
        bv_s = bit.Bitvector( "0011" )
        bv_l = bit.Bitvector( "010101" )

        # Shorthands
        shorter = bit.BitwiseOperatorLengthPolicy.kUseShorter
        longer  = bit.BitwiseOperatorLengthPolicy.kUseLonger

        self.assertEqual( bit.Bitvector("0011"), bit.bitwise_and( bv_s, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0001"), bit.bitwise_and( bv_l, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0001"), bit.bitwise_and( bv_s, bv_l, shorter ))
        self.assertEqual( bit.Bitvector("010101"), bit.bitwise_and( bv_l, bv_l, shorter ))

        self.assertEqual( bit.Bitvector("0011"), bit.bitwise_and( bv_s, bv_s, longer ))
        self.assertEqual( bit.Bitvector("000100"), bit.bitwise_and( bv_l, bv_s, longer ))
        self.assertEqual( bit.Bitvector("000100"), bit.bitwise_and( bv_s, bv_l, longer ))
        self.assertEqual( bit.Bitvector("010101"), bit.bitwise_and( bv_l, bv_l, longer ))

        self.assertEqual( bit.Bitvector("0011"), bit.bitwise_or( bv_s, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0111"), bit.bitwise_or( bv_l, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0111"), bit.bitwise_or( bv_s, bv_l, shorter ))
        self.assertEqual( bit.Bitvector("010101"), bit.bitwise_or( bv_l, bv_l, shorter ))

        self.assertEqual( bit.Bitvector("0011"), bit.bitwise_or( bv_s, bv_s, longer ))
        self.assertEqual( bit.Bitvector("011101"), bit.bitwise_or( bv_l, bv_s, longer ))
        self.assertEqual( bit.Bitvector("011101"), bit.bitwise_or( bv_s, bv_l, longer ))
        self.assertEqual( bit.Bitvector("010101"), bit.bitwise_or( bv_l, bv_l, longer ))

        self.assertEqual( bit.Bitvector("0000"), bit.bitwise_xor( bv_s, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0110"), bit.bitwise_xor( bv_l, bv_s, shorter ))
        self.assertEqual( bit.Bitvector("0110"), bit.bitwise_xor( bv_s, bv_l, shorter ))
        self.assertEqual( bit.Bitvector("000000"), bit.bitwise_xor( bv_l, bv_l, shorter ))

        self.assertEqual( bit.Bitvector("0000"), bit.bitwise_xor( bv_s, bv_s, longer ))
        self.assertEqual( bit.Bitvector("011001"), bit.bitwise_xor( bv_l, bv_s, longer ))
        self.assertEqual( bit.Bitvector("011001"), bit.bitwise_xor( bv_s, bv_l, longer ))
        self.assertEqual( bit.Bitvector("000000"), bit.bitwise_xor( bv_l, bv_l, longer ))
