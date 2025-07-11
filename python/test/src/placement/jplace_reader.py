import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.placement import *

class TestJplaceReader(unittest.TestCase):

	def test_from_file(self):
		infile = "test/data/placement/test_a.jplace"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 5, total_placement_count(smp))
		self.assertEqual( 5, total_name_count(smp))
		self.assertTrue( validate(smp, True, False))
		self.assertTrue( has_correct_edge_nums(smp.tree()))

	def test_from_file_gzip(self):
		infile = "test/data/placement/test_a.jplace.gz"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 5, total_placement_count(smp))
		self.assertEqual( 5, total_name_count(smp))
		self.assertTrue( validate(smp, True, False))
		self.assertTrue( has_correct_edge_nums(smp.tree()))

	def test_from_files(self):
		indir = "test/data/placement/"
		infiles = [ indir + "test_a.jplace", indir + "test_b.jplace", indir + "test_c.jplace" ]

		smps = JplaceReader().read( from_files( infiles ))
		self.assertEqual( 3, smps.size())
		self.assertEqual( 5, total_placement_count(smps[0]))
		self.assertEqual( 8, total_name_count(smps[2]))
		self.assertEqual( "test_b", smps.name_at(1))

	def test_from_files_gzip(self):
		indir = "test/data/placement/"
		infiles = [ indir + "test_a.jplace.gz", indir + "test_b.jplace.gz", indir + "test_c.jplace.gz" ]

		smps = JplaceReader().read( from_files( infiles ))
		self.assertEqual( 3, smps.size())
		self.assertEqual( 5, total_placement_count(smps[0]))
		self.assertEqual( 8, total_name_count(smps[2]))
		self.assertEqual( "test_b", smps.name_at(1))

	def test_version1(self):
		infile = "test/data/placement/version_1.jplace"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 9, total_placement_count(smp))
		self.assertEqual( 8, total_name_count(smp))
		self.assertTrue( validate( smp, True, False))
		self.assertTrue( has_correct_edge_nums(smp.tree()))

	def test_version2(self):
		infile = "test/data/placement/version_2.jplace"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 9, total_placement_count(smp))
		self.assertEqual( 8, total_name_count(smp))
		self.assertTrue( validate( smp, True, False))
		self.assertTrue( has_correct_edge_nums(smp.tree()))

	def test_version3(self):
		infile = "test/data/placement/version_3.jplace"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 9, total_placement_count(smp))
		self.assertEqual( 8, total_name_count(smp))
		self.assertTrue( validate( smp, True, False))
		self.assertTrue( has_correct_edge_nums(smp.tree()))

if __name__ == '__main__':
    unittest.main()