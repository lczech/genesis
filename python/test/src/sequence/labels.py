import numpy as np
import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class LabelsTest(unittest.TestCase):

	def test_filter_by_label_test(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = SequenceSet()
		FastaReader().read( from_file(infile), sset)

		self.assertEqual( 10, sset.size())
		self.assertEqual( 4600, total_length( sset ))

		good_labels = {
		    "Di106BGTue",
		    "Di145BGTue",
		    "Di307XishTrBotG",
		    "cs009BGTue",
		    "he005BGTue",
		    "he112BGTue",
		    "ne201NEStates"
		}

		bad_labels = {
		    "cs103MorArb",
		    "he111BGTue",
		    "ne203NEStates"
		}

		filter_by_label_list( sset, bad_labels )
		self.assertEqual( 7, sset.size())
		self.assertEqual( good_labels, labels( sset ))

		filter_by_label_list( sset, good_labels )
		self.assertEqual( 0, sset.size())
		self.assertTrue( sset.empty())

	def test_guess_abundances(self):
		good = ( "abc", 123 )

		self.assertEqual( good, guess_sequence_abundance( "abc_123" ))
		self.assertEqual( good, guess_sequence_abundance( "abc;size=123;" ))
		self.assertEqual( good, guess_sequence_abundance( "abc;size=123" ))
		self.assertEqual( good, guess_sequence_abundance( "abc;key=value;size=123;foo=bar;" ))

		self.assertEqual( ( "abc_size=123_", np.size(1) ), guess_sequence_abundance( "abc_size=123_" ))
		self.assertEqual( ( "abcsize=123", np.size(1) ), guess_sequence_abundance( "abcsize=123" ))
		self.assertEqual( ( "abc", np.size(1) ), guess_sequence_abundance( "abc;size=123x" ))

		self.assertEqual( ( "abc_",       np.size(1) ), guess_sequence_abundance( "abc_" ))
		self.assertEqual( ( "abc;size=",  np.size(1) ), guess_sequence_abundance( "abc;size=" ))
		self.assertEqual( ( "abc_123x",   np.size(1) ), guess_sequence_abundance( "abc_123x" ))
		self.assertEqual( ( "abc_x",      np.size(1) ), guess_sequence_abundance( "abc_x" ))
		self.assertEqual( ( "abc", np.size(1) ), guess_sequence_abundance( "abc;size=x" ))

	def test_label_attributes(self):
		exp1 = LabelAttributes()
		exp1.label = "bla"
		exp1.attributes = { "size":"123", "weight": "100" }
		act1 = label_attributes( "bla;size=123;weight=100;" )
		self.assertEqual( exp1.label, act1.label )
		self.assertEqual( exp1.attributes, act1.attributes )

		exp2 =LabelAttributes()
		exp2.label = "bla"
		exp2.attributes = {}
		act2 = label_attributes( "bla;" )
		self.assertEqual( exp2.label, act2.label )
		self.assertEqual( exp2.attributes, act2.attributes )

		self.assertRaises( RuntimeError, label_attributes, "bla;foo")

if __name__ == '__main__':
    unittest.main()
