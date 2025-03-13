/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/sequence/kmer/color_gamut_functions.hpp"
#include "genesis/sequence/kmer/color_gamut.hpp"
#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/printers/nested.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/bitvector/functions.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/text/string.hpp"

#include <stdexcept>
#include <iomanip>
#include <sstream>

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::taxonomy;
using namespace genesis::utils;

void test_kmer_color_taxonomy_(
    std::string const& infile,
    size_t exp_num_groups,
    size_t exp_secondary_colors,
    size_t exp_total_colors
) {
    // Read the test taxonomy
    auto const tax = read_kmer_taxonomy_from_json( from_file( infile ));
    auto const num_groups = count_taxon_groups( tax );
    EXPECT_EQ( exp_num_groups, num_groups );

    // Debugging output
    // auto printer = PrinterNested();
    // printer.indent_string( "    " );
    // LOG_DBG << printer( tax );

    // auto const bvs = make_kmer_color_gamut_seconary_colors( tax, 10, false );
    auto bvs = make_kmer_color_gamut_seconary_colors( tax );
    EXPECT_EQ( exp_secondary_colors, bvs.size() );

    // Debugging output
    // LOG_DBG << "secondary colors: " << bvs.size();
    // LOG_DBG << indent( bit_string_header( bvs[0].size() ), 8 );
    // for( size_t i = 0; i < bvs.size(); ++i ) {
    //     LOG_DBG << std::setw( 4 ) << i << "    " << to_bit_string( bvs[i], false );
    // }

    // Use the colors to construct a gamut
    auto gamut = KmerColorGamut( num_groups, std::move( bvs ));
    // LOG_DBG << print_kmer_color_gamut_summary( gamut );
    EXPECT_EQ( exp_total_colors, gamut.get_color_list_size() );
}

TEST( Taxonomy, KmerColors )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // We are testing with two types of files,
    // both made with the taxonomy grouping,
    // one being the full, and one being the trunk,
    // on a relatively recent NCBI taxonomy.

    // Expectation for both input files:
    // DBG  secondary colors: 1309
    // DBG  Elements:    512
    //      Colors:      1822
    //      Max colors:  1822
    //      Unique keys: 1818
    //      Gamut size:  0 x 0
    //      Gamut real:  0 (0.0%)
    //      Gamut imag:  0 (0.0%)
    //      Gamut empty: 0 (0.0%)

    test_kmer_color_taxonomy_(
        environment->data_dir + "taxonomy/grouped_taxonomy.json.gz",
        512,
        1309,
        1822
    );
    test_kmer_color_taxonomy_(
        environment->data_dir + "taxonomy/grouped_taxonomy_trunk.json.gz",
        512,
        1309,
        1822
    );
}

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
