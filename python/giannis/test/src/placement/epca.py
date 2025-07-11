import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *


class TestSampleMeasures(unittest.TestCase):

	def test_ImbalanceVector(self):
		# Read sample
		infile = "test/data/placement/test_a.jplace";
		smp = JplaceReader().read( from_file( infile ))

		# Get imbalance and weights per edge.
		imbalance_vec   = epca_imbalance_vector( smp, False )
		edge_weight_vec = placement_mass_per_edges_with_multiplicities( smp )
		self.assertEqual( len(imbalance_vec), len(edge_weight_vec) )

		# Get indicator which edge is on which side.
		edge_side_mat = edge_sides( smp.tree() )

		# Combine weights to get imbalance.
		sz = len(edge_weight_vec)
		combined = [0.0] * len(edge_weight_vec)
		for r in range(0,sz):
			for c in range(0,sz):
				combined[ r ] += edge_side_mat( r, c ) * edge_weight_vec[ c ]

		combined2 = matrix_multiplication( edge_side_mat, edge_weight_vec )

		self.assertEqual( imbalance_vec, combined )
		self.assertEqual( combined2, combined )

if __name__ == '__main__':
    unittest.main()
