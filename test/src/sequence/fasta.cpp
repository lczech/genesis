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

#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/formats/fasta_writer.hpp"
#include "genesis/sequence/formats/fastx_input_stream.hpp"
#include "genesis/sequence/formats/fastx_input_view_stream.hpp"
#include "genesis/sequence/formats/fastx_output_stream.hpp"
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

TEST( FastaInputStream, ReadingLoop )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto it = FastaInputStream( utils::from_file( infile ));

    size_t len = 0;
    size_t cnt = 0;
    for( auto const& seq : it ) {
        // Check first and last sequence, as examples.
        if( cnt == 0 ) {
            EXPECT_EQ( "Di106BGTue", seq.label() );
        }
        if( cnt == 9 ) {
            EXPECT_EQ( "GTCGTTCT", seq.sites().substr(387, 8 ));
        }

        len = std::max( len, seq.length() );
        ++cnt;
    }

    // EXPECT_FALSE( it );
    EXPECT_EQ( 460, len );
    EXPECT_EQ( 10, cnt );
}

// TEST( FastaInputStream, Advance )
// {
//     // Skip test if no data availabe.
//     NEEDS_TEST_DATA;
//
//     std::string infile = environment->data_dir + "sequence/dna_10.fasta";
//     auto stream = FastaInputStream( utils::from_file( infile ));
//     auto it = stream.begin();
//
//     std::advance( it, 3 );
//
//     size_t cnt = 0;
//     for( ; it != stream.end(); ++it ) {
//         std::advance( it, 2 );
//         ++cnt;
//     }
//
//     // EXPECT_FALSE( it );
//     EXPECT_EQ( 3, cnt );
//
//     // Fails assertion, as expected for an input iterator.
//     // The test is problematic because of the threaded reading routines. so we leave it out for now.
//     // EXPECT_DEATH_IF_SUPPORTED( std::advance( it, -2 ), ".*" );
// }

TEST( FastaInputStream, RangeBased )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";

    size_t cnt = 0;
    for( auto const& s : FastaInputStream( utils::from_file( infile ))) {
        (void) s;
        // std::cout << s.length() << "\n";
        ++cnt;
    }
    EXPECT_EQ( 10, cnt );
}

// TEST( FastaInputStream, ReadingInput )
// {
//     // Skip test if no data availabe.
//     NEEDS_TEST_DATA;
//
//     std::string infile = environment->data_dir + "sequence/dna_10.fasta";
//     auto it = FastaInputStream();
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

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

TEST( Sequence, FastaInputViewStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/dna_10_single.fasta";

    size_t cnt = 0;
    size_t sum_labels = 0;
    auto it = FastxInputViewStream( utils::from_file( infile ));
    for( auto const& seq : it ) {
        EXPECT_TRUE( seq.label().size() >= 10 || seq.label().size() <= 15 );
        EXPECT_EQ( 460, seq.sites().size() );
        ++cnt;
        sum_labels += seq.label().size();
    }
    EXPECT_EQ( 10, cnt );
    EXPECT_EQ( 112, sum_labels );
}

#endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

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

TEST( Sequence, FastaOutputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const sset = FastaReader().read( utils::from_file( infile ));

    // Write to string. Need scope to actually do the writing.
    std::string target;
    {
        auto out_it = FastaOutputStream( utils::to_string( target ));
        out_it.writer().line_length( 50 );
        for( auto const& seq : sset ) {
            out_it << seq;
        }
    }

    // Compare to raw file.
    auto const data = utils::file_read( infile );
    EXPECT_EQ( data, target );
}
