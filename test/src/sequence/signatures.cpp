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

#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/functions/signatures.hpp"
#include "genesis/sequence/functions/signature_specifications.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <numeric>
#include <string>

using namespace genesis::sequence;
using namespace genesis::utils;

TEST( Sequence, KmerList )
{
    // Test up to kmer size of 6
    for( size_t k = 1; k < 6; ++k ) {
        auto list = SignatureSpecifications( "ACGT", k ).kmer_list();
        EXPECT_EQ( genesis::utils::int_pow( 4, k ), list.size() );
        EXPECT_EQ( std::string( k, 'A' ), list.front() );
        EXPECT_EQ( std::string( k, 'T' ), list.back() );

        // for( auto ll : list ) {
        //     LOG_DBG << ll;
        // }
    }
}

TEST( Sequence, KmerCounts )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    remove_all_gaps( sset );
    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_plain() ));

    auto const alphabet = std::string( "ACGT" );

    // Test up to kmer size of 6
    for( size_t k = 1; k < 6; ++k ) {
        auto const settings = SignatureSpecifications( alphabet, k );
        auto const list = settings.kmer_list();

        for( auto const& seq : sset ) {
            // LOG_DBG << "seq " << seq.sites();

            auto const kmers = signature_counts( seq, settings );
            ASSERT_EQ( list.size(), kmers.size() );

            for( size_t i = 0; i < kmers.size(); ++i ) {
                // LOG_DBG1 << i << "\t" << kmers[i];

                auto const count = count_substring_occurrences( seq.sites(), list[i] );
                EXPECT_EQ( count, kmers[i] );
            }

            // Check that the sum of counts fits to the sequence length.
            auto sum = std::accumulate( std::begin( kmers ), std::end( kmers ), 0 );
            EXPECT_EQ( seq.size() - k + 1, sum );

            // Check the same for symmetrized counts.
            auto const sym_kmers = signature_symmetrized_counts( seq, settings );
            sum = std::accumulate( std::begin( kmers ), std::end( kmers ), 0 );
            EXPECT_EQ( seq.size() - k + 1, sum );
        }
    }
}

TEST( Sequence, SignatureFrequencies )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    remove_all_gaps( sset );
    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_plain() ));

    for( size_t k = 1; k < 6; ++k ) {
        auto const settings = SignatureSpecifications( "ACGT", k );

        for( auto const& seq : sset ) {
            auto const freqs = signature_frequencies( seq, settings );
            auto const sum = std::accumulate( freqs.begin(), freqs.end(), 0.0 );
            EXPECT_FLOAT_EQ( 1.0, sum );

            auto const sym_freqs = signature_symmetrized_frequencies( seq, settings );
            auto const sym_sum = std::accumulate( sym_freqs.begin(), sym_freqs.end(), 0.0 );
            EXPECT_FLOAT_EQ( 1.0, sym_sum );
        }
    }
}

TEST( Sequence, KmerReverseComplements )
{
    // Test up to kmer size of 6
    for( size_t k = 1; k < 6; ++k ) {
        auto const settings = SignatureSpecifications( "ACGT", k );
        auto const list = settings.kmer_list();
        auto const rc_map = settings.kmer_combined_reverse_complement_map();
        ASSERT_EQ( list.size(), rc_map.size() );

        // Get the length needed to store rev comp entries,
        auto const revcom_size = settings.kmer_reverse_complement_list_size();

        for( size_t i = 0; i < list.size(); ++i ) {
            // LOG_DBG << list[i] << " <-- " << rc_map[i];

            auto const rev = reverse_complement( list[i] );

            // rev comp applied twice gives identical sequence.
            EXPECT_EQ( list[i], reverse_complement( rev ) );

            // Check that a seq and its rev map to the same index.
            auto const pos = std::distance(
                list.begin(), std::find( list.begin(), list.end(), rev )
            );
            EXPECT_LT( pos, list.size() );
            EXPECT_EQ( rc_map[i], rc_map[pos] );

            // Make sure that the indices are in range [0, revcom_size).
            EXPECT_LT( rc_map[i], revcom_size );

            // LOG_DBG1 << i << " " << pos << " -> " << rc_map[i];
        }

        // Get list of rev comp kmers.
        auto const& rc_list = settings.kmer_reverse_complement_list();
        // LOG_DBG << "k " << k << " with rev comp list size " << revcom_size;
        // for( auto e : rc_list ) {
        //     LOG_DBG1 << e;
        // }

        // Test whether the rev comp of each entry is either itself, or
        // not part of the list.
        for( size_t i = 0; i < rc_list.size(); ++i ) {
            auto const rev = reverse_complement( rc_list[i] );
            if( rev == rc_list[i] ) {
                continue;
            }
            auto match = std::find( rc_list.begin(), rc_list.end(), rev );
            EXPECT_EQ( match, rc_list.end() );
        }

        // Test whether the indicies are pointing to each other and are correct.
        auto const& rc_ids = settings.kmer_reverse_complement_indices();
        ASSERT_EQ( rc_ids.size(), list.size() );
        for( size_t i = 0; i < list.size(); ++i ) {

            // If we follow the index twice, we need to get back to the start.
            // This is also true for palindromes.
            EXPECT_EQ( i, rc_ids[ rc_ids[i] ] );

            // Test the actual index list.
            auto const rev = reverse_complement( list[i] );
            EXPECT_EQ( rev, list[ rc_ids[i] ] );
        }
    }
}

TEST( Sequence, KmerStringOverlapping )
{
    // // Skip test if no data availabe.
    // NEEDS_TEST_DATA;
    //
    // // Load sequence file.
    // std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    // SequenceSet sset;
    // FastaReader().from_file(infile, sset);
    //
    // remove_all_gaps( sset );
    // EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_plain() ));
    //
    // for( size_t k = 1; k < 6; ++k ) {
    //     auto const settings = SignatureSpecifications( "ACGT", k );
    //
    //     for( auto const& seq : sset ) {
    //         std::cout << seq.sites() << " with k " << k << "\n";
    //         kmer_string_overlapping( seq, settings, std::cout );
    //         std::cout << "\n";
    //         break;
    //     }
    // }

    auto const seq = Sequence( "label", "AAAACCCCGGGGTTTT" );
    std::vector<std::string> expected = {
        "A A A A C C C C G G G G T T T T",
        "AA AA AA AC CC CC CC CG GG GG GG GT TT TT TT",
        "AAA AAA AAC ACC CCC CCC CCG CGG GGG GGG GGT GTT TTT TTT",
        "AAAA AAAC AACC ACCC CCCC CCCG CCGG CGGG GGGG GGGT GGTT GTTT TTTT",
        "AAAAC AAACC AACCC ACCCC CCCCG CCCGG CCGGG CGGGG GGGGT GGGTT GGTTT GTTTT"
    };

    for( size_t k = 1; k < 6; ++k ) {
        auto const settings = SignatureSpecifications( "ACGT", k );
        auto const kmerstr  = kmer_string_overlapping( seq, settings );
        EXPECT_EQ( kmerstr, expected[k-1] );

        // std::cout << seq.sites() << " with k " << k << "\n";
        // kmer_string_overlapping( seq, settings, std::cout );
        // std::cout << "\n";
    }

    // Test edge case.
    auto const short_seq = Sequence( "label", "ACGT" );
    auto const short_set = SignatureSpecifications( "ACGT", 5 );
    auto const short_str  = kmer_string_overlapping( short_seq, short_set );
    EXPECT_EQ( short_str, "" );
}

TEST( Sequence, KmerStringNonOverlapping )
{
    auto const seq = Sequence( "label", "AAAACCCCGGGGTTTT" );
    std::vector<std::vector<std::string>> expected = {
        {
            "A A A A C C C C G G G G T T T T"
        }, {
            "AA AA CC CC GG GG TT TT",
            "AA AC CC CG GG GT TT"
        }, {
            "AAA ACC CCG GGG TTT",
            "AAA CCC CGG GGT TTT",
            "AAC CCC GGG GTT"
        }, {
            "AAAA CCCC GGGG TTTT",
            "AAAC CCCG GGGT",
            "AACC CCGG GGTT",
            "ACCC CGGG GTTT"
        }, {
            "AAAAC CCCGG GGTTT",
            "AAACC CCGGG GTTTT",
            "AACCC CGGGG",
            "ACCCC GGGGT",
            "CCCCG GGGTT"
        }
    };

    for( size_t k = 1; k < 6; ++k ) {
        auto const settings = SignatureSpecifications( "ACGT", k );
        auto const kmervec  = kmer_strings_non_overlapping( seq, settings );
        EXPECT_EQ( kmervec, expected[k-1] );

        // std::cout << seq.sites() << " with k " << k << "\n";
        // kmer_strings_non_overlapping( seq, settings, std::cout );
        // std::cout << "\n";
    }

    // Test edge case.
    auto const short_seq = Sequence( "label", "ACGT" );
    auto const short_set = SignatureSpecifications( "ACGT", 5 );
    // kmer_strings_non_overlapping( short_seq, short_set, std::cout );
    auto const short_str  = kmer_strings_non_overlapping( short_seq, short_set );
    EXPECT_EQ( short_str, std::vector<std::string>() );
}
