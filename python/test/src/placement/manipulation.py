import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *


class TestPlacementManipulation(unittest.TestCase):

	def func_TestPlacementReroot(self, reroot_edge_id: int, check_string: str, proximal_lengths: list):
		# Input data.
		infile = "./test/data/placement/unrooted.jplace"
		sample = JplaceReader().read( from_file( infile ))
		tree = sample.tree()
		self.assertTrue( has_correct_edge_nums( tree ) )

		edge_map = edge_num_to_edge_map( sample )
		#print(type(edge_map))
		"""
		edge_ptr = edge_map[ reroot_edge_id ]
		self.assertNotEqual( None, edge_ptr )

		# reroot
		make_rooted( sample, edge_ptr )
		self.assertTrue( validate_topology(sample.tree()) )
		self.assertTrue( has_correct_edge_nums( sample.tree()) )

		result = "";
		for pq in sample:
			result += pq.name_at(0).name
			for p in pq.placements():
				result += str( p.edge_num() )
			result += " "

		# Check if the edge_nums reordering outcome is correct.
		self.assertEqual( check_string, trim( result ) ) # << " with start edge " << reroot_edge_id;

		# check if proximal lengths were adjusted correctly
		i = 0;
		for pq in sample:
			for p in pq.placements():
				self.assertEqual( proximal_lengths[i], p.proximal_length ) # << " with placement " << pq.name_at(0).name << " and reroot edge " << reroot_edge_id;
				++i;
		"""

	def test_Reroot(self):
		self.func_TestPlacementReroot( 0, "X759 Y10 Z4", [0.3, 0.6, 0.9, 0.1, 0.7, 1.0] )
		self.func_TestPlacementReroot( 1, "X536 Y09 Z7", [0.3, 0.6, 0.9, 0.1, 0.7, 0.0] )
		self.func_TestPlacementReroot( 2, "X314 Y96 Z5", [0.3, 0.6, 0.9, 0.1, 0.7, 0.0] )
		self.func_TestPlacementReroot( 3, "X203 Y75 Z4", [0.3, 0.6, 0.9, 0.1, 0.7, 0.0] )
		self.func_TestPlacementReroot( 4, "X203 Y75 Z4", [0.3, 0.6, 0.9, 0.1, 0.7, 0.0] )
		self.func_TestPlacementReroot( 5, "X203 Y65 Z9", [0.3, 0.6, 0.9, 0.1, 0.7, 1.0] )
		self.func_TestPlacementReroot( 6, "X791 Y32 Z6", [0.7, 0.6, 0.9, 0.1, 0.7, 1.0] )
		self.func_TestPlacementReroot( 7, "X670 Y21 Z5", [0.7, 0.6, 0.9, 0.1, 0.7, 1.0] )
		self.func_TestPlacementReroot( 8, "X970 Y21 Z5", [0.3, 0.6, 0.9, 0.1, 0.7, 1.0] )

if __name__ == '__main__':
    unittest.main()
