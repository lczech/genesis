import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class TreeTest(unittest.TestCase):

	def test_basics(self):

		input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/tree_py.txt') )
		#tree = CommonTreeNewickReader().read( from_string(input) )

		self.assertEqual(2,degree(tree.root_node()))
		self.assertEqual(7,leaf_node_count(tree))
		self.assertEqual(6,inner_node_count(tree))
		self.assertEqual(13,tree.node_count())
		self.assertTrue(is_bifurcating(tree))

		self.assertEqual("R",tree.root_node().data().name)
		self.assertTrue(validate_topology(tree))

		copy_a = tree
		self.assertTrue(validate_topology(copy_a))

		copy_b = tree
		self.assertTrue(validate_topology(copy_b))

if __name__ == '__main__':
    unittest.main()
