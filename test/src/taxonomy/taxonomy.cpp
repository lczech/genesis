/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include "lib/taxonomy/taxon.hpp"
#include "lib/taxonomy/taxonomy.hpp"
#include "lib/taxonomy/taxscriptor.hpp"
#include "lib/taxonomy/formats/taxscriptor_parser.hpp"
#include "lib/taxonomy/formats/taxscriptor_generator.hpp"
#include "lib/taxonomy/functions/taxonomy.hpp"
#include "lib/taxonomy/functions/taxscriptor.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxonomy, Basics )
{
    Taxonomy tax;

    // Add some simple taxa.
    auto& r = tax.add_child( "Tax_1" );
    r.add_child( "Tax_2" );
    EXPECT_EQ( 2, total_taxa_count( tax ));
    EXPECT_TRUE( tax.has_child( "Tax_1" ));

    // Add recursively.
    auto a = Taxon( "Tax_1" );
    a.add_child( "Tax_2" ).add_child( "Tax_3" );
    tax.add_child( a );
    EXPECT_EQ( 3, total_taxa_count( tax ));
    EXPECT_TRUE( validate( tax ));
}

TEST( Taxonomy, AddChildren )
{
    Taxonomy tax;
    auto parser = TaxscriptorParser();

    // Simple
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_3;Tax_4" ));
    EXPECT_EQ( 4, total_taxa_count( tax ));
    EXPECT_TRUE( validate( tax ));

    // Leave some out
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_5;Tax_6;" ));
    auto& par = add_from_taxscriptor( tax, parser( "Tax_1;;;Tax_7;Tax8" ));
    EXPECT_EQ( 4, taxon_level( par ));
    EXPECT_EQ( 10, total_taxa_count( tax ));
    EXPECT_EQ( "Tax_1", par.parent()->parent()->name() );

    // Invalid strings
    EXPECT_THROW( add_from_taxscriptor( tax, parser( "" )), std::runtime_error );
    EXPECT_THROW( add_from_taxscriptor( tax, parser( ";Tax_x" )), std::runtime_error );

    // Remove some
    par.parent()->parent()->remove_child( "Tax_7" );
    EXPECT_EQ( 8, total_taxa_count( tax ));
    EXPECT_TRUE( validate( tax ));
}

TEST( Taxonomy, ToString )
{
    Taxonomy tax;
    auto parser    = TaxscriptorParser();
    auto generator = TaxscriptorGenerator();

    // Standard behaviour.
    std::string s1 = "Tax_1;Tax_2;Tax_3;Tax_4";
    auto&       r1 = add_from_taxscriptor( tax, parser( s1 ));
    ASSERT_TRUE( validate( tax ));
    EXPECT_EQ( s1, generator( r1 ));

    // With left out elements.
    std::string s2 = "Tax_1;;Tax_3;Tax_4";
    auto&       r2 = add_from_taxscriptor( tax, parser( s2 ));
    ASSERT_TRUE( validate( tax ));
    EXPECT_NE( s2, generator( r2 ));

    // Leav out elements in generator as well.
    generator.trim_nested_duplicates( true );
    ASSERT_TRUE( validate( tax ));
    EXPECT_EQ( s2, generator( r2 ));
}

TEST( Taxonomy, Remove )
{
    // Add some elements.
    Taxonomy tax;
    auto parser = TaxscriptorParser();

    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_3;Tax_4" ));
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_3;Tax_5" ));
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_3;Tax_6" ));
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_7;Tax_8" ));
    add_from_taxscriptor( tax, parser( "Tax_1;Tax_2;Tax_9;Tax_10" ));
    EXPECT_EQ( 10, total_taxa_count( tax ));

    // Remove specific Taxon.
    tax["Tax_1"]["Tax_2"].remove_child( "Tax_7" );
    EXPECT_TRUE( validate( tax ));
    EXPECT_EQ( 8, total_taxa_count( tax ));

    // Remove fourth level.
    remove_taxa_at_level( tax, 3 );
    EXPECT_TRUE( validate( tax ));
    EXPECT_EQ( 4, total_taxa_count( tax ));

    // Remove third level.
    remove_taxa_at_level( tax, 2 );
    EXPECT_TRUE( validate( tax ));
    EXPECT_EQ( 2, total_taxa_count( tax ));
}

TEST( Taxonomy, ForEach )
{
    // Add some elements.
    Taxonomy tax;
    auto parser = TaxscriptorParser();

    add_from_taxscriptor( tax, parser( "A;B;C;D" ));
    add_from_taxscriptor( tax, parser( "A;B;E;F" ));
    add_from_taxscriptor( tax, parser( "A;G;H;I" ));
    add_from_taxscriptor( tax, parser( "A;G;H;J" ));
    add_from_taxscriptor( tax, parser( "K;L" ));
    add_from_taxscriptor( tax, parser( "K;M" ));
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
