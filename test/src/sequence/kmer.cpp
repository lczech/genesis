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
#include "genesis/sequence/kmer/canonical_encoding.hpp"
#include "genesis/sequence/kmer/extractor.hpp"
#include "genesis/sequence/kmer/function.hpp"
#include "genesis/sequence/kmer/microvariant_scanner.hpp"
#include "genesis/utils/math/random.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::utils;

template<typename Tag>
Kmer<Tag> make_random_kmer()
{
    using Bitfield = typename Kmer<Tag>::Bitfield;
    using WordType = typename Bitfield::WordType;
    static_assert( Bitfield::BIT_WIDTH == 64, "Bitfield::BIT_WIDTH != 64" );

    // We want to create a random kmer, which in the base implementation uses a 64bit word.
    // However, our fast number generator only generates 32bit words. We could use the standard
    // random distributions of C++ here of course, but that would be boring, so let's stitch
    // two random words together instead. Then, remove the unneded part high bits.
    WordType const h = permuted_congruential_generator();
    WordType const l = permuted_congruential_generator();
    WordType const w = ( h << 32 ) | l;
    WordType const s = w & Bitfield::ones_mask[ Kmer<Tag>::k() ];
    auto const k = Kmer<Tag>( s );
    validate( k );
    return k;
}

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

    // GATACAC = 0b 10 00 11 00 01 00 01 = 0x2311
    auto const k1 = Kmer<KmerTagDefault>( 0x2311 );
    auto const k2 = reverse_complement( k1 );

    // LOG_DBG << "k1 " << k1 << " from " << k1.value;
    // LOG_DBG << "k2 " << k2 << " from " << k2.value;

    // Basic string operators
    EXPECT_EQ( k1, kmer_from_string<KmerTagDefault>( "GATACAC" ));
    EXPECT_EQ( k2, kmer_from_string<KmerTagDefault>( "GTGTATC" ));
    EXPECT_EQ( "GATACAC", kmer_to_string( k1 ));
    EXPECT_EQ( "GTGTATC", kmer_to_string( k2 ));
    // LOG_DBG << "GATACAC: " << kmer_bits_to_string( k1 );
    // LOG_DBG << "GTGTATC: " << kmer_bits_to_string( k2 );

    // Canonical
    EXPECT_EQ( "GATACAC", kmer_to_string( canonical_representation( k1 )));
    EXPECT_EQ( "GATACAC", kmer_to_string( canonical_representation( k2 )));
}

TEST( Kmer, Lengths )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    using Alphabet = Kmer<KmerTagDefault>::Alphabet;
    using Bitfield = Kmer<KmerTagDefault>::Bitfield;

    for( size_t k = 1; k <= Bitfield::MAX_CHARS_PER_KMER; ++k ) {
        Kmer<KmerTagDefault>::reset_k( k );
        auto seq = make_random_kmer_sequence( k );
        auto kmer = kmer_from_string<KmerTagDefault>( seq );
        EXPECT_TRUE( validate( kmer ));
        EXPECT_EQ( kmer_to_string( kmer ), seq );
        for( size_t i = 0; i < k; ++i ) {
            EXPECT_EQ( Alphabet::rank_to_char( kmer[i] ), seq[i] );
        }
    }
}

TEST( Kmer, CanonicalRepresentation )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // We make the canonical representation based on the high bits, so that's kinda reverse.
    // Test that we still get what we expect. Needs to be the case, as we are comparing a string
    // to its reverse-ish, so the direction of comparison should not matter.

    auto run_test_ = []( size_t v )
    {
        auto const km = Kmer<KmerTagDefault>( v );
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
        auto const kms = kmer_to_string( km );
        auto const rcs = kmer_to_string( rc );
        auto const crs = kms < rcs ? kms : rcs;
        EXPECT_EQ( crs, kmer_to_string( canonical_representation( km )));
        EXPECT_EQ( crs, kmer_to_string( canonical_representation( rc )));

        // Test that the rc is correct
        EXPECT_EQ( rcs, string_rev_comp( kms ));
    };

    // Test all small k-mers
    for( size_t k = 1; k < 10; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {
            run_test_( i );
        }
    }

    // Also test a few random large k-mers to test the boundaries
    for( size_t k = 31; k <= 32; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < 10000; ++i ) {
            auto const km = make_random_kmer<KmerTagDefault>();
            run_test_( km.value );
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

    auto run_test_ = []()
    {
        auto const k = Kmer<KmerTagDefault>::k();
        for( size_t i = 0; i < 500; ++i ) {
            auto const sequence = make_random_kmer_sequence( i );

            // LOG_DBG << "==================================";
            // LOG_DBG << "  at " << i << ": " << sequence;

            // Run the kmer extractor
            size_t start_loc = 0;
            auto extractor = KmerExtractor<KmerTagDefault>( sequence );
            for( auto it = extractor.begin(); it != extractor.end(); ++it ) {
                // LOG_DBG1 << kmer_to_string( *it ) << " vs " << sequence.substr( start_loc, k );
                // LOG_DBG << kmer_bits_to_string( *it );

                // Basic tests of the location and characters at that location.
                EXPECT_EQ( it->location, start_loc );
                EXPECT_LE( it->location + k, sequence.size() );
                EXPECT_EQ( kmer_to_string( *it ), sequence.substr( start_loc, k ));
                EXPECT_TRUE( validate( *it ));

                // Test that the rc was set correctly by the extractor.
                EXPECT_EQ( it->rev_comp, reverse_complement( Kmer<KmerTagDefault>( it->value )).value );
                ++start_loc;
            }

            // Expect correct num of iterations
            EXPECT_EQ( extractor.count_valid_characters(), sequence.size() );
            EXPECT_EQ( extractor.count_invalid_characters(), 0 );
            if( sequence.size() >= k ) {
                EXPECT_EQ( start_loc, sequence.size() - k + 1 );
            } else {
                EXPECT_EQ( start_loc, 0 );
            }
        }
    };

    // Test some random strings for all k-mer sizes
    for( size_t k = 1; k <= 32; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        run_test_();
    }
}

TEST( Kmer, ExtractorInvalids )
{
    using Alphabet = Kmer<KmerTagDefault>::Alphabet;

    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    auto run_test_ = []()
    {
        auto const k = Kmer<KmerTagDefault>::k();
        for( size_t i = 1; i < 500; ++i ) {
            auto sequence = make_random_kmer_sequence( i );

            // Replace some random characters to invalid values.
            // We set up to the number of chars, but as we draw with replacement,
            // effectively we almost never set all of them to invalid chars.
            auto const num_inv = permuted_congruential_generator( 0, i );
            for( size_t j = 0; j < num_inv; ++j ) {
                auto const pos = permuted_congruential_generator( 0, sequence.size() - 1 );
                sequence[pos] = 'N';
            }
            auto const invalid_size = std::count( sequence.begin(), sequence.end(), 'N' );
            auto const valid_size = sequence.size() - invalid_size;

            // LOG_DBG << "--------------";
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
                EXPECT_EQ( kmer_to_string( *cur ), kstr );
                EXPECT_TRUE( validate( *cur ));

                // Test that the rc was set correctly by the extractor.
                EXPECT_EQ(
                    cur->rev_comp,
                    reverse_complement( Kmer<KmerTagDefault>( cur->value )).value
                );

                // Now move both to the next location
                ++start_loc;
                ++cur;
            }

            // Now we are done with the kmer extractor as well.
            EXPECT_EQ( cur, end );
            EXPECT_EQ( extractor.count_valid_characters(), valid_size );
            EXPECT_EQ( extractor.count_invalid_characters(), invalid_size );
            EXPECT_EQ(
                extractor.count_valid_characters() + extractor.count_invalid_characters(),
                sequence.size()
            );
            if( sequence.size() < k ) {
                EXPECT_EQ( start_loc, 0 );
            }
        }
    };

    // Test some random strings for all k-mer sizes
    for( size_t k = 1; k <= 32; ++k ) {
        // LOG_DBG << "########## at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        run_test_();
    }
}

// TEST( Kmer, ExtractorSpeed )
// {
//     // Random seed. Report it, so that in an error case, we can reproduce.
//     auto const seed = ::time(nullptr);
//     permuted_congruential_generator_init( seed );
//     LOG_INFO << "Seed: " << seed;
//
//     // We make one long sequence for the testing
//     LOG_TIME << "make sequence";
//     auto const sequence = make_random_kmer_sequence( 500000000 );
//     LOG_TIME << "done";
//
//     auto const k = 15;
//     Kmer<KmerTagDefault>::reset_k( k );
//     auto extractor = KmerExtractor<KmerTagDefault>( sequence );
//
//     size_t cnt = 0;
//     LOG_TIME << "extract kmers";
//     for( auto const& kmer : extractor ) {
//         (void) kmer;
//         ++cnt;
//     }
//     LOG_TIME << "done " << cnt;
//
//     // Expect correct num of iterations
//     EXPECT_EQ( extractor.count_valid_characters(), sequence.size() );
//     EXPECT_EQ( extractor.count_invalid_characters(), 0 );
//     EXPECT_EQ( cnt, sequence.size() - k + 1 );
// }

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

    auto run_test_ = [&]( size_t v )
    {
        auto const k = Kmer<KmerTagDefault>::k();

        // Make the kmer
        auto km = Kmer<KmerTagDefault>( v );
        set_reverse_complement( km );
        auto const kms = kmer_to_string( km );
        auto const rcs = kmer_to_string( reverse_complement( km ));

        // Go through all microvariants of the kmer,
        // and test that they have edit distance 1 to the original.
        size_t cnt = 0;
        for( auto const& mv : iterate_microvariants( km )) {
            // LOG_DBG << mv;
            auto const ekm = count_mismatches_( kms, kmer_to_string( mv ));
            auto const erv = count_mismatches_( rcs, kmer_to_string( reverse_complement( mv )));

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

        // We have 3 variants per position, plus the original kmer.
        EXPECT_EQ( cnt, 1 + 3 * k );
    };

    // Test all small k-mers
    for( size_t k = 1; k < 10; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {
            run_test_( i );
        }
    }

    // Also test a few random large k-mers to test the boundaries
    for( size_t k = 31; k <= 32; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < 10000; ++i ) {
            auto const km = make_random_kmer<KmerTagDefault>();
            run_test_( km.value );
        }
    }
}

// =================================================================================================
//     Minimal Canonical Encoding
// =================================================================================================

TEST( Kmer, NumCanonicalKmers )
{
    // We here test against a hard coded fixed table, which we could instead just use that table
    // to return the values in the function being tested... But well, seems cleaner to implement
    // the function as an actual computation following the original equation.
    auto const exp = std::vector<uint64_t>{{
        0, 2, 10, 32, 136, 512, 2080, 8192, 32896, 131072, 524800, 2097152, 8390656, 33554432,
        134225920, 536870912, 2147516416, 8589934592, 34359869440, 137438953472, 549756338176,
        2199023255552, 8796095119360, 35184372088832, 140737496743936, 562949953421312,
        2251799847239680, 9007199254740992, 36028797153181696, 144115188075855872,
        576460752840294400, 2305843009213693952, 9223372039002259456ULL
    }};

    for( size_t k = 1; k <= 32; ++k ) {
        EXPECT_EQ( exp[k], number_of_canonical_kmers( k ));
    }
}

TEST( Kmer, CanonicalEncoding )
{
    // Test several different lengths of kmers
    // for( size_t k = 1; k < 10; ++k ) {
    for( size_t k = 1; k < 12; ++k ) {
        // LOG_DBG << "=======================================================================";
        // LOG_DBG << "at " << k;

        Kmer<KmerTagDefault>::reset_k( k );
        auto const num_canon_kmers = number_of_canonical_kmers( k );
        auto const num_palindromes = number_of_palindromes( k );

        // We count all kmers of the given k, and see
        // if they evenly fill an array of all possible indices.
        auto counts = std::vector<size_t>( num_canon_kmers, 0 );

        // Test all kmers of that length
        auto encoder = MinimalCanonicalEncoding<KmerTagDefault>();
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {
            // Make the kmer
            auto km = Kmer<KmerTagDefault>( i );
            set_reverse_complement( km );

            // Get its index
            auto const index = encoder.encode( km );
            // LOG_DBG1 << kmer_to_string(km) << "\t" << index;

            // The index needs to match the one of the reverse complement
            EXPECT_EQ( index, encoder.encode( reverse_complement( km )));

            // Increment the count of that index, checking that we are in bounds.
            ASSERT_LT( index, num_canon_kmers );
            ++counts[index];
        }

        // Test that all bins got the number of kmers that we expect.
        EXPECT_EQ( counts.size(), num_canon_kmers );
        size_t cnt = 0;
        for( size_t i = 0; i < num_canon_kmers; ++i ) {
            // LOG_DBG1 << i << ": " << counts[i];

            // For palindromes: the first 4^(k/2)/2 entries are only set once.
            if( k % 2 == 0 && i < num_palindromes ) {
                EXPECT_EQ( 1, counts[i] );
            } else {
                EXPECT_EQ( 2, counts[i] );
            }
            cnt += counts[i];
        }
        EXPECT_EQ( cnt, number_of_kmers( k ));
    }
}

TEST( Kmer, CanonicalEncodingLarge )
{
    // Test large sizes of k for the boundaries.
    // Here, we cannot enumerate all values, so we just test a few properties.
    for( size_t k = 31; k <= 32; ++k ) {
        Kmer<KmerTagDefault>::reset_k( k );
        // LOG_DBG << "=======================================================================";
        // LOG_DBG << "at " << k;

        // Test all kmers of that length
        auto encoder = MinimalCanonicalEncoding<KmerTagDefault>();
        for( size_t i = 0; i < 100000; ++i ) {
            // LOG_DBG << "------------------------";

            // Make a random kmer
            auto km = make_random_kmer<KmerTagDefault>();
            set_reverse_complement( km );
            // LOG_DBG << km;

            // The index needs to match the one of the reverse complement
            auto const index = encoder.encode( km );
            EXPECT_EQ( index, encoder.encode( reverse_complement( km )));
        }
    }
}

// TEST( Kmer, CanonicalEncodingSpeed1 )
// {
//     int const k = 13;
//     Kmer<KmerTagDefault>::reset_k( k );
//
//     // Test all kmers of that length
//     auto encoder = MinimalCanonicalEncoding<KmerTagDefault>();
//     for( size_t i = 0; i < number_of_kmers( k ); ++i ) {
//
//         // Make the kmer
//         auto km = Kmer<KmerTagDefault>( i );
//         set_reverse_complement( km );
//         // EXPECT_EQ( km.rev_comp, reverse_complement( km ));
//
//         // Get its index
//         auto const index = encoder.encode( km );
//         // LOG_DBG << kmer_to_string(km) << "\t" << index;
//
//         // The index needs to match the one of the reverse complement
//         EXPECT_EQ( index, encoder.encode( reverse_complement( km )));
//     }
//     LOG_DBG << "num kmers " << number_of_kmers( k );
// }


// TEST( Kmer, CanonicalEncodingSpeed2 )
// {
//     // Random seed. Report it, so that in an error case, we can reproduce.
//     auto const seed = ::time(nullptr);
//     permuted_congruential_generator_init( seed );
//     LOG_INFO << "Seed: " << seed;
//
//     // We make one long sequence for the testing
//     LOG_TIME << "make sequence";
//     auto const sequence = make_random_kmer_sequence( 500000000 );
//     LOG_TIME << "done";
//
//     auto const k = 15;
//     Kmer<KmerTagDefault>::reset_k( k );
//     auto extractor = KmerExtractor<KmerTagDefault>( sequence );
//     auto encoder = MinimalCanonicalEncoding<KmerTagDefault>();
//
//     size_t cnt = 0;
//     LOG_TIME << "extract kmers";
//     for( auto const& kmer : extractor ) {
//         (void) kmer;
//         ++cnt;
//     }
//     LOG_TIME << "done " << cnt;
//
//     extractor = KmerExtractor<KmerTagDefault>( sequence );
//     size_t sum = 0;
//     LOG_TIME << "extract kmers and compute canonical index";
//     for( auto const& kmer : extractor ) {
//         auto const index = encoder.encode( kmer );
//         sum += index;
//     }
//     LOG_TIME << "done " << sum;
//
//     // Expect correct num of iterations
//     EXPECT_EQ( extractor.count_valid_characters(), sequence.size() );
//     EXPECT_EQ( extractor.count_invalid_characters(), 0 );
//     EXPECT_EQ( cnt, sequence.size() - k + 1 );
// }
