import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class NewickTest(unittest.TestCase):

	def test_from_and_to_string(self):
		True
		#LOL

	def test_newick_variants(self):

		tree = Tree()
		newick_string = ""
		writer = CommonTreeNewickWriter()

		self.assertTrue(tree.empty())

		# First, no branch lengths.
		writer.enable_branch_lengths( False )
		writer.trailing_new_line( False )

		# Stupid tree.
		newick_string = "();"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py1.txt' ))
		self.assertTrue( validate_topology(tree) )
		self.assertEqual(2,tree.node_count())
		self.assertFalse(tree.empty())
		self.assertEqual(newick_string,writer.to_string(tree))

		# No nodes are named.
		newick_string = "(,,(,));"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py2.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# Leaf nodes are named.
		newick_string = "(A,B,(C,D));"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py3.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# All nodes are named.
		newick_string = "(A,B,(C,D)E)F;"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py4.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# Now, test with branch lengths.
		writer.enable_branch_lengths( True )

		# All but root node have a distance to parent.
		newick_string = "(:0.1,:0.2,(:0.3,:0.4):0.5);"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py5.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# All have a distance to parent.
		# We never write out the root branch length, so this test is skipped.
		newick_string = "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py6.txt' ))
		self.assertTrue(validate_topology(tree))
		# EXPECT_EQ( newick_string, writer.to_string( tree ));

		# Distances and leaf names (popular).
		newick_string = "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py7.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# Distances and all names.
		newick_string = "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py8.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# A tree rooted on a leaf node (rare).
		newick_string = "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;"
		#tree = CommonTreeNewickReader().read( from_string( newick_string ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/newick_py9.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string,writer.to_string(tree))

		# All mixed, with comments and tags. Need to activate tags first.
		# We here only test the reading, and check for a simple standard written output,
		# instead of the hassle of using custom plugins to also write tags etc...
		newick_string = "( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};"
		newick_string2 = "((Ant:0.2,Bee:0.09)Inner:0.7,Coyote:0.5);"
		reader = CommonTreeNewickReader()
		reader.enable_tags( True )
		#tree = reader.read( from_string( newick_string ))
		tree = reader.read( from_file( './test/src/tree/newick_py10.txt' ))
		self.assertTrue(validate_topology(tree))
		self.assertEqual(newick_string2,writer.to_string(tree))

if __name__ == '__main__':
	unittest.main()
