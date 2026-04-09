import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.population import *
from mylibgenesis.genesis.sequence import *

class VariantTest(unittest.TestCase):

	def test_GuessBasesReferenceGenome(self):

		# Basic testing of the ref genome
		ref_genome = ReferenceGenome()
		ref_genome.add( Sequence( "1", "AAAANSWAZK" ))
		self.assertRaises( RuntimeError, ref_genome.get, "X")
		self.assertEqual( 'A', ref_genome.get( "1" )[0] )

		# Make some variants
		vars = [Variant()] * 10
		#auto vars = std::vector<Variant>( 10 )

		# Default case, where ref base is most abundant
		vars[0].chromosome = "1"
		vars[0].position = 1
		vars[0].reference_base = 'A'
		vars[0].samples = [BaseCounts()]
		vars[0].samples[0].a_count = 5
		vars[0].samples[0].c_count = 3
		guess_and_set_ref_and_alt_bases( vars[0], ref_genome )
		self.assertEqual( 'A', vars[0].reference_base )
		self.assertEqual( 'C', vars[0].alternative_base )

		# Default case, but alt base is more abundant than ref
		vars[1].chromosome = "1"
		vars[1].position = 2
		vars[1].reference_base = 'A'
		vars[1].samples = [BaseCounts()]
		vars[1].samples[0].a_count = 3
		vars[1].samples[0].c_count = 5
		guess_and_set_ref_and_alt_bases( vars[1], ref_genome )
		self.assertEqual( 'A', vars[1].reference_base )
		self.assertEqual( 'C', vars[1].alternative_base )

		# Default case, no ref given, ref most abundant
		vars[2].chromosome = "1"
		vars[2].position = 3
		vars[2].samples = [BaseCounts()]
		vars[2].samples[0].a_count = 5
		vars[2].samples[0].c_count = 3
		guess_and_set_ref_and_alt_bases( vars[2], ref_genome )
		self.assertEqual( 'A', vars[2].reference_base )
		self.assertEqual( 'C', vars[2].alternative_base )

		# Default case, no ref given, alt most abundant
		vars[3].chromosome = "1"
		vars[3].position = 4
		vars[3].samples = [BaseCounts()]
		vars[3].samples[0].a_count = 3
		vars[3].samples[0].c_count = 5
		guess_and_set_ref_and_alt_bases( vars[3], ref_genome )
		self.assertEqual( 'A', vars[3].reference_base )
		self.assertEqual( 'C', vars[3].alternative_base )

		# No usable ref in genome
		vars[4].chromosome = "1"
		vars[4].position = 5
		vars[4].samples = [BaseCounts()]
		vars[4].samples[0].a_count = 3
		vars[4].samples[0].c_count = 5
		guess_and_set_ref_and_alt_bases( vars[4], ref_genome )
		self.assertEqual( 'C', vars[4].reference_base )
		self.assertEqual( 'A', vars[4].alternative_base )

		# Ambiguity for ref in genome
		vars[5].chromosome = "1"
		vars[5].position = 6
		vars[5].samples = [BaseCounts()]
		vars[5].samples[0].a_count = 3
		vars[5].samples[0].c_count = 5
		guess_and_set_ref_and_alt_bases( vars[5], ref_genome )
		self.assertEqual( 'C', vars[5].reference_base )
		self.assertEqual( 'A', vars[5].alternative_base )

		# Ambiguity for alt in genome
		vars[6].chromosome = "1"
		vars[6].position = 7
		vars[6].samples = [BaseCounts()]
		vars[6].samples[0].a_count = 3
		vars[6].samples[0].c_count = 5
		guess_and_set_ref_and_alt_bases( vars[6], ref_genome )
		self.assertEqual( 'C', vars[6].reference_base )
		self.assertEqual( 'A', vars[6].alternative_base )

		# Error: wrong ref base
		vars[7].chromosome = "1"
		vars[7].position = 8
		vars[7].reference_base = 'C'
		self.assertRaises( RuntimeError, guess_and_set_ref_and_alt_bases, vars[7], ref_genome)

		# Error: Invalid ref base in genome
		vars[8].chromosome = "1"
		vars[8].position = 9
		vars[8].samples = [BaseCounts()]
		vars[8].samples[0].a_count = 3
		vars[8].samples[0].c_count = 5
		self.assertRaises( RuntimeError, guess_and_set_ref_and_alt_bases, vars[8], ref_genome)

		# Error: Non containing ambiguity base in genome
		vars[9].chromosome = "1"
		vars[9].position = 10
		vars[9].samples = [BaseCounts()]
		vars[9].samples[0].a_count = 3
		vars[9].samples[0].c_count = 5
		self.assertRaises( RuntimeError, guess_and_set_ref_and_alt_bases, vars[9], ref_genome)

if __name__ == '__main__':
	unittest.main()
