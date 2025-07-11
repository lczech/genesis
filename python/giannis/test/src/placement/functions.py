import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *


class TestSampleFunctions(unittest.TestCase):

	def test_FilterPlacements(self):
		infile = "test/data/placement/duplicates_b.jplace"
		smp = JplaceReader().read( from_file(infile))

		# Check before filtering.
		self.assertEqual( 10, total_placement_count(smp) )

		# Filter everything below a threshold and check result.
		filter_min_weight_threshold( smp, 0.5 )
		self.assertEqual( 8, total_placement_count(smp) )

		# Re-read the file.
		smp = JplaceReader().read( from_file(infile))

		# Filter max number of placements and check result.
		filter_n_max_weight_placements( smp, 1 )
		self.assertEqual( 7, total_placement_count(smp) )

		# Re-read the file.
		smp = JplaceReader().read( from_file(infile))

		# Filter max number of placements and check result.
		filter_min_accumulated_weight( smp, 0.6 )
		self.assertEqual( 8, total_placement_count(smp) )

	def test_FilterPqueryNameRegex(self):
		infile = "test/data/placement/duplicates_b.jplace"
		smp = JplaceReader().read( from_file(infile))

		# Check before filtering.
		self.assertEqual( 10, total_placement_count(smp) )
		self.assertEqual( 11, total_name_count(smp) )

		# Keep list.
		filter_pqueries_keeping_names( smp, "[ac]" )
		self.assertEqual( 6, total_placement_count(smp) )
		self.assertEqual( 4, total_name_count(smp) )

		# Re-read the file.
		smp = JplaceReader().read( from_file(infile))

		# Remove list.
		filter_pqueries_removing_names( smp, "[ac]" )
		self.assertEqual( 9, total_placement_count(smp) )
		self.assertEqual( 7, total_name_count(smp) )

	def test_FilterPqueryNameLists(self):
		infile = "test/data/placement/duplicates_b.jplace"
		smp = JplaceReader().read( from_file(infile))

		# Check before filtering.
		self.assertEqual( 10, total_placement_count(smp) )
		self.assertEqual( 11, total_name_count(smp) )

		# Keep list.
		keep_list = {"a" , "c"}
		filter_pqueries_keeping_names( smp, keep_list )
		self.assertEqual( 6, total_placement_count(smp) )
		self.assertEqual( 4, total_name_count(smp) )

		# Re-read the file.
		smp = JplaceReader().read( from_file(infile))

		# Remove list.
		remove_list = {"a" , "c"}
		filter_pqueries_removing_names( smp, remove_list )
		self.assertEqual( 9, total_placement_count(smp) )
		self.assertEqual( 7, total_name_count(smp) )

	def test_FilterPqueryNameSets(self):
		infile_1 = "test/data/placement/duplicates_a.jplace"
		infile_2 = "test/data/placement/duplicates_b.jplace"
		sample_1 = JplaceReader().read( from_file( infile_1 ))
		sample_2 = JplaceReader().read( from_file( infile_2 ))

		# Checks before filtering.
		self.assertEqual(  8, total_placement_count( sample_1 ))
		self.assertEqual( 10, total_placement_count( sample_2 ))
		self.assertEqual(  7, total_name_count( sample_1 ))
		self.assertEqual( 11, total_name_count( sample_2 ))

		# Intersection.
		filter_pqueries_intersecting_names( sample_1, sample_2 )
		self.assertEqual(  8, total_placement_count( sample_1 ))
		self.assertEqual(  8, total_placement_count( sample_2 ))
		self.assertEqual(  7, total_name_count( sample_1 ))
		self.assertEqual(  8, total_name_count( sample_2 ))

		# Re-read the files.
		sample_3 = JplaceReader().read( from_file( infile_1 ))
		sample_4 = JplaceReader().read( from_file( infile_2 ))

		# Symmetric difference.
		filter_pqueries_differing_names( sample_3, sample_4 )
		self.assertEqual(  0, total_placement_count( sample_3 ))
		self.assertEqual(  4, total_placement_count( sample_4 ))
		self.assertEqual(  0, total_name_count( sample_3 ))
		self.assertEqual(  3, total_name_count( sample_4 ))

	def test_ConvertFromCommonTree(self):
		infile = "test/data/tree/distances.newick"
		def_tree = CommonTreeNewickReader().read( from_file( infile ))

		# Convert it to a tree that is usable for samples.
		place_tree = convert_common_tree_to_placement_tree( def_tree )

		# Check if the tree is correct.
		self.assertEqual( 13, place_tree.node_count() )
		self.assertTrue( has_correct_edge_nums( place_tree ))

	def test_LabelledTree(self):
		infile = "test/data/placement/test_c.jplace"
		sample = JplaceReader().read( from_file( infile ))

		# Pre-checks
		self.assertEqual(  7, sample.size() )
		self.assertEqual( 10, sample.tree().node_count() )
		self.assertEqual(  9, sample.tree().edge_count() )

		# Get and check multifurcating tree.
		lm_tree = labelled_tree( sample, False )
		self.assertEqual( 25, lm_tree.node_count() )
		self.assertEqual( 24, lm_tree.edge_count() )

		# Get and check fully resolved tree.
		lf_tree = labelled_tree( sample, True )
		self.assertEqual( 26, lf_tree.node_count() )
		self.assertEqual( 25, lf_tree.edge_count() )

if __name__ == '__main__':
    unittest.main()