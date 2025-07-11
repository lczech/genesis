import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class FastaTest(unittest.TestCase):

	def test_characteristics(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = SequenceSet()
		FastaReader().read( from_file(infile), sset)

		self.assertTrue( validate_chars( sset, nucleic_acid_codes_all() ))

		self.assertEqual( 4600,total_length( sset ))
		self.assertTrue( is_alignment( sset ))
		self.assertEqual( 0.1508695652173913,gapyness( sset, nucleic_acid_codes_undetermined() ))

		sh = site_histogram( sset );
		bf = base_frequencies( sset, nucleic_acid_codes_plain() );

		self.assertEqual( 694, sh['-'])
		self.assertEqual( 786, sh['A'])
		self.assertEqual( 1175,sh['C'])
		self.assertEqual( 1100,sh['G'])
		self.assertEqual( 845, sh['T'])

		self.assertEqual( 0.20122887864823349,bf['A'])
		self.assertEqual( 0.30081925243215568,bf['C'])
		self.assertEqual( 0.2816180235535074, bf['G'])
		self.assertEqual( 0.21633384536610342,bf['T'])

if __name__ == '__main__':
    unittest.main()
