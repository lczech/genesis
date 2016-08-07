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

#include "lib/sequence/formats/fasta_input_iterator.hpp"
#include "lib/sequence/formats/fasta_reader.hpp"
#include "lib/sequence/functions/codes.hpp"
#include "lib/sequence/functions/functions.hpp"
#include "lib/sequence/sequence_set.hpp"

#include "lib/utils/core/std.hpp"
#include "lib/utils/io/input_stream.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;

TEST( Sequence, FastaReaderValidating )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader()
        .valid_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 10, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "",                     sset[0].metadata() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

TEST( FastaInputIterator, ReadingLoop )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    utils::InputStream instr( utils::make_unique< utils::FileInputSource >( infile ));
    auto it = FastaInputIterator( instr );

    size_t len = 0;
    size_t cnt = 0;
    while( it ) {
        // Check first sequence.
        if( cnt == 0 ) {
            EXPECT_EQ( "Di106BGTue", it->label() );
        }

        len = std::max( len, it->length() );
        ++cnt;
        ++it;
    }

    EXPECT_EQ( 460, len );
    EXPECT_EQ( 10, cnt );
}

// TEST( FastaInputIterator, ReadingInput )
// {
//     // Skip test if no data availabe.
//     NEEDS_TEST_DATA;
//
//     std::string infile = environment->data_dir + "sequence/dna_10.fasta";
//     std::ifstream ifs (infile);
//
//     auto it = FastaInputIterator( ifs );
//     Sequence seq;
//
//     size_t len = 0;
//     size_t cnt = 0;
//
//     while( it >> seq ) {
//         // Check first sequence.
//         if( cnt == 0 ) {
//             EXPECT_EQ( "Di106BGTue", seq.label() );
//         }
//
//         len = std::max( len, seq.length() );
//         ++cnt;
//
//         if( cnt == 353 ) {
//             std::cout << seq.label() << "\n";
//             std::cout << ( it ? "si" : "no" ) << "\n";
//         }
//     }
//
//     std::cout << ( it ? "si" : "no" ) << "\n";
//
//     EXPECT_EQ( 460, len );
//     EXPECT_EQ( 10, cnt );
// }
