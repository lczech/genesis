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

class TestGenesisutilBitTwobitvector(unittest.TestCase):

    # -------------------------------
    #     Twobit Vector
    # -------------------------------

    def test_from_to_nucleic_acids(self):
        seq = "ACGTGCA"
        tv = bit.from_nucleic_acids(seq)
        # size and to_nucleic_acids
        self.assertEqual(tv.size(), len(seq))
        self.assertEqual(len(tv), len(seq))
        self.assertEqual(bit.to_nucleic_acids(tv), seq)

    def test_operators(self):
        seq = "ACGT"
        tv = bit.from_nucleic_acids(seq)
        # __len__
        self.assertEqual(len(tv), 4)
        # __getitem__ returns ValueType
        self.assertEqual(tv[0], bit.TwobitVector.ValueType.A)
        # __setitem__ and back to string
        tv[0] = bit.TwobitVector.ValueType.T
        self.assertEqual(bit.to_nucleic_acids(tv), "TCGT")
        # validate
        self.assertTrue(tv.validate())
        # hash changes when mutated
        original_hash = tv.hash()
        tv[1] = bit.TwobitVector.ValueType.G
        self.assertNotEqual(tv.hash(), original_hash)
        # clear
        tv.clear()
        self.assertEqual(len(tv), 0)

    # -------------------------------
    #     Substitutions
    # -------------------------------

    def test_substitution_iterator(self):
        seq = "AA"
        tv = bit.from_nucleic_acids(seq)
        seen_positions = set()
        for it in bit.iterate_substitutions(tv):
            # iterator instance methods
            pos = it.position()
            h = it.hash()
            vec = it.vector()
            s = bit.to_nucleic_acids(vec)
            seen_positions.add(pos)
            # mutated length equals original
            self.assertEqual(len(s), len(seq))
            # exactly one base differs
            diff = sum(1 for a, b in zip(s, seq) if a != b)
            self.assertEqual(diff, 1)
            # hash matches vector.hash()
            self.assertEqual(h, vec.hash())
        # ensure we covered all positions
        self.assertEqual(seen_positions, set(range(len(seq))))

    def test_substitution_list(self):
        seq = "AA"
        tv = bit.from_nucleic_acids(seq)
        subs = list()
        for it in bit.substitutions(tv):
            # print(bit.to_nucleic_acids(it))
            subs.append(bit.to_nucleic_acids(it))
        # print(subs)

        # Should be len(seq) * 3 substitutions
        self.assertEqual(len(subs), len(seq) * 3)
        self.assertEqual(subs, ['CA', 'GA', 'TA', 'AC', 'AG', 'AT'])

    # -------------------------------
    #     Deletions
    # -------------------------------

    def test_deletion_iterator(self):
        seq = "ACG"
        tv = bit.from_nucleic_acids(seq)
        seen_positions = set()
        for it in bit.iterate_deletions(tv):
            pos = it.position()
            vec = it.vector()
            s = bit.to_nucleic_acids(vec)
            seen_positions.add(pos)
            # one base shorter
            self.assertEqual(len(s), len(seq) - 1)
            # removal yields original
            # cannot directly compare strings with missing char;
            # test by reinserting original has seq[pos] removed
            expected = seq[:pos] + seq[pos+1:]
            self.assertEqual(s, expected)
        # ensure we covered all positions
        self.assertEqual(seen_positions, set(range(len(seq))))

    def test_deletion_list(self):
        seq = "ACG"
        tv = bit.from_nucleic_acids(seq)
        dels = list()
        for it in bit.deletions(tv):
            # print(bit.to_nucleic_acids(it))
            dels.append(bit.to_nucleic_acids(it))
        # print(dels)

        # Should be len(seq) deletions
        self.assertEqual(len(dels), len(seq))
        self.assertEqual(dels, ['CG', 'AG', 'AC'])

    # -------------------------------
    #     Insertions
    # -------------------------------

    def test_insertion_iterator(self):
        seq = "CGT"
        tv = bit.from_nucleic_acids(seq)
        seen_positions = set()
        for it in bit.iterate_insertions(tv):
            pos = it.position()
            vec = it.vector()
            s = bit.to_nucleic_acids(vec)
            seen_positions.add(pos)
            # one base longer
            self.assertEqual(len(s), len(seq) + 1)
            # removing inserted base recovers original
            recovered = s[:pos] + s[pos+1:]
            self.assertEqual(recovered, seq)
        # positions should be within valid insertion indices [0..len(seq)]
        self.assertEqual(seen_positions, set(range(len(seq) + 1)))
        self.assertTrue(all(0 <= p <= len(seq) for p in seen_positions))

    def test_insertion_list(self):
        seq = "CGT"
        tv = bit.from_nucleic_acids(seq)
        ins = list()
        for it in bit.insertions(tv):
            # print(bit.to_nucleic_acids(it))
            ins.append(bit.to_nucleic_acids(it))
        # print(ins)

        self.assertEqual(len(ins), 4 * (len(seq) + 1))
        self.assertEqual(
            ins,
            [
                'ACGT', 'CCGT', 'GCGT', 'TCGT',
                'CAGT', 'CCGT', 'CGGT', 'CTGT',
                'CGAT', 'CGCT', 'CGGT', 'CGTT',
                'CGTA', 'CGTC', 'CGTG', 'CGTT'
            ]
        )
