import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class ReferenceGenomeTest(unittest.TestCase):

	def test_basics(self):
		infile = "test/data/sequence/dna_10.fasta"
		rg = FastaReader().read_reference_genome( from_file(infile) )

		self.assertEqual( 10, rg.size() )
		self.assertTrue( rg.contains( "Di106BGTue" ) )
		self.assertEqual( "Di106BGTue", rg.begin().label() )
		self.assertEqual( 460, rg.find( "Di106BGTue" ).length() )
		self.assertEqual( rg.end(), rg.find( "X" ) )

if __name__ == '__main__':
	unittest.main()
