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

#include "lib/taxonomy/rank.hpp"
#include "lib/taxonomy/taxonomy.hpp"
#include "lib/taxonomy/functions.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxonomy, Basics )
{
    Taxonomy tax;

    // Add some simple ranks.
    auto& r = tax.add_child( "Tax_1" );
    r.add_child( "Tax_2" );
    EXPECT_EQ( 2, total_taxa_count( tax ));
    EXPECT_TRUE( tax.has_child( "Tax_1" ));

    // Add recursively.
    auto a = Rank( "Tax_1" );
    a.add_child( "Tax_2" ).add_child( "Tax_3" );
    tax.add_child( a );
    EXPECT_EQ( 3, total_taxa_count( tax ));
}

TEST( Taxonomy, AddChildren )
{
    Taxonomy tax;

    // Simple
    add_children_from_string( tax, "Tax_1;Tax_2;Tax_3;Tax_4" );
    EXPECT_EQ( 4, total_taxa_count( tax ));

    // Leave some out
    add_children_from_string( tax, "Tax_1;Tax_5;Tax_6;" );
    auto par = add_children_from_string( tax, "Tax_1;;;Tax_7;Tax8" );
    EXPECT_EQ( 10, total_taxa_count( tax ));
    EXPECT_EQ( "Tax_1", par.parent()->parent()->name() );

    // Invalid strings
    EXPECT_THROW( add_children_from_string( tax, "" ), std::runtime_error );
    EXPECT_THROW( add_children_from_string( tax, ";Tax_x" ), std::runtime_error );

    // Remove some
    par.parent()->parent()->remove_child( "Tax_7" );
    EXPECT_EQ( 8, total_taxa_count( tax ));
}

TEST( Taxonomy, ToString )
{
    Taxonomy tax;

    std::string s1 = "Tax_1;Tax_2;Tax_3;Tax_4";
    auto        r1 = add_children_from_string( tax, s1 );
    EXPECT_EQ( s1, taxonomic_string( r1 ));

    std::string s2 = "Tax_1;;Tax_3;Tax_4";
    auto        r2 = add_children_from_string( tax, s2 );
    EXPECT_EQ( s2, taxonomic_string( r2 ));
    EXPECT_NE( s2, taxonomic_string( r2, ";", false ));
}

TEST( Taxonomy, Remove )
{
    // Add some elements.
    Taxonomy tax;
    add_children_from_string( tax, "Tax_1;Tax_2;Tax_3;Tax_4" );
    add_children_from_string( tax, "Tax_1;Tax_2;Tax_3;Tax_5" );
    add_children_from_string( tax, "Tax_1;Tax_2;Tax_3;Tax_6" );
    add_children_from_string( tax, "Tax_1;Tax_2;Tax_7;Tax_8" );
    EXPECT_EQ( 8, total_taxa_count( tax ));

    // Remove fourth level.
    remove_ranks_deeper_than( tax, 3 );
    EXPECT_EQ( 4, total_taxa_count( tax ));

    // Remove third level.
    remove_ranks_deeper_than( tax, 2 );
    EXPECT_EQ( 2, total_taxa_count( tax ));
}
