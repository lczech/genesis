import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class Functions(unittest.TestCase):

	def test_counts(self):

		reader = TaxonomyReader()
		reader.rank_field_position( 2 )

		tax = Taxonomy()
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.unordered"
		try:
			reader.read( from_file( infile ), tax )
		except Exception as e:
		        self.fail("reader.read( from_file( infile ), tax ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax) )
		self.assertTrue( validate(tax) )

		self.assertEqual( 16, taxa_count_lowest_levels( tax ) )

		self.assertEqual( 1, taxa_count_at_level( tax, 0 ) )
		self.assertEqual( 4, taxa_count_at_level( tax, 1 ) )
		self.assertEqual( 5, taxa_count_at_level( tax, 2 ) )
		self.assertEqual( 7, taxa_count_at_level( tax, 3 ) )
		self.assertEqual( 6, taxa_count_at_level( tax, 4 ) )
		self.assertEqual( 9, taxa_count_at_level( tax, 5 ) )
		self.assertEqual( 0, taxa_count_at_level( tax, 6 ) )

		level_count = taxa_count_levels( tax )
		level_count_ref = [ 1, 4, 5, 7, 6, 9 ]
		self.assertEqual( level_count_ref, level_count )

		self.assertEqual( 1, taxa_count_with_rank( tax, "Domain") )
		self.assertEqual( 4, taxa_count_with_rank( tax, "Phylum") )
		self.assertEqual( 5, taxa_count_with_rank( tax, "Class") )
		self.assertEqual( 7, taxa_count_with_rank( tax, "Order") )
		self.assertEqual( 6, taxa_count_with_rank( tax, "Family") )
		self.assertEqual( 9, taxa_count_with_rank( tax, "Genus") )
		self.assertEqual( 0, taxa_count_with_rank( tax, "Something") )

		rank_count = taxa_count_ranks( tax )
		rank_count_ref = {
		 "domain" : 1,
		 "phylum" : 4,
		 "class" :  5,
		 "order" :  7,
		 "family" : 6,
		 "genus" :  9
		}
		self.assertEqual( rank_count_ref, rank_count)

if __name__ == '__main__':
    unittest.main()
