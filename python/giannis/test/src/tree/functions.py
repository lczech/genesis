import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class FunctionsTest(unittest.TestCase):

	def TestSubtreeSize( self, link_index, out_subtree_size ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		st_size = subtree_size( tree, tree.link_at( link_index ))
		self.assertEqual(out_subtree_size,st_size)
		print("with link index " + str(link_index))

	def TestSubtreeSizes( self, node_name, out_sizes ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		node = find_node( tree, node_name )
		self.assertTrue(node)

		st_sizes = subtree_sizes( tree, node )
		self.assertEqual(out_sizes,st_sizes)
		print(" with start node " + str(node_name))

	def TestSubtreeMaxPathHeight( self, node_name, out_size ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		node = find_node( tree, node_name )
		self.assertTrue(node)

		#We are lazy and only evaluate the links towards the root.
		st_size = subtree_max_path_height( tree, node.link().outer() )
		self.assertEqual(out_size,st_size)
		print(" with start node " + str(node_name))

	def TestTreeLCA( self, node_name_a, node_name_b, node_name_lca ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		node_a = find_node( tree, node_name_a )
		node_b = find_node( tree, node_name_b )
		self.assertTrue(node_a)
		self.assertTrue(node_b)

		node_lca = lowest_common_ancestor( node_a, node_b )
		self.assertEqual(node_name_lca,node_lca.data().name)
		print(" with nodes " + str(node_name_a) + ", " + str(node_name_b) + " and LCA " + str(node_name_lca))

	def test_edge_sides(self):

		input = "((B,(D,E)C)A,F,(H,I)G)R;"
		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		edge_side_mat = edge_sides( tree )

		exp = Matrix_signed_char_t( 9, 9, [
			0,  1,  1, -1, -1, -1, -1, -1, -1,
			-1,  0, -1, -1, -1, -1, -1, -1, -1,
			-1, -1,  0, -1, -1, -1, -1, -1, -1,
			-1, -1, -1,  0, -1, -1, -1, -1, -1,
			-1, -1, -1, -1,  0,  1,  1,  1,  1,
			-1, -1, -1, -1, -1,  0,  1,  1, -1,
			-1, -1, -1, -1, -1, -1,  0, -1, -1,
			-1, -1, -1, -1, -1, -1, -1,  0, -1,
			-1, -1, -1, -1, -1, -1, -1, -1,  0
		])

		self.assertEqual( exp, edge_side_mat)

	def test_node_root_directions(self):

		input = "((B,(D,E)C)A,F,(H,I)G)R;"
		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		node_root_mat = node_root_direction_matrix( tree )

		exp = Matrix_signed_char_t( 10, 10, [
			0, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1,  0, -1, -1,  1,  1,  1,  1,  1,  1,
			1,  1,  0,  1,  1,  1,  1,  1,  1,  1,
			1,  1,  1,  0,  1,  1,  1,  1,  1,  1,
			1,  1,  1,  1,  0,  1,  1,  1,  1,  1,
			1,  1,  1,  1,  1,  0, -1, -1, -1, -1,
			1,  1,  1,  1,  1,  1,  0, -1, -1,  1,
			1,  1,  1,  1,  1,  1,  1,  0,  1,  1,
			1,  1,  1,  1,  1,  1,  1,  1,  0,  1,
			1,  1,  1,  1,  1,  1,  1,  1,  1,  0,
		])

		self.assertEqual(exp, node_root_mat)

	def test_subtree_size(self):

		self.TestSubtreeSize(  0, 5 )
		self.TestSubtreeSize(  1, 1 )
		self.TestSubtreeSize(  2, 3 )
		self.TestSubtreeSize(  3, 7 )
		self.TestSubtreeSize(  4, 1 )
		self.TestSubtreeSize(  5, 1 )
		self.TestSubtreeSize(  6, 9 )
		self.TestSubtreeSize(  7, 9 )
		self.TestSubtreeSize(  8, 9 )
		self.TestSubtreeSize(  9, 5 )
		self.TestSubtreeSize( 10, 1 )
		self.TestSubtreeSize( 11, 3 )
		self.TestSubtreeSize( 12, 7 )
		self.TestSubtreeSize( 13, 1 )
		self.TestSubtreeSize( 14, 1 )
		self.TestSubtreeSize( 15, 9 )
		self.TestSubtreeSize( 16, 9 )
		self.TestSubtreeSize( 17, 9 )

	def test_subtree_sizes(self):

		self.TestSubtreeSizes( "R", [ 9, 2, 0, 0, 0, 4, 2, 0, 0, 0 ] )
		self.TestSubtreeSizes( "A", [ 4, 2, 0, 0, 0, 9, 2, 0, 0, 0 ] )
		self.TestSubtreeSizes( "B", [ 4, 2, 0, 0, 0, 8, 2, 0, 0, 9 ] )
		self.TestSubtreeSizes( "C", [ 4, 2, 0, 0, 0, 6, 9, 0, 0, 0 ] )
		self.TestSubtreeSizes( "D", [ 4, 2, 0, 0, 0, 6, 8, 0, 9, 0 ] )
		self.TestSubtreeSizes( "E", [ 4, 2, 0, 0, 0, 6, 8, 9, 0, 0 ] )
		self.TestSubtreeSizes( "F", [ 8, 2, 0, 0, 9, 4, 2, 0, 0, 0 ] )
		self.TestSubtreeSizes( "G", [ 6, 9, 0, 0, 0, 4, 2, 0, 0, 0 ] )
		self.TestSubtreeSizes( "H", [ 6, 8, 0, 9, 0, 4, 2, 0, 0, 0 ] )
		self.TestSubtreeSizes( "I", [ 6, 8, 9, 0, 0, 4, 2, 0, 0, 0 ] )

	def test_subtree_maxpath_height(self):

		self.TestSubtreeMaxPathHeight( "A", 2 )
		self.TestSubtreeMaxPathHeight( "B", 0 )
		self.TestSubtreeMaxPathHeight( "C", 1 )
		self.TestSubtreeMaxPathHeight( "D", 0 )
		self.TestSubtreeMaxPathHeight( "E", 0 )
		self.TestSubtreeMaxPathHeight( "F", 0 )
		self.TestSubtreeMaxPathHeight( "G", 1 )
		self.TestSubtreeMaxPathHeight( "H", 0 )
		self.TestSubtreeMaxPathHeight( "I", 0 )

	def test_subtree_maxpath_heights(self):

		input = "((B,(D,E)C)A,F,(H,I)G)R;"

		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		heights = subtree_max_path_heights( tree )
		exp_heights = [ 3, 1, 0, 0, 0, 2, 1, 0, 0, 0 ]
		self.assertEqual(exp_heights,heights)

	def test_lca(self):

		self.TestTreeLCA( "A", "A", "A" )
		self.TestTreeLCA( "A", "B", "A" )
		self.TestTreeLCA( "A", "F", "R" )
		self.TestTreeLCA( "E", "C", "C" )
		self.TestTreeLCA( "E", "H", "R" )
		self.TestTreeLCA( "H", "I", "G" )

	def test_lcas(self):

		input = "((B,(D,E)C)A,F,(H,I)G)R;"
		tree = CommonTreeNewickReader().read( from_file('./test/src/tree/functions_py.txt'))
		#tree = CommonTreeNewickReader().read( from_string( input ))

		lcas = lowest_common_ancestors( tree )
		exp = Matrix_unsigned_long_t( 10, 10, [
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 1, 2, 1, 0, 0, 0, 0, 0, 0,
			0, 1, 1, 3, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 5, 5, 5, 5, 5,
			0, 0, 0, 0, 0, 5, 6, 6, 6, 5,
			0, 0, 0, 0, 0, 5, 6, 7, 6, 5,
			0, 0, 0, 0, 0, 5, 6, 6, 8, 5,
			0, 0, 0, 0, 0, 5, 5, 5, 5, 9
		])

		self.assertEqual(exp,lcas)

	def test_sign_matrix(self):

		infile = "test/data/tree/rooted.newick"
		tree = CommonTreeNewickReader().read( from_file( infile ))

		#Full sign matrix
		exf = Matrix_signed_char_t( 9, 9, [
			0, -1, -1, -1, +1, +1, +1, +1, +1,
			0,  0, -1, +1,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0, -1, -1, -1, +1,
			0,  0,  0,  0,  0,  0, -1, +1,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,  0
		])
		smf = sign_matrix( tree )
		self.assertEqual(exf,smf)

		#LOG_DBG << smf;
		#LOG_DBG;
		#LOG_DBG << exf;

		#Compressed sign matrix
		exc = Matrix_signed_char_t( 4, 5, [
			-1, -1, +1, +1, +1,
			-1, +1,  0,  0,  0,
			0,  0, -1, -1, +1,
			0,  0, -1, +1,  0
		])
		smc = sign_matrix( tree, True )
		self.assertEqual(exc,smc)

		#LOG_DBG << smc;
		#LOG_DBG;
		#LOG_DBG << exc;

if __name__ == '__main__':
	unittest.main()
