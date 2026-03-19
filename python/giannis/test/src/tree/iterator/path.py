import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class PathTest(unittest.TestCase):

	def TestPath( self, start_node_name, finish_node_name, out_nodes ):
		input = "((B,(D,E)C)A,F,(H,I)G)R;"
		nodes = ""

		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/iterator/path_py.txt' ))

		start_node  = find_node( tree, start_node_name  )
		finish_node = find_node( tree, finish_node_name )
		self.assertNotEqual(None,start_node)
		self.assertNotEqual(None,finish_node)

		for it in path( start_node, finish_node ):
			nodes = nodes + it.node().data().name;
			#print(dir(it.node().data()))
			#exit()

		self.assertEqual(out_nodes,nodes)
		print(" with start node " + start_node_name + " and finish node " + finish_node_name)

	def test_path(self):

		self.TestPath( "A", "A", "A" )
		self.TestPath( "A", "B", "AB" )
		self.TestPath( "A", "C", "AC" )
		self.TestPath( "A", "D", "ACD" )
		self.TestPath( "A", "E", "ACE" )
		self.TestPath( "A", "F", "ARF" )
		self.TestPath( "A", "G", "ARG" )
		self.TestPath( "A", "H", "ARGH" )
		self.TestPath( "A", "I", "ARGI" )
		self.TestPath( "A", "R", "AR" )

		self.TestPath( "B", "A", "BA" )
		self.TestPath( "B", "B", "B" )
		self.TestPath( "B", "C", "BAC" )
		self.TestPath( "B", "D", "BACD" )
		self.TestPath( "B", "E", "BACE" )
		self.TestPath( "B", "F", "BARF" )
		self.TestPath( "B", "G", "BARG" )
		self.TestPath( "B", "H", "BARGH" )
		self.TestPath( "B", "I", "BARGI" )
		self.TestPath( "B", "R", "BAR" )

		self.TestPath( "C", "A", "CA" )
		self.TestPath( "C", "B", "CAB" )
		self.TestPath( "C", "C", "C" )
		self.TestPath( "C", "D", "CD" )
		self.TestPath( "C", "E", "CE" )
		self.TestPath( "C", "F", "CARF" )
		self.TestPath( "C", "G", "CARG" )
		self.TestPath( "C", "H", "CARGH" )
		self.TestPath( "C", "I", "CARGI" )
		self.TestPath( "C", "R", "CAR" )

		self.TestPath( "D", "A", "DCA" )
		self.TestPath( "D", "B", "DCAB" )
		self.TestPath( "D", "C", "DC" )
		self.TestPath( "D", "D", "D" )
		self.TestPath( "D", "E", "DCE" )
		self.TestPath( "D", "F", "DCARF" )
		self.TestPath( "D", "G", "DCARG" )
		self.TestPath( "D", "H", "DCARGH" )
		self.TestPath( "D", "I", "DCARGI" )
		self.TestPath( "D", "R", "DCAR" )

		self.TestPath( "E", "A", "ECA" )
		self.TestPath( "E", "B", "ECAB" )
		self.TestPath( "E", "C", "EC" )
		self.TestPath( "E", "D", "ECD" )
		self.TestPath( "E", "E", "E" )
		self.TestPath( "E", "F", "ECARF" )
		self.TestPath( "E", "G", "ECARG" )
		self.TestPath( "E", "H", "ECARGH" )
		self.TestPath( "E", "I", "ECARGI" )
		self.TestPath( "E", "R", "ECAR" )

		self.TestPath( "F", "A", "FRA" )
		self.TestPath( "F", "B", "FRAB" )
		self.TestPath( "F", "C", "FRAC" )
		self.TestPath( "F", "D", "FRACD" )
		self.TestPath( "F", "E", "FRACE" )
		self.TestPath( "F", "F", "F" )
		self.TestPath( "F", "G", "FRG" )
		self.TestPath( "F", "H", "FRGH" )
		self.TestPath( "F", "I", "FRGI" )
		self.TestPath( "F", "R", "FR" )

		self.TestPath( "G", "A", "GRA" )
		self.TestPath( "G", "B", "GRAB" )
		self.TestPath( "G", "C", "GRAC" )
		self.TestPath( "G", "D", "GRACD" )
		self.TestPath( "G", "E", "GRACE" )
		self.TestPath( "G", "F", "GRF" )
		self.TestPath( "G", "G", "G" )
		self.TestPath( "G", "H", "GH" )
		self.TestPath( "G", "I", "GI" )
		self.TestPath( "G", "R", "GR" )

		self.TestPath( "H", "A", "HGRA" )
		self.TestPath( "H", "B", "HGRAB" )
		self.TestPath( "H", "C", "HGRAC" )
		self.TestPath( "H", "D", "HGRACD" )
		self.TestPath( "H", "E", "HGRACE" )
		self.TestPath( "H", "F", "HGRF" )
		self.TestPath( "H", "G", "HG" )
		self.TestPath( "H", "H", "H" )
		self.TestPath( "H", "I", "HGI" )
		self.TestPath( "H", "R", "HGR" )

		self.TestPath( "I", "A", "IGRA" )
		self.TestPath( "I", "B", "IGRAB" )
		self.TestPath( "I", "C", "IGRAC" )
		self.TestPath( "I", "D", "IGRACD" )
		self.TestPath( "I", "E", "IGRACE" )
		self.TestPath( "I", "F", "IGRF" )
		self.TestPath( "I", "G", "IG" )
		self.TestPath( "I", "H", "IGH" )
		self.TestPath( "I", "I", "I" )
		self.TestPath( "I", "R", "IGR" )

		self.TestPath( "R", "A", "RA" )
		self.TestPath( "R", "B", "RAB" )
		self.TestPath( "R", "C", "RAC" )
		self.TestPath( "R", "D", "RACD" )
		self.TestPath( "R", "E", "RACE" )
		self.TestPath( "R", "F", "RF" )
		self.TestPath( "R", "G", "RG" )
		self.TestPath( "R", "H", "RGH" )
		self.TestPath( "R", "I", "RGI" )
		self.TestPath( "R", "R", "R" )

if __name__ == '__main__':
	unittest.main()
