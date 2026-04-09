import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *

class TestSample(unittest.TestCase):

	def test_WithTree(self):
		tree = PlacementTreeNewickReader().read( from_file('test/src/placement/sample_py.txt'))
		#tree = PlacementTreeNewickReader().read( from_string("((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};"))

		smp = Sample(tree)
		self.assertEqual(0, total_placement_count(smp))
		self.assertTrue(validate(smp, True, False))

	def func_test_sample_stats ( self, smp: Sample, expected_pquery_size: int, expected_placement_size: int, expected_name_size: int):
		self.assertTrue (validate(smp, True, False))

		self.assertEqual (expected_pquery_size,    smp.size())
		self.assertEqual (expected_placement_size, total_placement_count(smp))

		name_count = 0
		for pqry in smp.pqueries():
			name_count += pqry.name_size()

		self.assertEqual (expected_name_size, name_count)

	def test_MergeDuplicatesSimple(self):
		infile = "test/data/placement/duplicates_a.jplace"
		smp = JplaceReader().read( from_file(infile))

		# Check before merging.
		self.func_test_sample_stats(smp, 7, 8, 7)

		# Run the function of interest!
		merge_duplicates(smp)

		# Check after merging.
		self.func_test_sample_stats(smp, 3, 7, 3)

	def test_MergeDuplicatesTransitive(self):
		infile = "test/data/placement/duplicates_b.jplace"
		smp = JplaceReader().read( from_file(infile))

		# Check before merging.
		self.func_test_sample_stats(smp, 7, 10, 11)

		# Run the function of interest!
		merge_duplicates(smp)

		# Check after merging.
		self.func_test_sample_stats(smp, 1, 4, 4)

if __name__ == '__main__':
    unittest.main()