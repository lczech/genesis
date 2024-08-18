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

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/sequence/kmer/extractor.hpp"
#include "genesis/sequence/kmer/function.hpp"
#include "genesis/sequence/kmer/microvariant_scanner.hpp"
#include "genesis/utils/math/random.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::utils;

std::string make_random_kmer_sequence( size_t length )
{
    using Alphabet = Kmer<KmerTagDefault>::Alphabet;

    // Make a string of length i with random valid chars.
    auto sequence = std::string( length, '-' );
    for( size_t i = 0; i < length; ++i ) {
        sequence[i] = Alphabet::rank_to_char(
            permuted_congruential_generator( Alphabet::MAX_RANK )
        );
    }
    EXPECT_EQ( length, sequence.size() );
    return sequence;
}

// Hand-crafted reverse complement on a string
std::string string_rev_comp( std::string const& str )
{
    auto res = std::string( str.size(), '#' );
    for( size_t i = 0; i < str.size(); ++i ) {
        char rc = '-';
        switch( str[i] ) {
            case 'A': {
                rc = 'T';
                break;
            }
            case 'C': {
                rc = 'G';
                break;
            }
            case 'G': {
                rc = 'C';
                break;
            }
            case 'T': {
                rc = 'A';
                break;
            }
            default: {
                rc = 'X';
            }
        }
        res[ str.size() - i - 1 ] = rc;
    }
    return res;
}

// =================================================================================================
//     Basics
// =================================================================================================

TEST( Kmer, Basics )
{
    Kmer<KmerTagDefault>::reset_k( 7 );

    // GATACAC = 0b 01 00 01 00 11 00 10 = 0x1132
    auto const k1 = Kmer<KmerTagDefault>( 0x1132 );
    auto const k2 = reverse_complement( k1 );

    // LOG_DBG << "k1 " << k1 << " from " << k1.value;
    // LOG_DBG << "k2 " << k2 << " from " << k2.value;

    // Basic string operators
    EXPECT_EQ( k1, kmer_from_string<KmerTagDefault>( "GATACAC" ));
    EXPECT_EQ( k2, kmer_from_string<KmerTagDefault>( "GTGTATC" ));
    EXPECT_EQ( "GATACAC", to_string( k1 ));
    EXPECT_EQ( "GTGTATC", to_string( k2 ));

    // Canonical
    EXPECT_EQ( "GATACAC", to_string( canonical_representation( k1 )));
    EXPECT_EQ( "GATACAC", to_string( canonical_representation( k2 )));
}

TEST( Kmer, Lengths )
{
    using Alphabet = Kmer<KmerTagDefault>::Alphabet;
    using Bitfield = Kmer<KmerTagDefault>::Bitfield;

    for( size_t k = 1; k <= Bitfield::MAX_CHARS_PER_KMER; ++k ) {
        Kmer<KmerTagDefault>::reset_k( k );
        auto seq = make_random_kmer_sequence( k );
        auto kmer = kmer_from_string<KmerTagDefault>( seq );
        EXPECT_TRUE( validate( kmer ));
        EXPECT_EQ( to_string( kmer ), seq );
        for( size_t i = 0; i < k; ++i ) {
            EXPECT_EQ( Alphabet::rank_to_char( kmer[i] ), seq[i] );
        }
    }
}

TEST( Kmer, CanonicalRepresentation )
{
    // We make the canonical representation based on the high bits, so that's kinda reverse.
    // Test that we still get what we expect. Needs to be the case, as we are comparing a string
    // to its reverse-ish, so the direction of comparison should not matter.

    for( size_t k = 1; k < 10; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {

            auto const km = Kmer<KmerTagDefault>( i );
            auto const rc = reverse_complement( km );
            // LOG_DBG1 << km << " <-> " << rc << " --> " << canonical_representation( km );
            EXPECT_EQ( 0, km.rev_comp );
            EXPECT_EQ( km.value, rc.rev_comp );
            EXPECT_TRUE( validate( km ));
            EXPECT_TRUE( validate( rc ));

            // Test that the canonical representation is the same for the kmer and its rc.
            EXPECT_EQ( canonical_representation( km ), canonical_representation( rc ));

            // Test that the rc of the rc is the original again.
            // We make a copy of the rc here, to ensure that we are testing the value.
            auto rcrc = reverse_complement( Kmer<KmerTagDefault>( rc.value ));
            EXPECT_EQ( rcrc.value, km.value );
            EXPECT_TRUE( validate( rcrc ));

            // Test that the canonical representation follows lexicographical ordering,
            // by actually sorting the string representation.
            auto const kms = to_string( km );
            auto const rcs = to_string( rc );
            auto const crs = kms < rcs ? kms : rcs;
            EXPECT_EQ( crs, to_string( canonical_representation( km )));
            EXPECT_EQ( crs, to_string( canonical_representation( rc )));

            // Test that the rc is correct
            EXPECT_EQ( rcs, string_rev_comp( kms ));
        }
    }
}

// =================================================================================================
//     Extractor
// =================================================================================================

TEST( Kmer, ExtractorBasics )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // Set the k for this test.
    Kmer<KmerTagDefault>::reset_k( 7 );
    auto const k = Kmer<KmerTagDefault>::k();

    for( size_t i = 0; i < 1000; ++i ) {
        auto const sequence = make_random_kmer_sequence( i );

        // LOG_DBG << "==================================";
        // LOG_DBG << "  at " << i << ": " << sequence;

        // Run the kmer extractor
        size_t start_loc = 0;
        auto extractor = KmerExtractor<KmerTagDefault>( sequence );
        for( auto it = extractor.begin(); it != extractor.end(); ++it ) {
            // LOG_DBG1 << to_string( *it );

            // Basic tests of the location and characters at that location.
            EXPECT_EQ( it->location, start_loc );
            EXPECT_LE( it->location + k, sequence.size() );
            EXPECT_EQ( to_string( *it ), sequence.substr( start_loc, k ));
            EXPECT_TRUE( validate( *it ));

            // Test that the rc was set correctly by the extractor.
            EXPECT_EQ( it->rev_comp, reverse_complement( Kmer<KmerTagDefault>( it->value )).value );
            ++start_loc;
        }

        // Expect correct num of iterations
        if( sequence.size() >= k ) {
            EXPECT_EQ( start_loc, sequence.size() - k + 1 );
        } else {
            EXPECT_EQ( start_loc, 0 );
        }
    }
}

TEST( Kmer, ExtractorInvalids )
{
    using Alphabet = Kmer<KmerTagDefault>::Alphabet;

    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // Set the k for this test.
    Kmer<KmerTagDefault>::reset_k( 7 );
    auto const k = Kmer<KmerTagDefault>::k();

    for( size_t i = k; i < 1000; ++i ) {
        auto sequence = make_random_kmer_sequence( i );

        // Replace some random characters to invalid values.
        // We set up to the number of chars, but as we draw with replacement,
        // effectively we almost never set all of them to invalid chars.
        auto const num_inv = permuted_congruential_generator( 0, i );
        for( size_t j = 0; j < num_inv; ++j ) {
            auto const pos = permuted_congruential_generator( 0, sequence.size() - 1 );
            sequence[pos] = 'N';
        }

        // LOG_DBG << "==================================";
        // LOG_DBG << "  at " << i << ": " << sequence;

        // Run the kmer extractor in sync with the sequence, where we need to skip over invalids.
        size_t start_loc = 0;
        auto extractor = KmerExtractor<KmerTagDefault>( sequence );
        auto cur = extractor.begin();
        auto end = extractor.end();
        while( start_loc + k <= sequence.size() ) {
            // Get the substr at the current location.
            // If it is invalid, go on to the next.
            auto const kstr = sequence.substr( start_loc, k );
            if( kstr.find_first_not_of( Alphabet::ALPHABET ) != std::string::npos ) {
                // LOG_DBG1 << kstr << " skip";
                ++start_loc;
                continue;
            }
            // LOG_DBG1 << kstr << " good";

            // Now we are at a valid kmer in the input. The extractor also needs to be valid here.
            ASSERT_NE( cur, end );

            // Basic tests of the location and character at that location.
            EXPECT_EQ( cur->location, start_loc );
            EXPECT_LE( cur->location + k, sequence.size() );
            EXPECT_EQ( to_string( *cur ), kstr );
            EXPECT_TRUE( validate( *cur ));

            // Test that the rc was set correctly by the extractor.
            EXPECT_EQ( cur->rev_comp, reverse_complement( Kmer<KmerTagDefault>( cur->value )).value );

            // Now move both to the next location
            ++start_loc;
            ++cur;
        }

        // Now we are done with the kmer extractor as well.
        EXPECT_EQ( cur, end );
    }
}

// =================================================================================================
//     Microvariants
// =================================================================================================

TEST( Kmer, MicrovariantScanner )
{
    auto count_mismatches_ = []( std::string const& str1, std::string const& str2 )
    {
        EXPECT_EQ( str1.length(), str2.length() );

        // Compare characters at each position in the strings
        size_t mismatch_cnt = 0;
        for( size_t i = 0; i < str1.length(); ++i ) {
            if( str1[i] != str2[i] ) {
                ++mismatch_cnt;
            }
        }
        return mismatch_cnt;
    };

    // Test several different lengths of kmers
    for( size_t k = 1; k < 10; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );

        // Test all kmers of that length
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {

            // Make the kmer
            auto km = Kmer<KmerTagDefault>( i );
            set_reverse_complement( km );
            auto const kms = to_string( km );
            auto const rcs = to_string( reverse_complement( km ));

            // Go through all microvariants of the kmer,
            // and test that they have edit distance 1 to the original.
            size_t cnt = 0;
            for( auto const& mv : iterate_microvariants( km )) {
                // LOG_DBG << kmer;
                auto const ekm = count_mismatches_( kms, to_string( mv ));
                auto const erv = count_mismatches_( rcs, to_string( reverse_complement( mv )));

                // Test that the rc was set correctly by the extractor.
                EXPECT_EQ(
                    mv.rev_comp,
                    reverse_complement( Kmer<KmerTagDefault>( mv.value )).value
                );
                EXPECT_TRUE( validate( mv ));

                // The first kmer is the original, so that has edit distance 0
                if( cnt == 0 ) {
                    EXPECT_EQ( 0, ekm );
                    EXPECT_EQ( 0, erv );
                } else {
                    EXPECT_EQ( 1, ekm );
                    EXPECT_EQ( 1, erv );
                }
                ++cnt;
            }
        }
    }
}
