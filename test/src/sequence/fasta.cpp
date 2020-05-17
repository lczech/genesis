/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/sequence/formats/fasta_input_iterator.hpp"
#include "genesis/sequence/formats/fasta_output_iterator.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/formats/fasta_writer.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/sequence_set.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/gzip_input_source.hpp"

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
        .read( utils::from_file(infile), sset);

    // Check data.
    EXPECT_EQ( 10, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

TEST( FastaInputIterator, ReadingLoop )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto it = FastaInputIterator( utils::from_file( infile ));

    size_t len = 0;
    size_t cnt = 0;
    while( it ) {
        // Check first sequence.
        if( cnt == 0 ) {
            EXPECT_EQ( "Di106BGTue", it->label() );
        }
        if( cnt == 9 ) {
            EXPECT_EQ( "GTCGTTCT", it->sites().substr(387, 8 ));
        }

        len = std::max( len, it->length() );
        ++cnt;
        ++it;
    }

    EXPECT_FALSE( it );
    EXPECT_EQ( 460, len );
    EXPECT_EQ( 10, cnt );
}

TEST( FastaInputIterator, Advance )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto it = FastaInputIterator( utils::from_file( infile ));

    std::advance( it, 3 );

    size_t cnt = 0;
    while( it ) {
        std::advance( it, 2 );
        ++cnt;
        ++it;
    }

    EXPECT_FALSE( it );
    EXPECT_EQ( 3, cnt );

    // Fails assertion, as expected for an input iterator.
    // The test is problematic because of the threaded reading routines. so we leave it out for now.
    // EXPECT_DEATH_IF_SUPPORTED( std::advance( it, -2 ), ".*" );
}

TEST( FastaInputIterator, RangeBased )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";

    size_t cnt = 0;
    for( auto const& s : FastaInputIterator( utils::from_file( infile ))) {
        (void) s;
        // std::cout << s.length() << "\n";
        ++cnt;
    }
    EXPECT_EQ( 10, cnt );
}

// TEST( FastaInputIterator, ReadingInput )
// {
//     // Skip test if no data availabe.
//     NEEDS_TEST_DATA;
//
//     std::string infile = environment->data_dir + "sequence/dna_10.fasta";
//     auto it = FastaInputIterator();
//     it.from_file( infile );
//     Sequence seq;
//
//     size_t len = 0;
//     size_t cnt = 0;
//
//     while( it ) {
//         seq = *it;
//
//         // Check first sequence.
//         if( cnt == 0 ) {
//             EXPECT_EQ( "Di106BGTue", seq.label() );
//         }
//
//         len = std::max( len, it->length() );
//         ++cnt;
//
//         if( cnt == 353 ) {
//             std::cout << seq.label() << "\n";
//             std::cout << ( it ? "si" : "no" ) << "\n";
//         }
//
//         ++it;
//     }
//
//     std::cout << ( it ? "si" : "no" ) << "\n";
//
//     EXPECT_EQ( 460, len );
//     EXPECT_EQ( 10, cnt );
// }

TEST( Sequence, FastaCompressed )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Get sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta.gz";
    utils::InputStream cit { utils::make_unique<utils::GzipInputSource>(
        utils::make_unique<utils::FileInputSource>( infile )
    )};

    // Read.
    SequenceSet sset;
    FastaReader().parse_document( cit, sset );

    // Check data.
    EXPECT_EQ( 10, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

TEST( Sequence, FastaGzip )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Get sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta.gz";

    // Read.
    auto const sset = FastaReader().read( utils::from_file( infile ));

    // Check data.
    EXPECT_EQ( 10, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

TEST( Sequence, FastaWriter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader()
        .valid_chars( nucleic_acid_codes_all() )
        .read( utils::from_file(infile), sset);

    // Check data.
    EXPECT_EQ( 10, sset.size() );

    std::string target;
    FastaWriter().line_length(50).write( sset, utils::to_string( target ));

    auto const read_again = utils::file_read( infile );
    EXPECT_FALSE( target.empty() );
    EXPECT_EQ( read_again, target );
}

TEST( Sequence, FastaOutputIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const sset = FastaReader().read( utils::from_file( infile ));

    // Write to string. Need scope to actually do the writing.
    std::string target;
    {
        auto out_it = FastaOutputIterator( utils::to_string( target ));
        out_it.writer().line_length( 50 );
        for( auto const& seq : sset ) {
            out_it << seq;
        }
    }

    // Compare to raw file.
    auto const data = utils::file_read( infile );
    EXPECT_EQ( data, target );
}
