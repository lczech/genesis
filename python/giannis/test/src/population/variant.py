import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.population import *

class VariantTest(unittest.TestCase):

	def test_ConvertFromVcfRecordPool(self):

		infile = "test/data/population/example_ad.vcf"

		# Load the VCF data and prepare the record
		file = HtsFile( infile )
		header = VcfHeader( file )
		record = VcfRecord( header )

		# Read all lines/records
		samples = []
		while( record.read_next( file )):
			# Typically, we want to ensure record.is_snp() as well (see commented condition below).
			# Here however, we also do a test that allows for "." deletions in the VCF file.
			# That is a bit uncommon, but works for this test. In normal applications, use the
			# condition below instead.
			if not record.has_format( "AD" ):
				continue
			# if( ! record.is_snp() || ! record.has_format( "AD" )) {
			#     continue;
			# }
			samples.append( convert_to_variant_as_pool( record ))
		self.assertEqual( 4, len(samples) )

		# Line 1

		self.assertEqual( "20",  samples[0].chromosome )
		self.assertEqual( 14370, samples[0].position )
		self.assertEqual( 'G',   samples[0].reference_base )
		self.assertEqual( 3,     len(samples[0].samples) )

		self.assertEqual( 8, samples[0].samples[0].a_count )
		self.assertEqual( 0, samples[0].samples[0].c_count )
		self.assertEqual( 5, samples[0].samples[0].g_count )
		self.assertEqual( 0, samples[0].samples[0].t_count )
		self.assertEqual( 0, samples[0].samples[0].n_count )
		self.assertEqual( 0, samples[0].samples[0].d_count )

		self.assertEqual( 5, samples[0].samples[1].a_count )
		self.assertEqual( 0, samples[0].samples[1].c_count )
		self.assertEqual( 8, samples[0].samples[1].g_count )
		self.assertEqual( 0, samples[0].samples[1].t_count )
		self.assertEqual( 0, samples[0].samples[1].n_count )
		self.assertEqual( 0, samples[0].samples[1].d_count )

		self.assertEqual( 0, samples[0].samples[2].a_count )
		self.assertEqual( 0, samples[0].samples[2].c_count )
		self.assertEqual( 0, samples[0].samples[2].g_count )
		self.assertEqual( 0, samples[0].samples[2].t_count )
		self.assertEqual( 0, samples[0].samples[2].n_count )
		self.assertEqual( 0, samples[0].samples[2].d_count )

		# Line 2

		self.assertEqual( "20",  samples[1].chromosome )
		self.assertEqual( 17330, samples[1].position )
		self.assertEqual( 'T',   samples[1].reference_base )
		self.assertEqual( 3,     len(samples[1].samples) )

		self.assertEqual( 2, samples[1].samples[0].a_count )
		self.assertEqual( 0, samples[1].samples[0].c_count )
		self.assertEqual( 0, samples[1].samples[0].g_count )
		self.assertEqual( 1, samples[1].samples[0].t_count )
		self.assertEqual( 0, samples[1].samples[0].n_count )
		self.assertEqual( 0, samples[1].samples[0].d_count )

		self.assertEqual( 4, samples[1].samples[1].a_count )
		self.assertEqual( 0, samples[1].samples[1].c_count )
		self.assertEqual( 0, samples[1].samples[1].g_count )
		self.assertEqual( 3, samples[1].samples[1].t_count )
		self.assertEqual( 0, samples[1].samples[1].n_count )
		self.assertEqual( 0, samples[1].samples[1].d_count )

		self.assertEqual( 6, samples[1].samples[2].a_count )
		self.assertEqual( 0, samples[1].samples[2].c_count )
		self.assertEqual( 0, samples[1].samples[2].g_count )
		self.assertEqual( 5, samples[1].samples[2].t_count )
		self.assertEqual( 0, samples[1].samples[2].n_count )
		self.assertEqual( 0, samples[1].samples[2].d_count )

		# Line 3

		self.assertEqual( "20",    samples[2].chromosome )
		self.assertEqual( 1110696, samples[2].position )
		self.assertEqual( 'A',     samples[2].reference_base )
		self.assertEqual( 3,       len(samples[2].samples) )

		self.assertEqual( 0, samples[2].samples[0].a_count )
		self.assertEqual( 0, samples[2].samples[0].c_count )
		self.assertEqual( 1, samples[2].samples[0].g_count )
		self.assertEqual( 2, samples[2].samples[0].t_count )
		self.assertEqual( 0, samples[2].samples[0].n_count )
		self.assertEqual( 0, samples[2].samples[0].d_count )

		self.assertEqual( 3, samples[2].samples[1].a_count )
		self.assertEqual( 0, samples[2].samples[1].c_count )
		self.assertEqual( 4, samples[2].samples[1].g_count )
		self.assertEqual( 5, samples[2].samples[1].t_count )
		self.assertEqual( 0, samples[2].samples[1].n_count )
		self.assertEqual( 0, samples[2].samples[1].d_count )

		self.assertEqual( 6, samples[2].samples[2].a_count )
		self.assertEqual( 0, samples[2].samples[2].c_count )
		self.assertEqual( 7, samples[2].samples[2].g_count )
		self.assertEqual( 8, samples[2].samples[2].t_count )
		self.assertEqual( 0, samples[2].samples[2].n_count )
		self.assertEqual( 0, samples[2].samples[2].d_count )

		# Line 4

		self.assertEqual( "20",    samples[3].chromosome )
		self.assertEqual( 1230237, samples[3].position )
		self.assertEqual( 'T',     samples[3].reference_base )
		self.assertEqual( 3,       (samples[3].samples) )

		self.assertEqual( 0, samples[3].samples[0].a_count )
		self.assertEqual( 0, samples[3].samples[0].c_count )
		self.assertEqual( 0, samples[3].samples[0].g_count )
		self.assertEqual( 5, samples[3].samples[0].t_count )
		self.assertEqual( 0, samples[3].samples[0].n_count )
		self.assertEqual( 4, samples[3].samples[0].d_count )

		self.assertEqual( 0, samples[3].samples[1].a_count )
		self.assertEqual( 0, samples[3].samples[1].c_count )
		self.assertEqual( 0, samples[3].samples[1].g_count )
		self.assertEqual( 3, samples[3].samples[1].t_count )
		self.assertEqual( 0, samples[3].samples[1].n_count )
		self.assertEqual( 2, samples[3].samples[1].d_count )

		self.assertEqual( 0, samples[3].samples[2].a_count )
		self.assertEqual( 0, samples[3].samples[2].c_count )
		self.assertEqual( 0, samples[3].samples[2].g_count )
		self.assertEqual( 1, samples[3].samples[2].t_count )
		self.assertEqual( 0, samples[3].samples[2].n_count )
		self.assertEqual( 0, samples[3].samples[2].d_count )

if __name__ == '__main__':
	unittest.main()
