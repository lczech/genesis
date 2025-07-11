import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.placement import *

class TestSampleMeasures(unittest.TestCase):

	def test_EDPL(self):
		infile = "test/data/placement/duplicates_b.jplace"

		smpl = JplaceReader().read( from_file( infile ))

		edpl_vec = edpl( smpl )
		expect = [ 1.5, 0, 1.218, 0, 0, 1.2, 0 ]
		# EXPECT_EQ( expect, edpl_vec )

		self.assertEqual( len(expect), len(edpl_vec) )
		for i in range(0,len(expect)):
			self.assertAlmostEqual( expect[i], edpl_vec[i] )

	def test_EarthMoversDistance(self):
		infile_lhs = "test/data/placement/test_a.jplace"
		infile_rhs = "test/data/placement/test_b.jplace"

		# Read files.
		smp_lhs = JplaceReader().read( from_file( infile_lhs ))
		smp_rhs = JplaceReader().read( from_file( infile_rhs ))

		# Distances and symmetric cases.
		self.assertAlmostEqual( 2.8458333, earth_movers_distance( smp_lhs, smp_rhs, 1.0, False ))
		self.assertAlmostEqual( 2.8458333, earth_movers_distance( smp_rhs, smp_lhs, 1.0, False ))
		self.assertAlmostEqual( 3.4916666, earth_movers_distance( smp_lhs, smp_rhs, 1.0, True  ), places = 6)
		self.assertAlmostEqual( 3.4916666, earth_movers_distance( smp_rhs, smp_lhs, 1.0, True  ), places = 6)

		# Self-distances.
		self.assertAlmostEqual( 0.0, earth_movers_distance( smp_lhs, smp_lhs, 1.0, False ))
		self.assertAlmostEqual( 0.0, earth_movers_distance( smp_rhs, smp_rhs, 1.0, False ))
		self.assertAlmostEqual( 0.52499998, earth_movers_distance( smp_lhs, smp_lhs, 1.0, True ))
		self.assertAlmostEqual( 0.76666665, earth_movers_distance( smp_rhs, smp_rhs, 1.0, True ))

		# Set-version of the EMD.
		set = SampleSet()
		set.add( smp_lhs )
		set.add( smp_rhs )

		set_emd_o =  earth_movers_distance( set, 1.0, False )
		set_emd_p =  earth_movers_distance( set, 1.0, True )

		self.assertAlmostEqual( 0.0,       set_emd_o( 0, 0 ) )
		self.assertAlmostEqual( 2.8458333, set_emd_o( 0, 1 ) )
		self.assertAlmostEqual( 2.8458333, set_emd_o( 1, 0 ) )
		self.assertAlmostEqual( 0.0,       set_emd_o( 1, 1 ) )

		self.assertAlmostEqual( 0.52499998, set_emd_p( 0, 0 ) )
		self.assertAlmostEqual( 3.4916666,  set_emd_p( 0, 1 ), places = 6 )
		self.assertAlmostEqual( 3.4916666,  set_emd_p( 1, 0 ), places = 6 )
		self.assertAlmostEqual( 0.7666666,  set_emd_p( 1, 1 ), places = 6 )

	def test_NodeHistogramDistance(self):
		infile_lhs = "test/data/placement/test_a.jplace"
		infile_rhs = "test/data/placement/test_b.jplace"

		# Read files.
		smp_lhs = JplaceReader().read( from_file(infile_lhs))
		smp_rhs = JplaceReader().read( from_file(infile_rhs))

		# Distance and symmetric case.
		self.assertAlmostEqual( 1.9533334, node_histogram_distance( smp_lhs, smp_rhs, 10 ), places = 6)
		self.assertAlmostEqual( 1.9533334, node_histogram_distance( smp_rhs, smp_lhs, 10 ), places = 6)

		# Self-distances.
		self.assertAlmostEqual( 0.0, node_histogram_distance( smp_lhs, smp_lhs ))
		self.assertAlmostEqual( 0.0, node_histogram_distance( smp_rhs, smp_rhs ))

		# Use SampleSet functions.
		set = SampleSet()
		set.add( smp_lhs )
		set.add( smp_rhs )

		nhd_mat = node_histogram_distance( set, 10 )
		self.assertEqual( 4, nhd_mat.size() )

		# Check matrix with negative histogram axis.
		self.assertAlmostEqual( 0.0,    nhd_mat( 0, 0 ))
		self.assertAlmostEqual( 1.9533334, nhd_mat( 1, 0 ), places = 6)
		self.assertAlmostEqual( 1.9533334, nhd_mat( 0, 1 ), places = 6)
		self.assertAlmostEqual( 0.0,    nhd_mat( 1, 1 ))

if __name__ == '__main__':
	unittest.main()