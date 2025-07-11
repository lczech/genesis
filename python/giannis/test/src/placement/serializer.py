import sys
import os
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.placement import *

class TestSampleSerializer(unittest.TestCase):

	def test_SaveAndLoad(self):
		infile  = "test/data/placement/test_a.jplace"
		tmpfile = "test/data/placement/test_a.bplace"

		# Prepare a Sample with data.
		smp_save = JplaceReader().read( from_file(infile))
		self.assertEqual(5, total_placement_count(smp_save))
		self.assertTrue(validate(smp_save, True, False))

		# Save it to a file.
		try:
			SampleSerializer.save(smp_save, tmpfile)
		except Exception:
			self.fail("")

		# Load again.
		smp_load = SampleSerializer.load( tmpfile )

		# Check for correctly read data.
		self.assertEqual(5, total_placement_count(smp_load))
		self.assertTrue(validate(smp_load, True, False))

		# Make sure the file is deleted.
		os.remove(tmpfile)

if __name__ == '__main__':
	unittest.main()