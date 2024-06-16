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

#include "genesis/sequence/functions/quality.hpp"
#include "genesis/utils/math/random.hpp"

#include <numeric>
#include <string>
#include <vector>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::utils;

// Fixed strings of all chars in the correct ASCII order for all encodings
std::string const sanger = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI";
std::string const illumina13 = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgh";
std::string const illumina15 = "BCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghi";
std::string const illumina18 = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJ";
std::string const solexa = ";<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgh";

TEST( Sequence, QualityDecodingChar )
{
    for( size_t i = 0; i < sanger.size(); ++i ) {
        EXPECT_EQ( i, quality_decode_to_phred_score( sanger[i], QualityEncoding::kSanger ));
    }
    for( size_t i = 0; i < illumina13.size(); ++i ) {
        EXPECT_EQ( i, quality_decode_to_phred_score( illumina13[i], QualityEncoding::kIllumina13 ));
    }

    // For Illumina 1.5, we start at 2, because the first two letters are not used.
    for( size_t i = 0; i < illumina15.size(); ++i ) {
        EXPECT_EQ( i, quality_decode_to_phred_score( illumina15[i], QualityEncoding::kIllumina15 ) - 2);
    }
    for( size_t i = 0; i < illumina18.size(); ++i ) {
        EXPECT_EQ( i, quality_decode_to_phred_score( illumina18[i], QualityEncoding::kIllumina18 ));
    }

    // For solexa, the first 15 (range -5 t0 10) scores do not match with phred,
    // so we do them manually...
    EXPECT_EQ( 1, quality_decode_to_phred_score( solexa[0], QualityEncoding::kSolexa ));
    EXPECT_EQ( 1, quality_decode_to_phred_score( solexa[1], QualityEncoding::kSolexa ));
    EXPECT_EQ( 2, quality_decode_to_phred_score( solexa[2], QualityEncoding::kSolexa ));
    EXPECT_EQ( 2, quality_decode_to_phred_score( solexa[3], QualityEncoding::kSolexa ));
    EXPECT_EQ( 3, quality_decode_to_phred_score( solexa[4], QualityEncoding::kSolexa ));
    EXPECT_EQ( 3, quality_decode_to_phred_score( solexa[5], QualityEncoding::kSolexa ));
    EXPECT_EQ( 4, quality_decode_to_phred_score( solexa[6], QualityEncoding::kSolexa ));
    EXPECT_EQ( 4, quality_decode_to_phred_score( solexa[7], QualityEncoding::kSolexa ));
    EXPECT_EQ( 5, quality_decode_to_phred_score( solexa[8], QualityEncoding::kSolexa ));
    EXPECT_EQ( 5, quality_decode_to_phred_score( solexa[9], QualityEncoding::kSolexa ));
    EXPECT_EQ( 6, quality_decode_to_phred_score( solexa[10], QualityEncoding::kSolexa ));
    EXPECT_EQ( 7, quality_decode_to_phred_score( solexa[11], QualityEncoding::kSolexa ));
    EXPECT_EQ( 8, quality_decode_to_phred_score( solexa[12], QualityEncoding::kSolexa ));
    EXPECT_EQ( 9, quality_decode_to_phred_score( solexa[13], QualityEncoding::kSolexa ));
    EXPECT_EQ( 10, quality_decode_to_phred_score( solexa[14], QualityEncoding::kSolexa ));
    for( size_t i = 15; i < solexa.size(); ++i ) {
        EXPECT_EQ( i, quality_decode_to_phred_score( solexa[i], QualityEncoding::kSolexa ) + 5 );
    }
}

TEST( Sequence, QualityDecodingString )
{
    auto sanger_v = std::vector<unsigned char>( sanger.size() );
    std::iota( sanger_v.begin(), sanger_v.end(), 0 );
    EXPECT_EQ( sanger_v, quality_decode_to_phred_score( sanger, QualityEncoding::kSanger ));

    auto illumina13_v = std::vector<unsigned char>( illumina13.size() );
    std::iota( illumina13_v.begin(), illumina13_v.end(), 0 );
    EXPECT_EQ( illumina13_v, quality_decode_to_phred_score( illumina13, QualityEncoding::kIllumina13 ));

    // For Illumina 1.5, we start at 2, because the first two letters are not used.
    auto illumina15_v = std::vector<unsigned char>( illumina15.size() );
    std::iota( illumina15_v.begin(), illumina15_v.end(), static_cast<unsigned char>(2) );
    EXPECT_EQ( illumina15_v, quality_decode_to_phred_score( illumina15, QualityEncoding::kIllumina15 ));

    auto illumina18_v = std::vector<unsigned char>( illumina18.size() );
    std::iota( illumina18_v.begin(), illumina18_v.end(), 0 );
    EXPECT_EQ( illumina18_v, quality_decode_to_phred_score( illumina18, QualityEncoding::kIllumina18 ));

    // Again solexa has to be filled manually, as the first 15 chars do not match phred.
    auto solexa_v = std::vector<unsigned char>( solexa.size() );
    solexa_v[0] = 1;
    solexa_v[1] = 1;
    solexa_v[2] = 2;
    solexa_v[3] = 2;
    solexa_v[4] = 3;
    solexa_v[5] = 3;
    solexa_v[6] = 4;
    solexa_v[7] = 4;
    solexa_v[8] = 5;
    solexa_v[9] = 5;
    solexa_v[10] = 6;
    solexa_v[11] = 7;
    solexa_v[12] = 8;
    solexa_v[13] = 9;
    solexa_v[14] = 10;
    std::iota( solexa_v.begin() + 15, solexa_v.end(), 10 );
    EXPECT_EQ( solexa_v, quality_decode_to_phred_score( solexa, QualityEncoding::kSolexa ));
}

TEST( Sequence, QualityEncodingString )
{
    auto scores = std::vector<unsigned char>{ 1, 5, 10, 20, 50, 90};
    EXPECT_EQ( scores, quality_decode_to_phred_score( quality_encode_from_phred_score( scores )));

    std::string codes = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI";
    EXPECT_EQ( codes, quality_encode_from_phred_score( quality_decode_to_phred_score( codes )));
}

TEST( Sequence, QualityComputations )
{
    // Convert between probabilities and scores
    for( int i = -5; i < 50; ++i ) {
        EXPECT_EQ( i, error_probability_to_solexa_score( solexa_score_to_error_probability( i )));
    }
    for( int i = 0; i < 50; ++i ) {
        EXPECT_EQ( i, error_probability_to_phred_score( phred_score_to_error_probability( i )));
    }

    // Convert between scores, for all larger values. They all map identical, so we simply
    // test by going back and forth between them.
    for( size_t i = 10; i < 50; ++i ) {
        EXPECT_EQ( i, phred_score_to_solexa_score( solexa_score_to_phred_score( i )));
        EXPECT_EQ( i, solexa_score_to_phred_score( phred_score_to_solexa_score( i )));
    }

    // Smaller values do not have a 1-to-1 mapping, so test manually.

    EXPECT_EQ( 1, solexa_score_to_phred_score( -5 ));
    EXPECT_EQ( 1, solexa_score_to_phred_score( -4 ));
    EXPECT_EQ( 2, solexa_score_to_phred_score( -3 ));
    EXPECT_EQ( 2, solexa_score_to_phred_score( -2 ));
    EXPECT_EQ( 3, solexa_score_to_phred_score( -1 ));
    EXPECT_EQ( 3, solexa_score_to_phred_score( 0 ));
    EXPECT_EQ( 4, solexa_score_to_phred_score( 1 ));
    EXPECT_EQ( 4, solexa_score_to_phred_score( 2 ));
    EXPECT_EQ( 5, solexa_score_to_phred_score( 3 ));
    EXPECT_EQ( 5, solexa_score_to_phred_score( 4 ));
    EXPECT_EQ( 6, solexa_score_to_phred_score( 5 ));
    EXPECT_EQ( 7, solexa_score_to_phred_score( 6 ));
    EXPECT_EQ( 8, solexa_score_to_phred_score( 7 ));
    EXPECT_EQ( 9, solexa_score_to_phred_score( 8 ));
    EXPECT_EQ( 10, solexa_score_to_phred_score( 9 ));
    EXPECT_EQ( 10, solexa_score_to_phred_score( 10 ));

    EXPECT_EQ( -5, phred_score_to_solexa_score( 0 ));
    EXPECT_EQ( -5, phred_score_to_solexa_score( 1 ));
    EXPECT_EQ( -2, phred_score_to_solexa_score( 2 ));
    EXPECT_EQ( 0, phred_score_to_solexa_score( 3 ));
    EXPECT_EQ( 2, phred_score_to_solexa_score( 4 ));
    EXPECT_EQ( 3, phred_score_to_solexa_score( 5 ));
    EXPECT_EQ( 5, phred_score_to_solexa_score( 6 ));
    EXPECT_EQ( 6, phred_score_to_solexa_score( 7 ));
    EXPECT_EQ( 7, phred_score_to_solexa_score( 8 ));
    EXPECT_EQ( 8, phred_score_to_solexa_score( 9 ));
    EXPECT_EQ( 10, phred_score_to_solexa_score( 10 ));
}

TEST( Sequence, QualityEncodingNames )
{
    // Basics: Back and forth.
    EXPECT_EQ(
        QualityEncoding::kSanger,
        guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding::kSanger ))
    );
    EXPECT_EQ(
        QualityEncoding::kIllumina13,
        guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding::kIllumina13 ))
    );
    EXPECT_EQ(
        QualityEncoding::kIllumina15,
        guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding::kIllumina15 ))
    );
    EXPECT_EQ(
        QualityEncoding::kIllumina18,
        guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding::kIllumina18 ))
    );
    EXPECT_EQ(
        QualityEncoding::kSolexa,
        guess_quality_encoding_from_name( quality_encoding_name( QualityEncoding::kSolexa ))
    );

    // Special and error cases.
    EXPECT_EQ(
        QualityEncoding::kIllumina18,
        guess_quality_encoding_from_name( " Illumina." )
    );
    EXPECT_ANY_THROW(
        guess_quality_encoding_from_name( " IlluminiaX" )
    );
    EXPECT_ANY_THROW(
        guess_quality_encoding_from_name( "" )
    );

    // Cases as might be used by a command line interface.
    EXPECT_EQ( QualityEncoding::kSanger,     guess_quality_encoding_from_name( "sanger" ));
    EXPECT_EQ( QualityEncoding::kSolexa,     guess_quality_encoding_from_name( "solexa" ));
    EXPECT_EQ( QualityEncoding::kIllumina13, guess_quality_encoding_from_name( "illumina-1.3" ));
    EXPECT_EQ( QualityEncoding::kIllumina15, guess_quality_encoding_from_name( "illumina-1.5" ));
    EXPECT_EQ( QualityEncoding::kIllumina18, guess_quality_encoding_from_name( "illumina-1.8" ));
}

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

void test_quality_decoding_fuzzy_()
{
    // Fill a vector of random length with random valid phred scores.
    auto const len = permuted_congruential_generator( 1, 1000 );
    auto phred_scores = std::vector<unsigned char>( len );
    for( size_t i = 0; i < len; ++i ) {
        phred_scores[i] = permuted_congruential_generator( 0, 93 );
    }

    // Encode the scores. We trust this function.
    auto const encoded = quality_encode_from_phred_score( phred_scores );

    // Now decode again and check that we have the same as before.
    EXPECT_EQ( phred_scores, quality_decode_to_phred_score( encoded ));
}

TEST( Sequence, QualityDecodingFuzzy )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 1000;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        test_quality_decoding_fuzzy_();
    }
}
