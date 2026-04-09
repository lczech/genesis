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

#include "genesis/sequence/format/fastq_reader.hpp"
#include "genesis/sequence/format/fastq_writer.hpp"
#include "genesis/sequence/format/fastx_input_stream.hpp"
#include "genesis/sequence/format/fastx_input_view_stream.hpp"
#include "genesis/sequence/format/fastx_output_stream.hpp"
#include "genesis/sequence/function/quality.hpp"
#include "genesis/sequence/sequence_set.hpp"

#include "genesis/util/core/fs.hpp"
#include "genesis/util/core/std.hpp"
#include "genesis/util/text/string.hpp"
#include "genesis/util/io/input_stream.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::util;
using namespace genesis::util::core;
using namespace genesis::util::io;
using namespace genesis::util::text;

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
        .read( from_file(infile), sset);

    // Check data.
    EXPECT_EQ( 2, sset.size() );
    EXPECT_EQ( "GATTTGGGGTTCAAAGCAGTATCGATCAAATAGTAAATCCATTTGTTCAACTCACAGTTT", sset[0].sites() );
    EXPECT_EQ( sset[0].size(), sset[0].phred_scores().size() );

    // Check quality scores. We just pick one of them here.
    // As this is an early one in the sequence, this also catches the AVX conversion
    EXPECT_EQ( 6, sset[0].phred_scores()[1] );

    // Now also check the full encoding. Because we can.
    auto const qual_0 = std::vector<unsigned char>{{
         0,  6,  6,  9,  7,  7,  7,  7,  9,  9,  9, 10,  8,  8,  4,  4,  4, 10, 10,  8,
         7,  4,  4,  4,  4,  8, 13, 16,  9,  9,  9, 12, 10,  9,  6,  6,  8,  8,  9,  9,
        20, 20, 34, 34, 37, 29, 29, 29, 29, 29, 29, 34, 34, 34, 34, 34, 34, 34, 21, 20
    }};
    auto const qual_1 = std::vector<unsigned char>{{
        68, 69, 66, 69, 69, 69, 69, 69, 66, 69, 68, 68, 69, 69, 69, 66, 69, 69, 69, 69,
        69, 69, 67, 67, 69, 63, 69, 66, 68, 69, 60, 63, 60, 62, 33, 64, 62, 61, 62, 62,
        58, 56, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 49, 51, 51, 59, 60, 60, 58, 60,
        67, 67, 67, 67, 63, 67, 67, 67, 61, 67, 67, 67, 64, 67, 67, 61, 33, 33, 33, 33,
        33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33
    }};

    EXPECT_EQ( qual_0, sset[0].phred_scores() );
    EXPECT_EQ( qual_1, sset[1].phred_scores() );
}

TEST( Sequence, FastqEncoding )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file. We use a different file, to have more testing variety.
    std::string infile = environment->data_dir + "sequence/SP1.fq";

    auto const enc = guess_fastq_quality_encoding( from_file( infile ));
    EXPECT_EQ( QualityEncoding::kSanger, enc );
}

TEST( Sequence, FastqInputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/SP1.fq";

    size_t cnt = 0;
    size_t sum_labels = 0;
    auto it = FastqInputStream( from_file( infile ));
    for( auto const& seq : it ) {
        // std::cout << "A " << cnt << " " << seq.length() << "\n";
        EXPECT_TRUE( seq.label().size() >= 21 || seq.label().size() <= 23 );
        EXPECT_EQ( 31, seq.sites().size() );
        EXPECT_TRUE( starts_with( seq.label(), "cluster_" ));
        ++cnt;
        sum_labels += seq.label().size();
    }
    // while( it ) {
    //     // std::cout << "A " << cnt << " " << it->length() << "\n";
    //     ++cnt;
    //     ++it;
    // }
    EXPECT_EQ( 250, cnt );
    EXPECT_EQ( 5471, sum_labels );

    // Again, to test resetting
    cnt = 0;
    for( auto const& s : FastqInputStream( from_file( infile )) ) {
        (void) s;
        // std::cout << "B " << cnt << " " << s.length() << "\n";
        ++cnt;
    }
    EXPECT_EQ( 250, cnt );
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

TEST( Sequence, FastqInputViewStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/SP1.fq";

    size_t cnt = 0;
    size_t sum_labels = 0;
    auto it = FastxInputViewStream( from_file( infile ));
    for( auto const& seq : it ) {
        EXPECT_TRUE( seq.label().size() >= 21 || seq.label().size() <= 23 );
        EXPECT_EQ( 31, seq.sites().size() );
        EXPECT_TRUE( starts_with( std::string( seq.label() ), "cluster_" ));
        ++cnt;
        sum_labels += seq.label().size();
    }
    EXPECT_EQ( 250, cnt );
    EXPECT_EQ( 5471, sum_labels );
}

#endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

TEST( Sequence, FastqWriter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/SP1.fq";
    auto const sset = FastqReader().read( from_file( infile ));

    // Write back into a string.
    std::string written;
    FastqWriter().write( sset, genesis::util::io::to_string( written ));

    // Compare to raw file data.
    auto const data = file_read( infile );
    EXPECT_EQ( data, written );
}

TEST( Sequence, FastqOutputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/SP1.fq";
    auto const sset = FastqReader().read( from_file( infile ));

    // Write to string. Need scope to actually do the writing.
    std::string target;
    {
        auto out_it = FastqOutputStream( genesis::util::io::to_string( target ));
        for( auto const& seq : sset ) {
            out_it << seq;
        }
    }

    // Compare to raw file.
    auto const data = file_read( infile );
    EXPECT_EQ( data, target );
}
