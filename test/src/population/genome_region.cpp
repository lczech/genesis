/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/population/genome_region.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( GenomeRegion, Basics )
{
    GenomeRegionList list;

    // Add some regions, in different orders.
    list.add({ "X", 15, 20 });
    list.add({ "X", 1, 5 });
    list.add({ "X", 10, 15 });
    list.add({ "X", 25, 35 });

    // Add some overlapping and invalid regions.
    EXPECT_ANY_THROW( list.add({ "X", 1, 5 }); );
    EXPECT_ANY_THROW( list.add({ "X", 3, 18 }); );
    EXPECT_ANY_THROW( list.add({ "X", 45, 40 }); );
    EXPECT_ANY_THROW( list.add({ "X", 50, 50 }); );

    // Test all existing regions.
    EXPECT_FALSE( list.is_covered( "X", 0 ));
    EXPECT_TRUE(  list.is_covered( "X", 1 ));
    EXPECT_TRUE(  list.is_covered( "X", 4 ));
    EXPECT_FALSE( list.is_covered( "X", 5 ));
    EXPECT_FALSE( list.is_covered( "X", 9 ));
    EXPECT_TRUE(  list.is_covered( "X", 10 ));
    EXPECT_TRUE(  list.is_covered( "X", 15 ));
    EXPECT_TRUE(  list.is_covered( "X", 19 ));
    EXPECT_FALSE( list.is_covered( "X", 20 ));
    EXPECT_FALSE( list.is_covered( "X", 22 ));
    EXPECT_FALSE( list.is_covered( "X", 24 ));
    EXPECT_TRUE(  list.is_covered( "X", 25 ));
    EXPECT_TRUE(  list.is_covered( "X", 34 ));
    EXPECT_FALSE( list.is_covered( "X", 35 ));

    // Test some other stuff.
    EXPECT_FALSE( list.is_covered( "Y", 5 ));
    EXPECT_EQ( 4, list.size() );
}
