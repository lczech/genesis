/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/genome_locus.hpp"
#include "genesis/population/functions/genome_locus.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Population, GenomeLocus )
{
    // Some cases with identical positions
    GenomeLocus a{ "1", 5 };
    GenomeLocus b{ "1", 8 };
    GenomeLocus c{ "2", 5 };
    GenomeLocus d{ "2", 8 };

    // Equality. Simple cases should suffice here.
    EXPECT_TRUE( a == a );
    EXPECT_TRUE( a != b );

    // Now, we do every combination of comparisons.

    // Spaceship
    EXPECT_EQ(  0, locus_compare( a, a ));
    EXPECT_EQ( -1, locus_compare( a, b ));
    EXPECT_EQ( -1, locus_compare( a, c ));
    EXPECT_EQ( -1, locus_compare( a, d ));
    EXPECT_EQ( +1, locus_compare( b, a ));
    EXPECT_EQ(  0, locus_compare( b, b ));
    EXPECT_EQ( -1, locus_compare( b, c ));
    EXPECT_EQ( -1, locus_compare( b, d ));
    EXPECT_EQ( +1, locus_compare( c, a ));
    EXPECT_EQ( +1, locus_compare( c, b ));
    EXPECT_EQ(  0, locus_compare( c, c ));
    EXPECT_EQ( -1, locus_compare( c, d ));
    EXPECT_EQ( +1, locus_compare( d, a ));
    EXPECT_EQ( +1, locus_compare( d, b ));
    EXPECT_EQ( +1, locus_compare( d, c ));
    EXPECT_EQ(  0, locus_compare( d, d ));

    // Less than, true
    EXPECT_TRUE( a < b );
    EXPECT_TRUE( a < c );
    EXPECT_TRUE( a < d );
    EXPECT_TRUE( b < c );
    EXPECT_TRUE( b < d );
    EXPECT_TRUE( c < d );

    // Less than, false
    EXPECT_FALSE( a < a );
    EXPECT_FALSE( b < a );
    EXPECT_FALSE( b < b );
    EXPECT_FALSE( c < a );
    EXPECT_FALSE( c < b );
    EXPECT_FALSE( c < c );
    EXPECT_FALSE( d < a );
    EXPECT_FALSE( d < b );
    EXPECT_FALSE( d < c );
    EXPECT_FALSE( d < d );

    // Greater than, true
    EXPECT_TRUE( b > a );
    EXPECT_TRUE( c > a );
    EXPECT_TRUE( c > b );
    EXPECT_TRUE( d > a );
    EXPECT_TRUE( d > b );
    EXPECT_TRUE( d > c );

    // Greater than, false
    EXPECT_FALSE( a > a );
    EXPECT_FALSE( a > b );
    EXPECT_FALSE( a > c );
    EXPECT_FALSE( a > d );
    EXPECT_FALSE( b > b );
    EXPECT_FALSE( b > c );
    EXPECT_FALSE( b > d );
    EXPECT_FALSE( c > c );
    EXPECT_FALSE( c > d );
    EXPECT_FALSE( d > d );

    // Less than or equal, true
    EXPECT_TRUE( a <= a );
    EXPECT_TRUE( a <= b );
    EXPECT_TRUE( a <= c );
    EXPECT_TRUE( a <= d );
    EXPECT_TRUE( b <= b );
    EXPECT_TRUE( b <= c );
    EXPECT_TRUE( b <= d );
    EXPECT_TRUE( c <= c );
    EXPECT_TRUE( c <= d );
    EXPECT_TRUE( d <= d );

    // Less than or equal, false
    EXPECT_FALSE( b <= a );
    EXPECT_FALSE( c <= a );
    EXPECT_FALSE( c <= b );
    EXPECT_FALSE( d <= a );
    EXPECT_FALSE( d <= b );
    EXPECT_FALSE( d <= c );

    // Greater than or equal, true
    EXPECT_TRUE( a >= a );
    EXPECT_TRUE( b >= a );
    EXPECT_TRUE( b >= b );
    EXPECT_TRUE( c >= a );
    EXPECT_TRUE( c >= b );
    EXPECT_TRUE( c >= c );
    EXPECT_TRUE( d >= a );
    EXPECT_TRUE( d >= b );
    EXPECT_TRUE( d >= c );
    EXPECT_TRUE( d >= d );

    // Greater than or equal, false
    EXPECT_FALSE( a >= b );
    EXPECT_FALSE( a >= c );
    EXPECT_FALSE( a >= d );
    EXPECT_FALSE( b >= c );
    EXPECT_FALSE( b >= d );
    EXPECT_FALSE( c >= d );

    // LOG_DBG << "a " << a;
    // std::string t = "a " + a.to_string();
}
