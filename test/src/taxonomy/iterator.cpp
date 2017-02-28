/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/formats/taxopath_parser.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxopath.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/iterator/postorder.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

#include <string>

using namespace genesis::taxonomy;

TEST( Taxonomy, ForEach )
{
    // Add some elements.
    Taxonomy tax;
    auto parser = TaxopathParser();

    add_from_taxopath( tax, parser( "A;B;C;D" ));
    add_from_taxopath( tax, parser( "A;B;E;F" ));
    add_from_taxopath( tax, parser( "A;G;H;I" ));
    add_from_taxopath( tax, parser( "A;G;H;J" ));
    add_from_taxopath( tax, parser( "K;L" ));
    add_from_taxopath( tax, parser( "K;M" ));
    EXPECT_EQ( 13, total_taxa_count( tax ));

    // std::cout << tax;

    // Levelorder with inner taxa.
    std::string levelorder;
    levelorder_for_each(
        tax,
        [ &levelorder ] ( Taxon & tax ) {
            levelorder += tax.name();
        },
        true
    );
    EXPECT_EQ( "AKBGLMCEHDFIJ", levelorder );

    // Levelorder without inner taxa.
    levelorder = "";
    levelorder_for_each(
        tax,
        [ &levelorder ] ( Taxon & tax ) {
            levelorder += tax.name();
        },
        false
    );
    EXPECT_EQ( "LMDFIJ", levelorder );

    // Preorder with inner taxa.
    std::string preorder;
    preorder_for_each(
        tax,
        [ &preorder ] ( Taxon & tax ) {
            preorder += tax.name();
        },
        true
    );
    EXPECT_EQ( "ABCDEFGHIJKLM", preorder );

    // Preorder without inner taxa.
    preorder = "";
    preorder_for_each(
        tax,
        [ &preorder ] ( Taxon & tax ) {
            preorder += tax.name();
        },
        false
    );
    EXPECT_EQ( "DFIJLM", preorder );

    // Postorder with inner taxa.
    std::string postorder;
    postorder_for_each(
        tax,
        [ &postorder ] ( Taxon & tax ) {
            postorder += tax.name();
        },
        true
    );
    EXPECT_EQ( "DCFEBIJHGALMK", postorder );

    // Postorder without inner taxa.
    postorder = "";
    postorder_for_each(
        tax,
        [ &postorder ] ( Taxon & tax ) {
            postorder += tax.name();
        },
        false
    );
    EXPECT_EQ( "DFIJLM", postorder );

    // Final check.
    EXPECT_TRUE( validate( tax ));
}

TEST( Taxonomy, IteratorPreorder )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile;

    auto reader = TaxonomyReader();
    reader.rank_field_position( 2 );

    // Read file.
    Taxonomy tax;
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.unordered";
    EXPECT_NO_THROW( reader.from_file( infile, tax ));
    EXPECT_EQ( 32, total_taxa_count(tax) );
    EXPECT_TRUE( validate( tax ));

    // Build string using iterator.
    std::string iterator_result;
    for( auto it : preorder( tax )) {
        auto gen = TaxopathGenerator();
        auto name = gen( it.taxon() );
        iterator_result += name + "\n";
    }

    // Use for each loop as control.
    std::string for_each_result;
    auto print_preorder = [&] ( Taxon const& taxon ) {
        auto gen = TaxopathGenerator();
        auto name = gen( taxon );
        for_each_result += name + "\n";
    };
    preorder_for_each( tax, print_preorder );

    EXPECT_EQ( for_each_result, iterator_result );
}
