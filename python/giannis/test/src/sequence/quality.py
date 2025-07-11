import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

sanger = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI"
illumina13 = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefgh"
illumina15 = "BCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghi"
illumina18 = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJ"
solexa = ";<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefgh"

class PrintTest(unittest.TestCase):

	def test_quality_decoding_char(self):

		for i in range(0,len(sanger)):
			temp = [i]
			self.assertEqual( temp, quality_decode_to_phred_score( sanger[i], QualityEncoding.kSanger))
		for i in range(0,len(illumina13)):
			temp = [i]
			self.assertEqual( temp, quality_decode_to_phred_score( illumina13[i], QualityEncoding.kIllumina13))

		for i in range(0,len(illumina15)):
			temp = [i]
			temp2 = quality_decode_to_phred_score( illumina15[i], QualityEncoding.kIllumina15 )
			temp2 = temp2[0] - 2
			temp2 = [temp2]
			self.assertEqual( temp, temp2)
		for i in range(0,len(illumina18)):
			temp = [i]
			self.assertEqual( temp, quality_decode_to_phred_score( illumina18[i], QualityEncoding.kIllumina18))

		self.assertEqual( [1], quality_decode_to_phred_score( solexa[0], QualityEncoding.kSolexa ))
		self.assertEqual( [1], quality_decode_to_phred_score( solexa[1], QualityEncoding.kSolexa ))
		self.assertEqual( [2], quality_decode_to_phred_score( solexa[2], QualityEncoding.kSolexa ))
		self.assertEqual( [2], quality_decode_to_phred_score( solexa[3], QualityEncoding.kSolexa ))
		self.assertEqual( [3], quality_decode_to_phred_score( solexa[4], QualityEncoding.kSolexa ))
		self.assertEqual( [3], quality_decode_to_phred_score( solexa[5], QualityEncoding.kSolexa ))
		self.assertEqual( [4], quality_decode_to_phred_score( solexa[6], QualityEncoding.kSolexa ))
		self.assertEqual( [4], quality_decode_to_phred_score( solexa[7], QualityEncoding.kSolexa ))
		self.assertEqual( [5], quality_decode_to_phred_score( solexa[8], QualityEncoding.kSolexa ))
		self.assertEqual( [5], quality_decode_to_phred_score( solexa[9], QualityEncoding.kSolexa ))
		self.assertEqual( [6], quality_decode_to_phred_score( solexa[10], QualityEncoding.kSolexa ))
		self.assertEqual( [7], quality_decode_to_phred_score( solexa[11], QualityEncoding.kSolexa ))
		self.assertEqual( [8], quality_decode_to_phred_score( solexa[12], QualityEncoding.kSolexa ))
		self.assertEqual( [9], quality_decode_to_phred_score( solexa[13], QualityEncoding.kSolexa ))
		self.assertEqual( [10], quality_decode_to_phred_score( solexa[14], QualityEncoding.kSolexa ))
		for i in range(15,len(solexa)):
			temp = [i]
			temp2 = quality_decode_to_phred_score( solexa[i], QualityEncoding.kSolexa )
			temp2 = temp2[0] + 5
			temp2 = [temp2]
			self.assertEqual( [i], temp2)

	def test_quality_decoding_string(self):

		sanger_v = list(range(0,len(sanger)))
		self.assertEqual( sanger_v, quality_decode_to_phred_score( sanger, QualityEncoding.kSanger))

		illumina13_v = list(range(0,len(illumina13)))
		self.assertEqual( illumina13_v, quality_decode_to_phred_score( illumina13, QualityEncoding.kIllumina13))

		illumina15_v = list(range(2,len(illumina15)+2))
		self.assertEqual( illumina15_v, quality_decode_to_phred_score( illumina15, QualityEncoding.kIllumina15))

		illumina18_v = list(range(0,len(illumina18)))
		self.assertEqual( illumina18_v, quality_decode_to_phred_score( illumina18, QualityEncoding.kIllumina18 ))

		solexa_v = [None] * len(solexa)
		solexa_v[0] = 1;
		solexa_v[1] = 1;
		solexa_v[2] = 2;
		solexa_v[3] = 2;
		solexa_v[4] = 3;
		solexa_v[5] = 3;
		solexa_v[6] = 4;
		solexa_v[7] = 4;
		solexa_v[8] = 5;
		solexa_v[9] = 5;
		solexa_v[10] = 6;
		solexa_v[11] = 7;
		solexa_v[12] = 8;
		solexa_v[13] = 9;
		solexa_v[14] = 10;
		for i in range(15, len(solexa)):
			solexa_v[i] = i - 5
		self.assertEqual( solexa_v, quality_decode_to_phred_score( solexa, QualityEncoding.kSolexa ))

	def test_quality_encoding_string(self):

		scores = [1,5,10,20,50,90]
		self.assertEqual( scores, quality_decode_to_phred_score( quality_encode_from_phred_score( scores )))

		codes = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI"
		self.assertEqual( codes, quality_encode_from_phred_score( quality_decode_to_phred_score( codes )))

	def test_quality_omputations(self):

		for i in range(-5,50):
			self.assertEqual( i, error_probability_to_solexa_score( solexa_score_to_error_probability( i )))

		for i in range(0,50):
			self.assertEqual( i, error_probability_to_phred_score( phred_score_to_error_probability( i )))

		for i in range(10, 50):
			self.assertEqual( i, phred_score_to_solexa_score( solexa_score_to_phred_score( i )))
			self.assertEqual( i, solexa_score_to_phred_score( phred_score_to_solexa_score( i )))

		self.assertEqual( 1, solexa_score_to_phred_score( -5 ))
		self.assertEqual( 1, solexa_score_to_phred_score( -4 ))
		self.assertEqual( 2, solexa_score_to_phred_score( -3 ))
		self.assertEqual( 2, solexa_score_to_phred_score( -2 ))
		self.assertEqual( 3, solexa_score_to_phred_score( -1 ))
		self.assertEqual( 3, solexa_score_to_phred_score( 0 ))
		self.assertEqual( 4, solexa_score_to_phred_score( 1 ))
		self.assertEqual( 4, solexa_score_to_phred_score( 2 ))
		self.assertEqual( 5, solexa_score_to_phred_score( 3 ))
		self.assertEqual( 5, solexa_score_to_phred_score( 4 ))
		self.assertEqual( 6, solexa_score_to_phred_score( 5 ))
		self.assertEqual( 7, solexa_score_to_phred_score( 6 ))
		self.assertEqual( 8, solexa_score_to_phred_score( 7 ))
		self.assertEqual( 9, solexa_score_to_phred_score( 8 ))
		self.assertEqual( 10, solexa_score_to_phred_score( 9 ))
		self.assertEqual( 10, solexa_score_to_phred_score( 10 ))

		self.assertEqual( -5, phred_score_to_solexa_score( 0 ))
		self.assertEqual( -5, phred_score_to_solexa_score( 1 ))
		self.assertEqual( -2, phred_score_to_solexa_score( 2 ))
		self.assertEqual( 0, phred_score_to_solexa_score( 3 ))
		self.assertEqual( 2, phred_score_to_solexa_score( 4 ))
		self.assertEqual( 3, phred_score_to_solexa_score( 5 ))
		self.assertEqual( 5, phred_score_to_solexa_score( 6 ))
		self.assertEqual( 6, phred_score_to_solexa_score( 7 ))
		self.assertEqual( 7, phred_score_to_solexa_score( 8 ))
		self.assertEqual( 8, phred_score_to_solexa_score( 9 ))
		self.assertEqual( 10, phred_score_to_solexa_score( 10 ))

	def test_quality_encoding_names(self):

		self.assertEqual( QualityEncoding.kSanger, guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding.kSanger )))
		self.assertEqual( QualityEncoding.kIllumina13, guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding.kIllumina13 )))
		self.assertEqual( QualityEncoding.kIllumina15, guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding.kIllumina15 )))
		self.assertEqual( QualityEncoding.kIllumina18, guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding.kIllumina18 )))
		self.assertEqual( QualityEncoding.kSolexa, guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding.kSolexa )))

		self.assertEqual( QualityEncoding.kIllumina18, guess_quality_encoding_from_name( " Illumina." ))
		self.assertRaises( RuntimeError, guess_quality_encoding_from_name, " IlluminiaX" )
		self.assertRaises( RuntimeError, guess_quality_encoding_from_name, "" )

		self.assertEqual(  QualityEncoding.kSanger, guess_quality_encoding_from_name( "sanger" ))
		self.assertEqual(  QualityEncoding.kSolexa, guess_quality_encoding_from_name( "solexa" ))
		self.assertEqual(  QualityEncoding.kIllumina13, guess_quality_encoding_from_name( "illumina-1.3" ))
		self.assertEqual(  QualityEncoding.kIllumina15, guess_quality_encoding_from_name( "illumina-1.5" ))
		self.assertEqual(  QualityEncoding.kIllumina18, guess_quality_encoding_from_name( "illumina-1.8" ))

if __name__ == '__main__':
    unittest.main()
