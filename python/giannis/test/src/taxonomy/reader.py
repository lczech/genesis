import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class Reader(unittest.TestCase):

	def test_reader_order(self):

		reader = TaxonomyReader()

		tax_0 = Taxonomy()
		reader.expect_strict_order( True )
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.ordered"
		try:
			reader.read( from_file( infile ), tax_0 )
		except:
		        self.fail("reader.read( from_file( infile ), tax_0 ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax_0))
		self.assertTrue( tax_0 )
		sort_by_name( tax_0 )
		self.assertTrue( tax_0 )

		tax_1 = Taxonomy()
		reader.expect_strict_order( False )
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.ordered"
		try:
			reader.read( from_file( infile ), tax_1 )
		except:
		        self.fail("reader.read( from_file( infile ), tax_1 ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax_1) )
		self.assertTrue( tax_1 )
		sort_by_name( tax_1 )
		self.assertTrue( tax_1 )

		tax_2 = Taxonomy()
		reader.expect_strict_order( True )
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.unordered"
		with self.assertRaises(RuntimeError):
			reader.read( from_file( infile ), tax_2 )
		self.assertEqual( 0 , total_taxa_count(tax_2) )
		self.assertTrue( validate(tax_2) )

		tax_3 = Taxonomy()
		reader.expect_strict_order( False )
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.unordered"
		try:
			reader.read( from_file( infile ), tax_3 )
		except:
			self.fail("reader.read( from_file( infile ), tax_3 ) ERROR")
		self.assertEqual( 32 , total_taxa_count(tax_3) )
		self.assertTrue( validate(tax_3) )
		sort_by_name( tax_3 )
		self.assertTrue( validate(tax_3) )

	def test_reader_trimming(self):

		infile = "test/data/taxonomy/tax_slv_ssu_123.1.unordered";

		tax = Taxonomy()
		reader = TaxonomyReader()
		reader.taxopath_parser().trim_whitespaces( True )
		try:
			reader.read( from_file( infile ), tax )
		except:
			self.fail("reader.read( from_file( infile ), tax ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax) )
		self.assertTrue( validate(tax) )

		t = find_taxon_by_name( tax, "Candidatus Caldiarchaeum" )
		self.assertNotEqual( t, None)

if __name__ == '__main__':
    unittest.main()
