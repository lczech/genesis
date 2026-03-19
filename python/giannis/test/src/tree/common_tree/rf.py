import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class rfTest(unittest.TestCase):

	def test_rfdistance(self):

		trees = TreeSet()
		infile = "./test/data/tree/random-trees.newick";
		CommonTreeNewickReader().read( from_file( infile ), trees );
		self.assertEqual(10,trees.size())

		rf_mat = rf_distance_absolute( trees )

		rf_mat_exp = Matrix_unsigned_long_t( 10, 10, [
			0, 14, 14, 12, 14, 14, 14, 10, 14, 10,
			14, 0, 14, 14, 14, 14, 14, 14, 14, 14,
			14, 14, 0, 14, 12, 14, 10, 14, 14, 14,
			12, 14, 14, 0, 14, 14, 12, 12, 14, 14,
			14, 14, 12, 14, 0, 14, 14, 14, 12, 14,
			14, 14, 14, 14, 14, 0, 14, 12, 12, 14,
			14, 14, 10, 12, 14, 14, 0, 12, 14, 14,
			10, 14, 14, 12, 14, 12, 12, 0, 14, 12,
			14, 14, 14, 14, 12, 12, 14, 14, 0, 14,
			10, 14, 14, 14, 14, 14, 14, 12, 14, 0
		]);
		self.assertEqual(rf_mat_exp,rf_mat)

		rf_vec = rf_distance_absolute( trees[0], trees )

		rf_vec_exp = [0, 14, 14, 12, 14, 14, 14, 10, 14, 10]
		self.assertEqual(rf_vec_exp,rf_vec)

if __name__ == '__main__':
    unittest.main()
