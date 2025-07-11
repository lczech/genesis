import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class DictTest(unittest.TestCase):

	def tair10_dict_file_test( self, dict ):
		self.assertEqual(              7, dict.size())
		self.assertEqual(            "1", dict[0].name)
		self.assertEqual(       30427671, dict[0].length)
		self.assertEqual(            "2", dict[1].name)
		self.assertEqual(       19698289, dict[1].length)
		self.assertEqual(            "3", dict[2].name)
		self.assertEqual(       23459830, dict[2].length)
		self.assertEqual(            "4", dict[3].name)
		self.assertEqual(       18585056, dict[3].length)
		self.assertEqual(            "5", dict[4].name)
		self.assertEqual(       26975502, dict[4].length)
		self.assertEqual( "mitochondria", dict[5].name)
		self.assertEqual(         366924, dict[5].length)
		self.assertEqual(  "chloroplast", dict[6].name)
		self.assertEqual(         154478, dict[6].length)

		self.assertNotEqual( dict.end(), dict.find( "1" ))
		self.assertEqual( dict.end(), dict.find( "X" ))
		self.assertTrue( dict.contains( "1" ))
		self.assertFalse( dict.contains( "X" ))
		self.assertEqual( "1", dict.get("1").name)

		self.assertEqual( 0, dict.index_of( "1" ))
		self.assertEqual( 1, dict.index_of( "2" ))
		self.assertEqual( 2, dict.index_of( "3" ))
		self.assertEqual( 3, dict.index_of( "4" ))
		self.assertEqual( 4, dict.index_of( "5" ))
		self.assertEqual( 5, dict.index_of( "mitochondria" ))
		self.assertEqual( 6, dict.index_of( "chloroplast" ))
		self.assertRaises( RuntimeError, dict.index_of, "X")

	def test_dict_reader(self):
		infile = "./test/data/sequence/TAIR10_chr_all.dict"
		dict = read_sequence_dict( from_file( infile ))
		self.tair10_dict_file_test( dict )

	def test_fai_reader(self):
		infile = "./test/data/sequence/TAIR10_chr_all.fa.fai"
		dict = read_sequence_fai( from_file( infile ))
		self.tair10_dict_file_test( dict )

	def test_fasta_reader(self):
		infile = "./test/data/sequence/dna_10.fasta"
		dict = FastaReader().read_dict( from_file( infile ))

		self.assertEqual(           10, dict.size())
		self.assertEqual( "Di106BGTue", dict[0].name)
		self.assertEqual(          460, dict[0].length)

	def test_fasta_convert(self):
		infile = "./test/data/sequence/dna_10.fasta"
		seqs = FastaReader().read(from_file( infile ))
		dict = sequence_set_to_dict( seqs )

		self.assertEqual(           10, dict.size())
		self.assertEqual( "Di106BGTue", dict[0].name)
		self.assertEqual(          460, dict[0].length)
		self.assertTrue( verify( dict, seqs, False ))
		self.assertTrue( verify( dict, seqs, True ))

if __name__ == '__main__':
    unittest.main()