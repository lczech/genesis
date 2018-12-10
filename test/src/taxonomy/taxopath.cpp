/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"
#include "genesis/taxonomy/formats/taxopath_parser.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxopath.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxopath, Comparison )
{
    // Add some elements.
    Taxonomy tax;
    auto parser = TaxopathParser();

    // Create some taxopaths.
    auto tp0 = parser.parse( "Archaea" );
    auto tp1 = parser.parse( "Archaea;Something;Interesting" );
    auto tp2 = parser.parse( "Bacteria;Something;Even;More;Interesting" );
    auto tp3 = parser.parse( "Bacteria;Something;New" );

    // Boring cases
    EXPECT_TRUE( tp0 == tp0 );
    EXPECT_TRUE( tp0 != tp1 );
    EXPECT_TRUE( tp3 >= tp3 );
    EXPECT_TRUE( tp2 <= tp2 );

    // Interesting cases
    EXPECT_TRUE( tp0 < tp1 );
    EXPECT_TRUE( tp0 <= tp1 );
    EXPECT_TRUE( tp1 < tp2 );
    EXPECT_TRUE( tp0 < tp2 );
    EXPECT_TRUE( tp3 > tp2 );
    EXPECT_TRUE( tp3 > tp0 );
    EXPECT_TRUE( tp3 >= tp0 );

}
