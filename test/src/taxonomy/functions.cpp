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

#include "genesis/taxonomy/formats/taxonomy_reader.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/formats/taxopath_parser.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxopath.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/printers/nested.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

using namespace genesis;
using namespace genesis::taxonomy;

TEST( Taxonomy, Counts )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile;

    auto reader = TaxonomyReader();
    reader.rank_field_position( 2 );

    // Read file.
    Taxonomy tax;
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.unordered";
    EXPECT_NO_THROW( reader.read( utils::from_file( infile ), tax ));
    EXPECT_EQ( 32, total_taxa_count(tax) );
    EXPECT_TRUE( validate( tax ));

    EXPECT_EQ( 16, taxa_count_lowest_levels( tax ));

    // Level count.
    EXPECT_EQ( 1, taxa_count_at_level( tax, 0 ));
    EXPECT_EQ( 4, taxa_count_at_level( tax, 1 ));
    EXPECT_EQ( 5, taxa_count_at_level( tax, 2 ));
    EXPECT_EQ( 7, taxa_count_at_level( tax, 3 ));
    EXPECT_EQ( 6, taxa_count_at_level( tax, 4 ));
    EXPECT_EQ( 9, taxa_count_at_level( tax, 5 ));
    EXPECT_EQ( 0, taxa_count_at_level( tax, 6 ));

    // Level all count.
    auto level_count = taxa_count_levels( tax );
    std::vector< size_t > level_count_ref { 1, 4, 5, 7, 6, 9 };
    EXPECT_EQ( level_count_ref, level_count );

    // Rank count.
    EXPECT_EQ( 1, taxa_count_with_rank( tax, "Domain" ));
    EXPECT_EQ( 4, taxa_count_with_rank( tax, "Phylum" ));
    EXPECT_EQ( 5, taxa_count_with_rank( tax, "Class" ));
    EXPECT_EQ( 7, taxa_count_with_rank( tax, "Order" ));
    EXPECT_EQ( 6, taxa_count_with_rank( tax, "Family" ));
    EXPECT_EQ( 9, taxa_count_with_rank( tax, "Genus" ));
    EXPECT_EQ( 0, taxa_count_with_rank( tax, "Something" ));

    // All rank counts.
    auto rank_count = taxa_count_ranks( tax );
    std::unordered_map< std::string, size_t> rank_count_ref = {
        { "domain", 1 },
        { "phylum", 4 },
        { "class",  5 },
        { "order",  7 },
        { "family", 6 },
        { "genus",  9 }
    };
    EXPECT_EQ( rank_count_ref, rank_count );
}

TEST( Taxonomy, IsLeaf )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read the test taxonomy
    std::string const infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.ordered";
    auto const tax = TaxonomyReader().read( utils::from_file( infile ));

    size_t il_cnt = 0;
    size_t is_cnt = 0;

    // Test the leaf function
    for( auto const& taxon : preorder( tax )) {
        int const il = taxon_is_leaf( taxon );
        int const is = taxon_is_single_lineage( taxon );

        // A leaf always also is a single lineage, and a non-single lineage is never a leaf.
        EXPECT_TRUE( !il || is );
        il_cnt += il;
        is_cnt += is;

        // LOG_DBG << il << " " << is << " " << TaxopathGenerator().to_string( taxon );
    }

    EXPECT_EQ( 16, il_cnt );
    EXPECT_EQ( 24, is_cnt );
}
