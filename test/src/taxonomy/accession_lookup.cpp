/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/taxonomy/accession_lookup.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/formats/taxonomy_reader.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"

using namespace genesis;
using namespace genesis::taxonomy;

TEST( Taxonomy, AccessionLookup )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read the test taxonomy
    std::string const infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.clean";
    auto tax = TaxonomyReader().read( utils::from_file( infile ));

    // Get some taxon from the taxonomy
    auto* taxon = find_taxon_by_name( tax, "Acidilobus" );
    ASSERT_TRUE( taxon != nullptr );

    // Create a lookup for the taxonomy
    auto lookup = AccessionLookup<>();
    lookup.add( "ABCD.1", taxon );

    // Perform a lookup by full name
    EXPECT_TRUE( lookup.get( "ABCD.1 Acidilobaceae Acidilobus" ) != nullptr );

    // Perform a lookup by first word
    EXPECT_TRUE( lookup.get( "ABCD.1" ) != nullptr );
}
