import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.population import *

class WindowIterator(unittest.TestCase):

	def test_ChromosomeIteratorChromosome(self):

		infile = "test/data/population/empty.pileup"
		# std::string const infile = environment->data_dir + "population/example.pileup";

		# Make a Lambda Iterator over the data stream,
		# and set up the window iterator. Rename to `win_it` to use it with the below test code.
		data_gen = make_variant_input_iterator_from_pileup_file( infile )
		a = data_gen.begin()
		print(a)
		win_it = make_default_chromosome_iterator( a, data_gen.end() )
		
		print(type(win_it))

		# Run the tests.
		pos_per_chr = []
		for it in win_it:
			True
		#	pos_per_chr.append(0)
		#	# LOG_DBG << "chr " << it->chromosome();
		#	for elem in it:
		#		# LOG_DBG1 << "at " << elem.position;
		#		# (void) elem;
		#		pos_per_chr[len(pos_per_chr)-1] += 1
		#	# LOG_DBG << "done " << cnt;

		# Tests
		self.assertEqual( 0, len(pos_per_chr) )

if __name__ == '__main__':
	unittest.main()
