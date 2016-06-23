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

#include "lib/taxonomy/formats/taxonomy_reader.hpp"
#include "lib/taxonomy/taxon.hpp"
#include "lib/taxonomy/taxonomy.hpp"
#include "lib/taxonomy/functions/taxonomy.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxonomy, ReaderOrder )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile;

    auto reader = TaxonomyReader();

    // Read ordered file, expect order.
    Taxonomy tax_0;
    reader.expect_strict_order( true );
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.ordered";
    EXPECT_NO_THROW( reader.from_file( infile, tax_0 ));
    EXPECT_EQ( 32, total_taxa_count(tax_0) );
    EXPECT_TRUE( validate( tax_0 ));
    sort_by_name( tax_0 );
    EXPECT_TRUE( validate( tax_0 ));

    // Read ordered file, don't use order.
    Taxonomy tax_1;
    reader.expect_strict_order( false );
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.ordered";
    EXPECT_NO_THROW( reader.from_file( infile, tax_1 ));
    EXPECT_EQ( 32, total_taxa_count(tax_1) );
    EXPECT_TRUE( validate( tax_1 ));
    sort_by_name( tax_1 );
    EXPECT_TRUE( validate( tax_1 ));

    // Read unordered file, expect order.
    Taxonomy tax_2;
    reader.expect_strict_order( true );
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.unordered";
    EXPECT_THROW( reader.from_file( infile, tax_2 ), std::runtime_error );
    EXPECT_EQ( 0, total_taxa_count(tax_2) );
    EXPECT_TRUE( validate( tax_2 ));

    // Read unordered file, don't use order.
    Taxonomy tax_3;
    reader.expect_strict_order( false );
    infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.unordered";
    EXPECT_NO_THROW( reader.from_file( infile, tax_3 ));
    EXPECT_EQ( 32, total_taxa_count(tax_3) );
    EXPECT_TRUE( validate( tax_3 ));
    sort_by_name( tax_3 );
    EXPECT_TRUE( validate( tax_3 ));
}

TEST( Taxonomy, ReaderTrimming )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "taxonomy/tax_slv_ssu_123.1.unordered";

    // Read file and check basics.
    Taxonomy tax;
    auto reader = TaxonomyReader();
    reader.taxscriptor_parser().trim_whitespaces( true );
    EXPECT_NO_THROW( reader.from_file( infile, tax ));
    EXPECT_EQ( 32, total_taxa_count(tax) );
    EXPECT_TRUE( validate( tax ));

    // We trimmed white spaces. This should only affect the surrounding onces.
    // So, find a taxon with internal whitespaces. If it is there, this worked.
    auto t = find_taxon_by_name( tax, "Candidatus Caldiarchaeum" );
    ASSERT_NE( nullptr, t );
}
