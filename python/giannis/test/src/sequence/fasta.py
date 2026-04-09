import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class FastaTest(unittest.TestCase):

	def test_fasta_reader_validating(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = SequenceSet()
		FastaReader().valid_chars( nucleic_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual( 10, sset.size())
		self.assertEqual( 460, sset[0].length())
		self.assertEqual( "Di106BGTue", sset[0].label())
		self.assertEqual( "TCGAAACCTGC------CTA", sset[0].sites()[0:20])

	def test_reading_loop(self):
		infile = "./test/data/sequence/dna_10.fasta"
		fasta_input_iterator = FastaInputIterator( from_file( infile ))

		len = 0
		cnt = 0
		for seq in fasta_input_iterator:

			if cnt == 0:
				self.assertEqual( "Di106BGTue", seq.label())

			if cnt == 9:
				self.assertEqual( "GTCGTTCT", seq.sites()[387:387+8])

			len = max( len, seq.length() )

			cnt += 1

		# no need to bind operator bool()
		#if fasta_input_iterator == False:
		#	print("Error at READING_LOOP-3")
		self.assertEqual( 460, len)
		self.assertEqual( 10, cnt)

	def test_fasta_compressed(self):
		infile = "./test/data/sequence/dna_10.fasta.gz";
		cit = InputStream( GzipInputSource(FileInputSource( infile )) )

		sset = SequenceSet()
		FastaReader().parse_document( cit, sset )

		self.assertEqual( 10, sset.size())
		self.assertEqual( 460, sset[0].length())
		self.assertEqual( "Di106BGTue", sset[0].label())
		self.assertEqual( "TCGAAACCTGC------CTA", sset[0].sites()[0:20])

	def test_fasta_gzip(self):
		infile = "./test/data/sequence/dna_10.fasta.gz"

		sset = FastaReader().read( from_file( infile ))

		self.assertEqual( 10, sset.size())
		self.assertEqual( 460, sset[0].length())
		self.assertEqual( "Di106BGTue", sset[0].label())
		self.assertEqual( "TCGAAACCTGC------CTA", sset[0].sites()[0:20])

	def test_fasta_writer(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = SequenceSet()
		FastaReader().valid_chars( nucleic_acid_codes_all() ).read( from_file(infile), sset)

		self.assertEqual(10, sset.size())

		target = StringTarget()
		FastaWriter().line_length(50).write( sset, to_string( target ))
		read_again = file_read( infile )
		if target.get() == "":
			print("Error at FASTA_WRITER-2")
		if read_again != target.get():
			print("Error at FASTA_WRITER-3")

	def test_fasta_output_iterator(self):
		infile = "./test/data/sequence/dna_10.fasta"
		sset = FastaReader().read( from_file( infile ))
		
		target = StringTarget()
		out_it = FastaOutputIterator( target )
		"""
		out_it.writer().line_length( 50 );
		for seq in sset:
			out_it << seq;

		#print("Hi over")
		data = file_read( infile );
		self.assertEqual( data, target.get())
		"""

if __name__ == '__main__':
    unittest.main()
