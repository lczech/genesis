import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class PathSetTest(unittest.TestCase):

	def TestPathSet( self, start_node_name, finish_node_name, lca_node_name, out_nodes ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"
		nodes = ""

		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/iterator/path_set_py.txt' ))

		start_node  = find_node( tree, start_node_name  )
		finish_node = find_node( tree, finish_node_name )
		lca_node    = find_node( tree, lca_node_name )
		self.assertNotEqual(None, start_node)
		self.assertNotEqual(None, finish_node)
		self.assertNotEqual(None, lca_node)

		for it in path_set( start_node, finish_node, lca_node ):
			nodes = nodes + it.node().data().name

		self.assertEqual(out_nodes,nodes)
		print(" with start node " + start_node_name + " and finish node " + finish_node_name)

	def test_path_set(self):

		self.TestPathSet( "A", "A", "A", "A" )
		self.TestPathSet( "A", "B", "A", "BA" )
		self.TestPathSet( "A", "C", "A", "CA" )
		self.TestPathSet( "A", "D", "A", "DCA" )
		self.TestPathSet( "A", "E", "A", "ECA" )
		self.TestPathSet( "A", "F", "R", "AFR" )
		self.TestPathSet( "A", "G", "R", "AGR" )
		self.TestPathSet( "A", "H", "R", "AHGR" )
		self.TestPathSet( "A", "I", "R", "AIGR" )
		self.TestPathSet( "A", "R", "R", "AR" )

		self.TestPathSet( "B", "A", "A", "BA" )
		self.TestPathSet( "B", "B", "B", "B" )
		self.TestPathSet( "B", "C", "A", "BCA" )
		self.TestPathSet( "B", "D", "A", "BDCA" )
		self.TestPathSet( "B", "E", "A", "BECA" )
		self.TestPathSet( "B", "F", "R", "BAFR" )
		self.TestPathSet( "B", "G", "R", "BAGR" )
		self.TestPathSet( "B", "H", "R", "BAHGR" )
		self.TestPathSet( "B", "I", "R", "BAIGR" )
		self.TestPathSet( "B", "R", "R", "BAR" )

		self.TestPathSet( "C", "A", "A", "CA" )
		self.TestPathSet( "C", "B", "A", "CBA" )
		self.TestPathSet( "C", "C", "C", "C" )
		self.TestPathSet( "C", "D", "C", "DC" )
		self.TestPathSet( "C", "E", "C", "EC" )
		self.TestPathSet( "C", "F", "R", "CAFR" )
		self.TestPathSet( "C", "G", "R", "CAGR" )
		self.TestPathSet( "C", "H", "R", "CAHGR" )
		self.TestPathSet( "C", "I", "R", "CAIGR" )
		self.TestPathSet( "C", "R", "R", "CAR" )

		self.TestPathSet( "D", "A", "A", "DCA" )
		self.TestPathSet( "D", "B", "A", "DCBA" )
		self.TestPathSet( "D", "C", "C", "DC" )
		self.TestPathSet( "D", "D", "D", "D" )
		self.TestPathSet( "D", "E", "C", "DEC" )
		self.TestPathSet( "D", "F", "R", "DCAFR" )
		self.TestPathSet( "D", "G", "R", "DCAGR" )
		self.TestPathSet( "D", "H", "R", "DCAHGR" )
		self.TestPathSet( "D", "I", "R", "DCAIGR" )
		self.TestPathSet( "D", "R", "R", "DCAR" )

		self.TestPathSet( "E", "A", "A", "ECA" )
		self.TestPathSet( "E", "B", "A", "ECBA" )
		self.TestPathSet( "E", "C", "C", "EC" )
		self.TestPathSet( "E", "D", "C", "EDC" )
		self.TestPathSet( "E", "E", "E", "E" )
		self.TestPathSet( "E", "F", "R", "ECAFR" )
		self.TestPathSet( "E", "G", "R", "ECAGR" )
		self.TestPathSet( "E", "H", "R", "ECAHGR" )
		self.TestPathSet( "E", "I", "R", "ECAIGR" )
		self.TestPathSet( "E", "R", "R", "ECAR" )

		self.TestPathSet( "F", "A", "R", "FAR" )
		self.TestPathSet( "F", "B", "R", "FBAR" )
		self.TestPathSet( "F", "C", "R", "FCAR" )
		self.TestPathSet( "F", "D", "R", "FDCAR" )
		self.TestPathSet( "F", "E", "R", "FECAR" )
		self.TestPathSet( "F", "F", "F", "F" )
		self.TestPathSet( "F", "G", "R", "FGR" )
		self.TestPathSet( "F", "H", "R", "FHGR" )
		self.TestPathSet( "F", "I", "R", "FIGR" )
		self.TestPathSet( "F", "R", "R", "FR" )

		self.TestPathSet( "G", "A", "R", "GAR" )
		self.TestPathSet( "G", "B", "R", "GBAR" )
		self.TestPathSet( "G", "C", "R", "GCAR" )
		self.TestPathSet( "G", "D", "R", "GDCAR" )
		self.TestPathSet( "G", "E", "R", "GECAR" )
		self.TestPathSet( "G", "F", "R", "GFR" )
		self.TestPathSet( "G", "G", "G", "G" )
		self.TestPathSet( "G", "H", "G", "HG" )
		self.TestPathSet( "G", "I", "G", "IG" )
		self.TestPathSet( "G", "R", "R", "GR" )

		self.TestPathSet( "H", "A", "R", "HGAR" )
		self.TestPathSet( "H", "B", "R", "HGBAR" )
		self.TestPathSet( "H", "C", "R", "HGCAR" )
		self.TestPathSet( "H", "D", "R", "HGDCAR" )
		self.TestPathSet( "H", "E", "R", "HGECAR" )
		self.TestPathSet( "H", "F", "R", "HGFR" )
		self.TestPathSet( "H", "G", "G", "HG" )
		self.TestPathSet( "H", "H", "H", "H" )
		self.TestPathSet( "H", "I", "G", "HIG" )
		self.TestPathSet( "H", "R", "R", "HGR" )

		self.TestPathSet( "I", "A", "R", "IGAR" )
		self.TestPathSet( "I", "B", "R", "IGBAR" )
		self.TestPathSet( "I", "C", "R", "IGCAR" )
		self.TestPathSet( "I", "D", "R", "IGDCAR" )
		self.TestPathSet( "I", "E", "R", "IGECAR" )
		self.TestPathSet( "I", "F", "R", "IGFR" )
		self.TestPathSet( "I", "G", "G", "IG" )
		self.TestPathSet( "I", "H", "G", "IHG" )
		self.TestPathSet( "I", "I", "I", "I" )
		self.TestPathSet( "I", "R", "R", "IGR" )

		self.TestPathSet( "R", "A", "R", "AR" )
		self.TestPathSet( "R", "B", "R", "BAR" )
		self.TestPathSet( "R", "C", "R", "CAR" )
		self.TestPathSet( "R", "D", "R", "DCAR" )
		self.TestPathSet( "R", "E", "R", "ECAR" )
		self.TestPathSet( "R", "F", "R", "FR" )
		self.TestPathSet( "R", "G", "R", "GR" )
		self.TestPathSet( "R", "H", "R", "HGR" )
		self.TestPathSet( "R", "I", "R", "IGR" )
		self.TestPathSet( "R", "R", "R", "R" )

	def test_path_set_exceptions(self):

		self.assertRaises( RuntimeError, self.TestPathSet , "A", "A", "G", "" )
		self.assertRaises( RuntimeError, self.TestPathSet , "A", "B", "B", "" )
		self.assertRaises( RuntimeError, self.TestPathSet , "A", "B", "G", "" )
		self.assertRaises( RuntimeError, self.TestPathSet , "G", "R", "G", "" )
		self.assertRaises( RuntimeError, self.TestPathSet , "H", "I", "I", "" )

if __name__ == '__main__':
	unittest.main()
