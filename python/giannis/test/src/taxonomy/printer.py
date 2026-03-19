import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class Printer(unittest.TestCase):

	def test_printer_nested(self):

		reader = TaxonomyReader()
		reader.rank_field_position( 2 )

		tax = Taxonomy()
		infile = "test/data/taxonomy/tax_slv_ssu_123.1.unordered";

		try:
			reader.read( from_file( infile ), tax )
		except Exception as e:
		        self.fail("reader.read( from_file( infile ), tax ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax) )
		sort_by_name( tax )
		self.assertTrue( validate(tax) )

		printer = PrinterNested()
		all = printer( tax )

		printer.line_limit( 0 )
		self.assertEqual( "", printer(tax) )

		printer.line_limit( 32 )
		self.assertEqual( all, printer(tax) )

		import mylibgenesis.genesis.utils # to use utils::head with another variable named head
		for i in range(1,32):
			printer.line_limit( i )
			head = printer( tax )

			self.assertEqual( mylibgenesis.genesis.utils.head(all, i) + "\n...\n", head)

if __name__ == '__main__':
    unittest.main()
