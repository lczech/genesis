import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class PhylipTest(unittest.TestCase):

	def test_phylip_reader_aainterleaved(self):

		infile = "./test/data/sequence/aa_3_384_i.phylip"
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).mode( PhylipReader.Mode.kInterleaved ).valid_chars( amino_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual( 3, sset.size())
		self.assertEqual( 384, sset[0].length())
		self.assertEqual( "CATH_HUMAN", sset[2].label())
		self.assertEqual( "G-AVTPVKNQ", sset[0].sites()[160:10+160])

	def test_phylip_reader_dnainterleaved(self):

		infile = "./test/data/sequence/dna_5_42_i.phylip"
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).mode( PhylipReader.Mode.kInterleaved ).valid_chars( nucleic_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual( 5, sset.size())
		self.assertEqual( 42, sset[0].length())
		self.assertEqual( "Turkey", sset[0].label())
		self.assertEqual( "AAGCTNGGGC", sset[0].sites()[ 0: 10 ])

	def test_phylip_reader_aasequential(self):

		infile = "./test/data/sequence/aa_3_384_s.phylip"
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).valid_chars( amino_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual( 3, sset.size())
		self.assertEqual( 384, sset[0].length())
		self.assertEqual( "CATH_HUMAN", sset[2].label())
		self.assertEqual( "G-AVTPVKNQ", sset[0].sites()[160: 10+160 ])

	def test_phylip_reader_dnasequential(self):

		infile = "./test/data/sequence/dna_5_42_s.phylip"
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).valid_chars( nucleic_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual(5, sset.size())
		self.assertEqual(42, sset[0].length())
		self.assertEqual("Salmo gair", sset[1].label())
		self.assertEqual("CGGGCACGGT", sset[1].sites()[30: 10+30])

	def test_phylip_writer(self):

		infile = "./test/data/sequence/dna_5_42_s.phylip"
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).valid_chars( nucleic_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual( 5, sset.size())

		target = StringTarget()
		PhylipWriter().label_length(10).write( sset, to_string(target) )
		self.assertEqual(6, count_substring_occurrences( target.get(), "\n" ))

if __name__ == '__main__':
    unittest.main()
