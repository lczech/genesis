import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class CommonTreeTest(unittest.TestCase):

	def test_edge_color_branch_length_gradient(self):

		infile = "./test/data/tree/distances.newick"
		tree = CommonTreeNewickReader().read( from_file( infile ))

		# Colorize the branches according to their length.
		colors = edge_color_branch_length_gradient(tree)

		# We expect at least one branch to have max color (red) and one to have min color (green).
		count = 0
		for i in colors:
			if (i.r() == 1) and (i.g() == 0) and (i.b() == 0):
				count = count + 1
		self.assertLessEqual(1,count)
		count = 0
		for i in colors:
			if (i.r() == 0) and (i.g() == 1) and (i.b() == 0):
				count = count + 1
		self.assertLessEqual(1,count)

	def test_node_names(self):

		tree = Tree()

		# Using a tree with all names set to some value.
		input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"
		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/common_tree/common_tree_py1.txt' ))

		self.assertEqual(13,tree.node_count())
		self.assertEqual(7,leaf_node_count(tree))

		names_all    = node_names(tree)
		names_leaves = node_names(tree, True)

		self.assertEqual(13,len(names_all))
		self.assertEqual(7,len(names_leaves))

		# Using a tree where some names are empty.
		input = "((A,(B,))E,((,(G,H))J,)L);"
		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/common_tree/common_tree_py2.txt' ))

		self.assertEqual(13,tree.node_count())
		self.assertEqual(7,leaf_node_count(tree))

		names_all    = node_names(tree)
		names_leaves = node_names(tree, True)

		self.assertEqual(7,len(names_all))
		self.assertEqual(4,len(names_leaves))

if __name__ == '__main__':
    unittest.main()
