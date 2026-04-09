import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class CodesTest(unittest.TestCase):

	def test_code_ambiguities(self):
		codes = nucleic_acid_codes_plain() + nucleic_acid_codes_degenerated() + "N-"

		# CODE_AMBIGUITIES-1
		for code in codes:
			code_string = nucleic_acid_ambiguities( code )
			code_char   = nucleic_acid_ambiguity_code( code_string )
			self.assertEqual( code, code_char)

	def test_code_containment(self):
		self.assertTrue( nucleic_acid_code_containment( 'A', 'A' ) )
		self.assertTrue( nucleic_acid_code_containment( 'C', 'C' ) )
		self.assertTrue( nucleic_acid_code_containment( 'G', 'G' ) )
		self.assertTrue( nucleic_acid_code_containment( 'T', 'T' ) )

		self.assertTrue( nucleic_acid_code_containment( 'A', 'W' ) )
		self.assertTrue( nucleic_acid_code_containment( 'T', 'W' ) )
		self.assertTrue( nucleic_acid_code_containment( 'C', 'H' ) )

		self.assertFalse( nucleic_acid_code_containment( 'C', 'D' ) )
		self.assertFalse( nucleic_acid_code_containment( 'A', 'T' ) )
		self.assertFalse( nucleic_acid_code_containment( 'A', 'Y' ) )

		self.assertTrue( nucleic_acid_code_containment( '-', 'C', True ) )
		self.assertTrue( nucleic_acid_code_containment( '-', 'H', True ) )

		self.assertFalse( nucleic_acid_code_containment( '-', 'C', False ) )
		self.assertFalse( nucleic_acid_code_containment( '-', 'H', False ) )


if __name__ == '__main__':
    unittest.main()
