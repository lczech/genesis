import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class PrintTest(unittest.TestCase):

	def test_printer_simple(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = SequenceSet()
		FastaReader().read( from_file(infile), sset)

		printer = PrinterSimple()

		printer.label_mode( PrinterSimple.LabelMode.kSameLine )
		printer.length_limit(5)
		self.assertEqual("Di106BGTue: TCGAA ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kNone )
		self.assertEqual("TCGAA ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kSameLine )
		printer.sequence_limit( 2 )
		self.assertEqual("Di106BGTue: TCGAA ...\nDi145BGTue: TCGAA ...\n...\n", printer( sset ))
		printer.label_mode( PrinterSimple.LabelMode.kNone )
		self.assertEqual("TCGAA ...\nTCGAA ...\n...\n", printer( sset ))

		printer.label_mode( PrinterSimple.LabelMode.kSameLine )
		printer.color_mode (PrinterSimple.ColorMode.kBackground )
		printer.color_map( nucleic_acid_text_colors() )
		printer.length_limit( 3 )
		self.assertEqual("Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kNone )
		self.assertEqual("\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kSameLine )
		printer.color_mode (PrinterSimple.ColorMode.kForeground )
		self.assertEqual("Di106BGTue: \x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kNone )
		self.assertEqual("\x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n", printer( sset[0] ))

		printer.label_mode( PrinterSimple.LabelMode.kSameLine )
		printer.color_mode (PrinterSimple.ColorMode.kBackground )
		self.assertEqual("Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n" "Di145BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n", printer( sset ))

		printer.label_mode( PrinterSimple.LabelMode.kNone )
		self.assertEqual("\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n" "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n", printer( sset ))

	def test_printer_bitmap(self):
		infile ="./test/data/sequence/dna_10.fasta";
		sset = SequenceSet()
		FastaReader().read( from_file(infile), sset)

		print("to_stream bug")
		self.assertFalse(True)	

if __name__ == '__main__':
    unittest.main()
