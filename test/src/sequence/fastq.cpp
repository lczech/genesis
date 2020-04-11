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

#include "genesis/sequence/formats/fastq_reader.hpp"
#include "genesis/sequence/functions/quality.hpp"
#include "genesis/sequence/sequence_set.hpp"

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;

TEST( Sequence, FastqReader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/wiki.fastq";
    SequenceSet sset;
    FastqReader()
        // .quality_string_plugin(
        //     []( std::string const& quality_string, Sequence& sequence ){
        //         sequence.phred_scores( quality_decode_to_phred_score( quality_string ));
        //     }
        // )
        .read( utils::from_file(infile), sset);

    // Check data.
    EXPECT_EQ( 2, sset.size() );
    EXPECT_EQ( "GATTTGGGGTTCAAAGCAGTATCGATCAAATAGTAAATCCATTTGTTCAACTCACAGTTT", sset[0].sites() );
    EXPECT_EQ( sset[0].size(), sset[0].phred_scores().size() );

    // Check quality scores. We just pick one of them here.
    EXPECT_EQ( 6, sset[0].phred_scores()[1] );
}

TEST( Sequence, FastqEncoding )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/SP1.fq";

    auto const enc = guess_fastq_quality_encoding( utils::from_file( infile ));
    EXPECT_EQ( QualityEncoding::kSanger, enc );
}
