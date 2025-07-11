import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *

class Rank(unittest.TestCase):

	def test_rank(self):

		self.assertEqual( "Kingdom", rank_from_abbreviation( 'k' ))
		self.assertEqual( "", rank_from_abbreviation( 'q' ))

		self.assertEqual( "f", rank_to_abbreviation( "familY" ) )
		self.assertEqual( "", rank_to_abbreviation("hamlet") )

		rra_0 = resolve_rank_abbreviation( "K__Bacteria" )
		self.assertEqual( "Kingdom", rra_0[0] )
		self.assertEqual( "Bacteria", rra_0[1] )

		rra_1 = resolve_rank_abbreviation( "q__quartz" )
		self.assertEqual( "", rra_1[0] )
		self.assertEqual( "q__quartz", rra_1[1])

		rra_2 = resolve_rank_abbreviation( "Panthera tigris altaicia" )
		self.assertEqual( "", rra_2[0])
		self.assertEqual( "Panthera tigris altaicia", rra_2[1] )

if __name__ == '__main__':
    unittest.main()
