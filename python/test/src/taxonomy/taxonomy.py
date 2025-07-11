import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class TaxonomyTest(unittest.TestCase):

	def test_basics(self):

		tax = Taxonomy()

		r = tax.add_child( "Tax_1" )
		r.add_child( "Tax_2" )
		self.assertEqual( 2, total_taxa_count(tax) )
		if 2 != total_taxa_count( tax ):
			print("Error at BASICS-1")
		self.assertTrue( tax.has_child("Tax_1"))

		a = Taxon( "Tax_1" )
		a.add_child( "Tax_2" ).add_child( "Tax_3" )
		tax.add_child( a )
		self.assertEqual( 3, total_taxa_count(tax) )
		self.assertTrue( validate(tax) )

	def test_add_children(self):

		tax = Taxonomy()
		parser = TaxopathParser();

		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_3;Tax_4" ))
		self.assertEqual( 4, total_taxa_count(tax) )
		self.assertTrue( validate(tax) )

		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_5;Tax_6;" ))
		par = add_from_taxopath( tax, parser.parse( "Tax_1;;;Tax_7;Tax8" ))
		self.assertEqual( 4, taxon_level(par) )
		self.assertEqual( 10, total_taxa_count(tax) )
		self.assertEqual( "Tax_1", par.parent().parent().name())

		with self.assertRaises(RuntimeError):
			add_from_taxopath( tax, parser.parse( "" ))
		with self.assertRaises(RuntimeError):
			add_from_taxopath( tax, parser.parse( ";Tax_x" ))

		par.parent().parent().remove_child( "Tax_7" )
		self.assertEqual( 8, total_taxa_count(tax) )
		self.assertTrue( validate(tax) )

	def test_to_string(self):

		tax = Taxonomy()
		parser    = TaxopathParser()
		generator = TaxopathGenerator()

		s1 = "Tax_1;Tax_2;Tax_3;Tax_4"
		r1 = add_from_taxopath( tax, parser.parse( s1 ))
		self.assertTrue( validate(tax) )
		self.assertEqual( s1, generator( r1 ) )

		s2 = "Tax_1;;Tax_3;Tax_4"
		r2 = add_from_taxopath( tax, parser.parse( s2 ))
		self.assertTrue( validate(tax) )
		self.assertNotEqual( s2, generator(r2) )

		generator.trim_nested_duplicates( True )
		self.assertTrue( validate(tax) )
		self.assertEqual( s2, generator( r2 ) )

	def test_remove(self):

		tax = Taxonomy()
		parser = TaxopathParser()

		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_3;Tax_4" ))
		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_3;Tax_5" ))
		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_3;Tax_6" ))
		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_7;Tax_8" ))
		add_from_taxopath( tax, parser.parse( "Tax_1;Tax_2;Tax_9;Tax_10" ))
		self.assertEqual( 10, total_taxa_count(tax) )

		tax["Tax_1"]["Tax_2"].remove_child( "Tax_7" )
		self.assertTrue( validate(tax) )
		self.assertEqual( 8, total_taxa_count(tax) )

		remove_taxa_at_level( tax, 3 )
		self.assertTrue( validate(tax) )
		self.assertEqual( 4, total_taxa_count(tax) )

		remove_taxa_at_level( tax, 2 )
		self.assertTrue( validate(tax) )
		self.assertEqual( 2, total_taxa_count(tax) )

if __name__ == '__main__':
    unittest.main()
