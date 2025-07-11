import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class PrinterTest(unittest.TestCase):

	def test_compact(self):

		input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"

		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/printer_py.txt' ))

		tp = PrinterCompact()
		res = tp.print(tree)

	def test_detailed(self):

		input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"

		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/printer_py.txt' ))

		tp = PrinterDetailed()
		res = tp.print(tree)

	def test_table(self):

		input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

		#tree = CommonTreeNewickReader().read( from_string( input ))
		tree = CommonTreeNewickReader().read( from_file( './test/src/tree/printer_py.txt' ))

		tp = PrinterTable()
		res = tp.print(tree)

if __name__ == '__main__':
    unittest.main()
