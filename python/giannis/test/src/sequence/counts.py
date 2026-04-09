import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.sequence import *

class CountsTest(unittest.TestCase):

	def test_Entropy(self):
		s0 = Sequence( "", "AAAA" )
		s1 = Sequence( "", "AAAC" )
		s2 = Sequence( "", "AACG" )
		s3 = Sequence( "", "ACGT" )

		c = SiteCounts( "ACGT", 4 )

		c.add_sequence( s0 )
		c.add_sequence( s1 )
		c.add_sequence( s2 )
		c.add_sequence( s3 )

		self.assertEqual( 0.0, site_entropy( c, 0 ) )
		self.assertEqual( 2.0, site_information( c, 0 ) )
		self.assertEqual( round(0.8112781,6), round(site_entropy( c, 1), 6) )
		self.assertEqual( round(1.1887219,6), round(site_information( c, 1), 6) )
		self.assertEqual( 1.5, site_entropy( c, 2) )
		self.assertEqual( 0.5, site_information( c, 2) )
		self.assertEqual( 2.0, site_entropy( c, 3) )
		self.assertEqual( 0.0, site_information( c, 3) )

		self.assertEqual( round(4.3112783,6), round(absolute_entropy( c ),6) )
		self.assertEqual( round(1.0778196,6), round(average_entropy( c ),6))

		# ENTROPY-11
		self.assertEqual( "AAAA" , consensus_sequence_with_majorities( c ))

	def test_Consensus_Majority(self):
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).read( from_file("./test/data/sequence/dna_5_42_s.phylip"), sset)

		counts = SiteCounts( "ACGT", 42 )
		counts.add_sequences( sset )

		self.assertEqual( "AAACCCTGGCCGTTCAGGGTAAACCGTGGCCGGGCAGGGTAT", consensus_sequence_with_majorities( counts ) )

	def test_Consensus_Ambiguity(self):
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).read( from_file("./test/data/sequence/dna_5_42_s.phylip"), sset)

		counts = SiteCounts( "ACGT", 42 )
		counts.add_sequences( sset )

		self.assertEqual( "AARCCYTGGCCGTTCAGGGTAAACCGTGGCCGGKCAGGGTAT", consensus_sequence_with_ambiguities( counts, 1.0 ))
		self.assertEqual( "AARCCYTGGCCGTTCAGGGTAAACCGTGGCCGGKCAGGGTAT", consensus_sequence_with_ambiguities( counts, 0.75 ))
		self.assertEqual( "AAVCC-TKGCMGTTMMGSKTRARCCNTGGCCGKDMMGSKTAW", consensus_sequence_with_ambiguities( counts, 0.5 ))
		self.assertEqual( "AMVSB-KKGCMKKKMMGSKTRMRSSNDKGCMRKDMMVSKYAW", consensus_sequence_with_ambiguities( counts, 0.0 ))

		counts_2 = SiteCounts( "ACGT", 5 )
		self.assertEqual("-----", consensus_sequence_with_ambiguities( counts_2, 1.0, True ))
		self.assertEqual("-----", consensus_sequence_with_ambiguities( counts_2, 0.0, True ))
		self.assertEqual("-----", consensus_sequence_with_ambiguities( counts_2, 1.0, False ))
		self.assertEqual("-----", consensus_sequence_with_ambiguities( counts_2, 0.0, False ))

		counts_2.add_sequence( "-ACGT" )
		self.assertEqual("-ACGT", consensus_sequence_with_ambiguities( counts_2, 1.0, True ))
		self.assertEqual("-ACGT", consensus_sequence_with_ambiguities( counts_2, 0.0, True ))
		self.assertEqual("-ACGT", consensus_sequence_with_ambiguities( counts_2, 1.0, False ))
		self.assertEqual("-ACGT", consensus_sequence_with_ambiguities( counts_2, 0.0, False ))

		counts_2.add_sequence( "-ACCT" )
		counts_2.add_sequence( "ACCT-" )
		self.assertEqual("-ACBT", consensus_sequence_with_ambiguities( counts_2, 1.0, True ))
		self.assertEqual("-MCB-", consensus_sequence_with_ambiguities( counts_2, 0.0, True ))
		self.assertEqual("AACBT", consensus_sequence_with_ambiguities( counts_2, 1.0, False ))
		self.assertEqual("AMCBT", consensus_sequence_with_ambiguities( counts_2, 0.0, False ))

	def test_Consensus_Threshold(self):
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).read( from_file("./test/data/sequence/dna_5_42_s.phylip"), sset)

		counts = SiteCounts( "ACGT", 42 )
		counts.add_sequences( sset )

		self.assertEqual( "AMVSB-KKGCMKKKMMGSKTRMRSSNDKGCMRKDMMVSKYAW", consensus_sequence_with_threshold( counts, 1.0 ))
		self.assertEqual( "AARCSYTKGCMGTTMMGSKTRARCCVWGGCCGKKMMRSKTAW", consensus_sequence_with_threshold( counts, 0.75 ))
		self.assertEqual( "AARCCYTGGCCGTTCAGGGTAAACCRTGGCCGGKCAGGGTAT", consensus_sequence_with_threshold( counts, 0.5 ))
		self.assertEqual( "AAACCCTGGCCGTTCAGGGTAAACCGTGGCCGGGCAGGGTAT", consensus_sequence_with_threshold( counts, 0.0 ))

		counts_2 = SiteCounts( "ACGT", 5 )
		self.assertEqual( "-----", consensus_sequence_with_threshold( counts_2, 1.0, True ))
		self.assertEqual( "-----", consensus_sequence_with_threshold( counts_2, 0.0, True ))
		self.assertEqual( "-----", consensus_sequence_with_threshold( counts_2, 1.0, False ))
		self.assertEqual( "-----", consensus_sequence_with_threshold( counts_2, 0.0, False ))

		counts_2.add_sequence( "-ACGT" )
		self.assertEqual( "-ACGT", consensus_sequence_with_threshold( counts_2, 1.0, True ))
		self.assertEqual( "-ACGT", consensus_sequence_with_threshold( counts_2, 0.0, True ))
		self.assertEqual( "-ACGT", consensus_sequence_with_threshold( counts_2, 1.0, False ))
		self.assertEqual( "-ACGT", consensus_sequence_with_threshold( counts_2, 0.0, False ))

		counts_2.add_sequence( "-ACCT" )
		counts_2.add_sequence( "ACCT-" )
		self.assertEqual( "-MCB-", consensus_sequence_with_threshold( counts_2, 1.0, True ))
		self.assertEqual( "-ACCT", consensus_sequence_with_threshold( counts_2, 0.0, True ))
		self.assertEqual( "AMCBT", consensus_sequence_with_threshold( counts_2, 1.0, False ))
		self.assertEqual( "AACCT", consensus_sequence_with_threshold( counts_2, 0.0, False ))

	def test_Consensus_Cavener(self):
		sset = SequenceSet()
		PhylipReader().label_length( 10 ).read( from_file("./test/data/sequence/dna_5_42_s.phylip"), sset)

		counts = SiteCounts( "ACGT", 42 )
		counts.add_sequences( sset )

		self.assertEqual( "AARCCYTKGCMGTTMMGSKTRARCCNTGGCCGKKMMGSKTAW", consensus_sequence_cavener( counts, True ))
		self.assertEqual( "AARCCYTKGCMGTTMMGSKTRARCCNTGGCCGKKMMGSKTAW", consensus_sequence_cavener( counts, False ))

		counts_2 = SiteCounts( "ACGT", 5 )
		self.assertEqual( "-----", consensus_sequence_cavener( counts_2, True ))
		self.assertEqual( "-----", consensus_sequence_cavener( counts_2, False ))

		counts_2.add_sequence( "-ACGT" )
		self.assertEqual( "-ACGT", consensus_sequence_cavener( counts_2, True ))
		self.assertEqual( "-ACGT", consensus_sequence_cavener( counts_2, False ))

		counts_2.add_sequence( "-ACCT" )
		counts_2.add_sequence( "ACCT-" )
		self.assertEqual( "-MCB-", consensus_sequence_cavener( counts_2, True ))
		self.assertEqual( "AMCBT", consensus_sequence_cavener( counts_2, False ))

if __name__ == '__main__':
    unittest.main()
