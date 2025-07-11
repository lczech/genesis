import sys
import os
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.placement import *

class TestJplaceWriter(unittest.TestCase):

	def test_to_target(self):
		infile = "test/data/placement/test_a.jplace"
		tmpfile = "test/data/placement/test_a.tmp.jplace"

		smp = JplaceReader().read( from_file( infile ))
		self.assertEqual( 5, total_placement_count( smp ))

		JplaceWriter().write( smp, to_file( tmpfile ))

		target = ""
		#JplaceWriter().write( smp, to_string( target ))
		JplaceWriter().write( smp, to_file( 'jplace_writer_py.txt' ))
		read_again = file_read( tmpfile )
		os.remove(tmpfile)

		#self.assertTrue( not target.empty() )
		self.assertTrue( not (read_again == "") )
		#self.assertEqual( read_again.size(), target.size() )

		#smp2 = JplaceReader().read( from_string( target ))
		smp2 = JplaceReader().read( from_file( 'jplace_writer_py.txt' ))
		self.assertEqual( 5, total_placement_count( smp2 ))

		#smp3 = JplaceReader().read( from_string( read_again ))
		f = open("jplace_writer_py.txt", "w")
		f.write(read_again)
		f.close()
		smp3 = JplaceReader().read( from_file( 'jplace_writer_py.txt' ))
		self.assertEqual( 5, total_placement_count( smp3 ))
		os.remove("jplace_writer_py.txt")

if __name__ == '__main__':
    unittest.main()