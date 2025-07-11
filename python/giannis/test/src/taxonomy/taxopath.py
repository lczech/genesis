import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class Taxopath(unittest.TestCase):

	def test_comparison(self):

		tax = Taxonomy()
		parser = TaxopathParser()

		tp0 = parser.parse( "Archaea" )
		tp1 = parser.parse( "Archaea;Something;Interesting" )
		tp2 = parser.parse( "Bacteria;Something;Even;More;Interesting" )
		tp3 = parser.parse( "Bacteria;Something;New" )

		self.assertTrue(tp0 == tp0)
		self.assertTrue(tp0 != tp1)
		self.assertTrue(tp3 >= tp3)
		self.assertTrue(tp2 <= tp2)

		self.assertTrue(tp0 < tp1)
		self.assertTrue(tp0 <= tp1)
		self.assertTrue(tp1 < tp2)
		self.assertTrue(tp0 < tp2)
		self.assertTrue(tp3 > tp2)
		self.assertTrue(tp3 > tp0)
		self.assertTrue(tp3 >= tp0)

if __name__ == '__main__':
    unittest.main()
