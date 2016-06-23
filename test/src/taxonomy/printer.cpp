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
#include "lib/taxonomy/functions/taxonomy.hpp"
#include "lib/taxonomy/printers/nested.hpp"
#include "lib/taxonomy/taxon.hpp"
#include "lib/taxonomy/taxonomy.hpp"

#include "lib/utils/text/string.hpp"

#include <stdexcept>

using namespace genesis;
using namespace genesis::taxonomy;

TEST( Taxonomy, PrinterNested )
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
    sort_by_name( tax );
    EXPECT_TRUE( validate( tax ));

    // Get full printout.
    auto printer = PrinterNested();
    auto all = printer( tax );

    // Check line limit 0.
    printer.line_limit( 0 );
    EXPECT_EQ( "", printer( tax ) );

    // Check line limit 32.
    printer.line_limit( 32 );
    EXPECT_EQ( all, printer( tax ) );

    // Check line limted printout.
    for( size_t i = 1; i < 32; ++i ) {
        printer.line_limit( i );
        auto head = printer( tax );

        EXPECT_EQ( utils::head( all, i ) + "\n...\n", head );
    }

    // printer.print_ranks( true );
    // std::cout << printer( tax );
}
