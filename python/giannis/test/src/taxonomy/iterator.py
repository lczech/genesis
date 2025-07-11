import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

levelorder_global = ""
def levelorder_lambda( tax: Taxon) -> type(None):
	global levelorder_global
	levelorder_global += tax.name()

preorder_global = ""
def preorder_lambda( tax: Taxon) -> type(None):
	global preorder_global
	preorder_global += tax.name()

postorder_global = ""
def postorder_lambda( tax: Taxon) -> type(None):
	global postorder_global
	postorder_global += tax.name()

for_each_result_global = ""
def print_preorder_lambda( tax: Taxon):
	global for_each_result_global
	gen = TaxopathGenerator()
	name = gen( tax )
	for_each_result_global += name + "\n";

class Iterators(unittest.TestCase):

	def test_foreach(self):
		global levelorder_global
		global preorder_global
		global postorder_global

		# Add some elements.
		tax = Taxonomy()
		parser = TaxopathParser()

		add_from_taxopath( tax, parser.parse( "A;B;C;D" ))
		add_from_taxopath( tax, parser.parse( "A;B;E;F" ))
		add_from_taxopath( tax, parser.parse( "A;G;H;I" ))
		add_from_taxopath( tax, parser.parse( "A;G;H;J" ))
		add_from_taxopath( tax, parser.parse( "K;L" ))
		add_from_taxopath( tax, parser.parse( "K;M" ))
		self.assertEqual( 13, total_taxa_count(tax) )

		# std::cout << tax;

		# Levelorder with inner taxa.
		levelorder_global = ""
		levelorder_for_each( tax, levelorder_lambda, True)
		self.assertEqual( "AKBGLMCEHDFIJ", levelorder_global)

		# Levelorder without inner taxa.
		levelorder_global = ""
		levelorder_for_each( tax, levelorder_lambda, False)
		self.assertEqual( "LMDFIJ", levelorder_global )

		# Preorder with inner taxa.
		preorder_global = ""
		preorder_for_each( tax, preorder_lambda, True)
		self.assertEqual( "ABCDEFGHIJKLM", preorder_global )

		# Preorder without inner taxa.
		preorder_global = ""
		preorder_for_each( tax, preorder_lambda, False)
		self.assertEqual( "DFIJLM", preorder_global )

		# Postorder with inner taxa.
		postorder_global = ""
		postorder_for_each( tax, postorder_lambda, True)
		self.assertEqual( "DCFEBIJHGALMK", postorder_global )

		# Postorder without inner taxa.
		postorder_global = ""
		postorder_for_each( tax, postorder_lambda, False)
		self.assertEqual( "DFIJLM", postorder_global )

		# Final check.
		self.assertTrue( validate( tax ));

	def test_iterator_preorder(self):
		global for_each_result_global

		reader = TaxonomyReader()
		reader.rank_field_position( 2 )

		# Read file.
		tax = Taxonomy()
		infile = "./test/data/taxonomy/tax_slv_ssu_123.1.unordered"
		try:
			reader.read( from_file( infile ), tax )
		except BaseException as ex:
			self.fail("reader.read() raised Exception unexpectedly!")
		self.assertEqual( 32, total_taxa_count(tax) )
		self.assertTrue( validate( tax ))

		# Build string using iterator.
		iterator_result = ""
		for it in preorder(tax):
			gen = TaxopathGenerator()
			name = gen( it.taxon() )
			iterator_result += name + "\n"

		# Use for each loop as control.
		for_each_result_global = ""
		preorder_for_each( tax, print_preorder_lambda )

		self.assertEqual( for_each_result_global, iterator_result )

if __name__ == '__main__':
    unittest.main()
