/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/taxonomy/formats/taxonomy_reader.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/tree.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/tree.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;
using namespace genesis::tree;
using namespace genesis::utils;

TEST( Taxonomy, Tree )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.clean";
    auto const reader = TaxonomyReader();
    Taxonomy tax;
    EXPECT_NO_THROW( reader.read( from_file( infile ), tax ));
    EXPECT_EQ( 32, total_taxa_count(tax) );
    sort_by_name( tax );
    EXPECT_TRUE( validate( tax ));

    // auto const tree = taxonomy_to_tree( tax );
    auto nw = CommonTreeNewickWriter();
    nw.enable_branch_lengths( false );
    nw.trailing_new_line( false );

    // LOG_DBG << "default";
    auto const t1 = taxonomy_to_tree( tax );
    // LOG_DBG << nw.to_string( t1 );
    // nw.to_file( t1, "/home/lucas/t1.newick" );
    EXPECT_EQ(
        "(((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum),(SK190,Caldisphaera,Acidilobus),a87Y42,4136-1-21,1A-6),Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Candidatus_Caldiarchaeum),(Deep_Sea_Euryarchaeotic_Group_DSEG,Candidatus_Aenigmarchaeum));",
        nw.to_string( t1 )
    );

    // LOG_DBG << "false, true, -1";
    auto const t2 = taxonomy_to_tree( tax, true, false, -1 );
    // LOG_DBG << nw.to_string( t2 );
    // nw.to_file( t2, "/home/lucas/t2.newick" );
    EXPECT_EQ(
        "(((((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum)),(SK190,(Caldisphaera),(Acidilobus)),a87Y42,4136-1-21,1A-6)),Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,(((Candidatus_Caldiarchaeum)))),(Deep_Sea_Euryarchaeotic_Group_DSEG,(((Candidatus_Aenigmarchaeum)))));",
        nw.to_string( t2 )
    );

    // LOG_DBG << "true, false, -1";
    auto const t3 = taxonomy_to_tree( tax, false, true, -1 );
    // LOG_DBG << nw.to_string( t3 );
    // nw.to_file( t3, "/home/lucas/t3.newick" );
    EXPECT_EQ(
        "(((Staphylothermus,Ignisphaera,Ignicoccus,Desulfurococcus,Aeropyrum)Desulfurococcaceae,(SK190,Caldisphaera,Acidilobus)Acidilobales,a87Y42,4136-1-21,1A-6)Thermoprotei,Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Candidatus_Caldiarchaeum)Aigarchaeota,(Deep_Sea_Euryarchaeotic_Group_DSEG,Candidatus_Aenigmarchaeum)Aenigmarchaeota)Archaea;",
         nw.to_string( t3 )
    );

    // LOG_DBG << "false, false, 2";
    auto const t4 = taxonomy_to_tree( tax, true, true, 2 );
    // LOG_DBG << nw.to_string( t4 );
    // nw.to_file( t4, "/home/lucas/t4.newick" );
    EXPECT_EQ(
        "((Thermoprotei)Crenarchaeota,Ancient_Archaeal_Group_AAG,(Terrestrial_Hot_Spring_Gp_THSCG,Aigarchaeota_Incertae_Sedis)Aigarchaeota,(Deep_Sea_Euryarchaeotic_Group_DSEG,Aenigmarchaeota_Incertae_Sedis)Aenigmarchaeota)Archaea;",
        nw.to_string( t4 )
    );
}
