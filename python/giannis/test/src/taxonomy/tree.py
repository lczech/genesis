import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.taxonomy import *
from mylibgenesis.genesis.tree import *

class Tree(unittest.TestCase):

	def test_counts(self):

		infile = "test/data/taxonomy/tax_slv_ssu_123.1.clean"
		reader = TaxonomyReader()
		tax = Taxonomy()
		try:
			reader.read( from_file( infile ), tax )
		except:
			self.fail("reader.read( from_file( infile ), tax ) ERROR")
		self.assertEqual( 32, total_taxa_count(tax) )
		sort_by_name( tax )
		self.assertTrue( validate(tax) )

		# auto const tree = taxonomy_to_tree( tax );
		nw = CommonTreeNewickWriter()
		nw.enable_branch_lengths( False )
		nw.trailing_new_line( False )

		# LOG_DBG << "default";
		t1 = taxonomy_to_tree( tax )
		# LOG_DBG << nw.to_string( t1 );
		# nw.to_file( t1, "/home/lucas/t1.newick" );
		self.assertEqual( "(((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum),(SK190,Caldisphaera,Acidilobus),a87Y42,4136-1-21,1A-6),Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Candidatus_Caldiarchaeum),(Deep_Sea_Euryarchaeotic_Group_DSEG,Candidatus_Aenigmarchaeum));", nw.to_string( t1 ))

		# LOG_DBG << "false, true, -1";
		t2 = taxonomy_to_tree( tax, True, False, -1 )
		# LOG_DBG << nw.to_string( t2 );
		# nw.to_file( t2, "/home/lucas/t2.newick" );
		self.assertEqual( "(((((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum)),(SK190,(Caldisphaera),(Acidilobus)),a87Y42,4136-1-21,1A-6)),Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,(((Candidatus_Caldiarchaeum)))),(Deep_Sea_Euryarchaeotic_Group_DSEG,(((Candidatus_Aenigmarchaeum)))));", nw.to_string( t2 ) )

		# LOG_DBG << "true, false, -1";
		t3 = taxonomy_to_tree( tax, False, True, -1 )
		# LOG_DBG << nw.to_string( t3 );
		# nw.to_file( t3, "/home/lucas/t3.newick" );
		self.assertEqual( "(((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum)Desulfurococcaceae,(SK190,Caldisphaera,Acidilobus)Acidilobales,a87Y42,4136-1-21,1A-6)Thermoprotei,Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Candidatus_Caldiarchaeum)Aigarchaeota,(Deep_Sea_Euryarchaeotic_Group_DSEG,Candidatus_Aenigmarchaeum)Aenigmarchaeota)Archaea;", nw.to_string( t3 ) )

		# LOG_DBG << "false, false, 2";
		t4 = taxonomy_to_tree( tax, True, True, 2 )
		# LOG_DBG << nw.to_string( t4 );
		# nw.to_file( t4, "/home/lucas/t4.newick" );
		self.assertEqual( "((Thermoprotei)Crenarchaeota,Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Aigarchaeota_Incertae_Sedis)Aigarchaeota,(Deep_Sea_Euryarchaeotic_Group_DSEG,Aenigmarchaeota_Incertae_Sedis)Aenigmarchaeota)Archaea;", nw.to_string( t4 ) )

if __name__ == '__main__':
    unittest.main()
