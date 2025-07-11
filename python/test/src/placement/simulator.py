import sys
import os
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *

class TestPlacementSimulator(unittest.TestCase):

	def test_Simple(self):
		#tree = PlacementTreeNewickReader().read( from_string( "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};" ))
		tree = PlacementTreeNewickReader().read( from_file( './test/src/placement/simulator_py.txt' ))

		smp = Sample(tree)
		self.assertEqual(0, total_placement_count(smp))
		self.assertTrue(validate(smp, True, False))

		sim  = Simulator()

		n = 100
		sim.generate(smp, n)
		self.assertEqual(n, total_placement_count(smp))
		self.assertTrue(validate(smp, True, False))

	def test_LeavesOnly(self):
		#tree = PlacementTreeNewickReader().read( from_string("((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};"))
		tree = PlacementTreeNewickReader().read( from_file( './test/src/placement/simulator_py.txt' ))

		smp = Sample(tree)
		sim = Simulator()

		# Set weight so that onlye edges adjacet to leaves are populated.
		edge_weights = [ 1.0 ]
		set_depths_distributed_weights( smp, edge_weights, sim.edge_distribution() )

		# Generate placements.
		n = 100
		sim.generate(smp, n)
		self.assertEqual(n, total_placement_count(smp))
		self.assertTrue(validate(smp, True, False))

		# Check whether all placements are next to leaf nodes.
		for  pqry in smp.pqueries():
			edge = pqry.placement_at(0).edge()
		self.assertTrue( is_leaf( edge.primary_node() ) or is_leaf( edge.secondary_node() ))

	def test_Learning(self):
		infile  = "test/data/placement/test_a.jplace"

		# Read the Jplace file into a Sample object.
		sample = JplaceReader().read( from_file( infile ))

		# Learn simulation parameters.
		sim = Simulator()
		learn_per_edge_weights( sample, sim.edge_distribution() )
		learn_placement_number_weights( sample, sim.extra_placement_distribution() )
		learn_placement_path_length_weights( sample, sim.extra_placement_distribution() )
		learn_like_weight_ratio_distribution( sample, sim.like_weight_ratio_distribution(), 25 )

		# Check edge weight sum.
		sum_edge_weights = 0.0
		for i in sim.edge_distribution().edge_weights:
			sum_edge_weights += i
		self.assertEqual( total_placement_mass_with_multiplicities( sample ), sum_edge_weights )

		placement_number_weights = sim.extra_placement_distribution().placement_number_weights

		# Check extra placement weight sum.
		sum_extra_placement_weights = 0
		for i in placement_number_weights:
			sum_extra_placement_weights += i
		self.assertEqual( sample.size(), sum_extra_placement_weights )

		# Check sum of path lengths weights.
		# This check is a bit more complicatted. We accumulate the weights that denote the distribution
		# of how long the path between two placements of a pquery is. This sum is stored in
		# `sum_path_length_weights`. Furthermore, as the learning function used to get those weights
		# leaves out the paths of length 0 (they are not needed in the simulation), we also add this
		# number, which is the value of total_placement_count(). This is because every placement has
		# a path length of 0 to itself.
		# The resulting value is then compared to the expected value, which is calculated as follows.
		# We expect as many weights as there are combinations of placements within all pqueries.
		# Those combinations come from a pairwise comparison of all placments of a pquery, thus it is
		# quadratic (the i*i component) in the number of placements per pquery (we also need to add 1
		# to the i's, as the numbers actually mean "extra" placements). This is multiplied by the
		# number of pqueries that have `i` many placements.
		# In other words: There are x = placement_number_weights[ i ] many pqueries that have (i+1)
		# many placements. Take this x times the number of combinations of those placements, which is
		# (i+1)*(i+1), and add this up for all pqueries, to get the total expected number of paths
		# between all placements of the pqueries.
		possible_placement_combinations = 0.0
		for i in range(0, len(placement_number_weights)):
			possible_placement_combinations += (i+1) * (i+1) * placement_number_weights[ i ]

		sum_path_length_weights = 0.0
		for i in sim.extra_placement_distribution().placement_path_length_weights:
			sum_path_length_weights += i
		sum_path_length_weights += total_placement_count( sample )
		self.assertEqual( possible_placement_combinations, sum_path_length_weights )

		# Check lwr weight sum.
		sum_lwr_weights = 0
		for i in sim.like_weight_ratio_distribution().weights:
			sum_lwr_weights += i
		self.assertEqual( total_placement_count( sample ), sum_lwr_weights )

if __name__ == '__main__':
	unittest.main()