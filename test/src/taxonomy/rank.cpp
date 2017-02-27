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

#include "genesis/taxonomy/functions/ranks.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxonomy, Rank )
{
    EXPECT_EQ( "Kingdom", rank_from_abbreviation( 'k' ));
    EXPECT_EQ( "",        rank_from_abbreviation( 'q' ));

    EXPECT_EQ( "f", rank_to_abbreviation( "familY" ));
    EXPECT_EQ( "",  rank_to_abbreviation( "hamlet" ));

    auto rra_0 = resolve_rank_abbreviation( "K__Bacteria" );
    EXPECT_EQ( "Kingdom",  rra_0.first );
    EXPECT_EQ( "Bacteria", rra_0.second );

    auto rra_1 = resolve_rank_abbreviation( "q__quartz" );
    EXPECT_EQ( "",  rra_1.first );
    EXPECT_EQ( "q__quartz", rra_1.second );

    auto rra_2 = resolve_rank_abbreviation( "Panthera tigris altaicia" );
    EXPECT_EQ( "",  rra_2.first );
    EXPECT_EQ( "Panthera tigris altaicia", rra_2.second );
}
