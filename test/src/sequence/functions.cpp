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

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/functions/stats.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( SequenceSet, Characteristics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_all() ));

    // Some basic assertments.
    EXPECT_EQ( 4600, total_length( sset ));
    EXPECT_TRUE( is_alignment( sset ));
    EXPECT_FLOAT_EQ( 0.15086956, gapyness( sset, nucleic_acid_codes_undetermined() ));

    // Check counting.
    auto const sh = site_histogram( sset );
    auto const bf = base_frequencies( sset, nucleic_acid_codes_plain() );

    // Site histogram.
    EXPECT_EQ( 694,  sh.at('-') );
    EXPECT_EQ( 786,  sh.at('A') );
    EXPECT_EQ( 1175, sh.at('C') );
    EXPECT_EQ( 1100, sh.at('G') );
    EXPECT_EQ( 845,  sh.at('T') );

    // Base frequencies.
    EXPECT_DOUBLE_EQ( 0.20122887864823349, bf.at('A') );
    EXPECT_DOUBLE_EQ( 0.30081925243215568, bf.at('C') );
    EXPECT_DOUBLE_EQ( 0.2816180235535074,  bf.at('G') );
    EXPECT_DOUBLE_EQ( 0.21633384536610342, bf.at('T') );
}
