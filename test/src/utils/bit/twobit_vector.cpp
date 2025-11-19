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

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "genesis/utils/bit/twobit_vector.hpp"
#include "genesis/utils/bit/twobit_vector/functions.hpp"
#include "genesis/utils/bit/twobit_vector/iterator_deletions.hpp"
#include "genesis/utils/bit/twobit_vector/iterator_insertions.hpp"
#include "genesis/utils/bit/twobit_vector/iterator_substitutions.hpp"

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace utils;

// =================================================================================================
//     Helper
// =================================================================================================

class RandomNucleotideGenerator
{
public:

    RandomNucleotideGenerator()
        : nucleotids_distrib({ 1, 1, 1, 1 })
        , length_distrib( 100, 20 )
    {}

    std::string generate_random_nt_string( size_t n )
    {
        std::string result;
        result.reserve( n );

        char nucleotides[] = { 'A', 'C', 'G', 'T' };
        for( size_t i = 0; i < n; ++i ) {
            size_t p = nucleotids_distrib( utils::Options::get().random_engine() );
            result += nucleotides[ p ];
        }

        return result;
    }

    std::string generate_random_nt_string()
    {
        double nd;
        do {
            nd = length_distrib( utils::Options::get().random_engine() );
        } while( nd <= 1.0 );
        size_t n = static_cast< size_t >( nd );
        return generate_random_nt_string( n );
    }

    std::discrete_distribution<size_t> nucleotids_distrib;
    std::normal_distribution<double>   length_distrib;

};

// =================================================================================================
//     Deletions
// =================================================================================================

TwobitVector::WordType test_delete_position( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];

        for( size_t i = 0; i < vec.size(); ++i ) {
            auto del_seq = to_nucleic_acids( vec );
            del_seq.erase( i, 1 );

            auto del_vec = vec;
            del_vec.remove_at( i );
            xhash ^= del_vec.hash();

            EXPECT_EQ( del_seq, to_nucleic_acids( del_vec ) );
            EXPECT_EQ( vec.size() - 1, del_vec.size() );
            EXPECT_TRUE( del_vec.validate() );
        }
    }

    return xhash;
}

TwobitVector::WordType test_delete_iterator( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];

        auto it_b = IteratorDeletions( vec );
        auto it_e = IteratorDeletions();
        while( it_b != it_e ) {

            auto h1 = it_b.hash();
            auto h2 = it_b.vector().hash();
            EXPECT_EQ( h2, h1 );

            xhash ^= h1;

            EXPECT_EQ( vec.size() - 1, it_b.vector().size() );
            EXPECT_TRUE( it_b.vector().validate() );

            ++it_b;
        }
    }

    return xhash;
}

// =================================================================================================
//     Insertions
// =================================================================================================

TwobitVector::WordType test_insert_position( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];
        auto const  seq = to_nucleic_acids( vec );

        for( size_t i = 0; i < vec.size() + 1; ++i ) {
            for( size_t j = 0; j < 4; ++j ) {
                auto n_str = std::string(
                    1,
                    translate_to_nucleic_acid( static_cast<TwobitVector::ValueType>(j) )
                );

                auto ins_vec = vec;
                ins_vec.insert_at( i, TwobitVector::ValueType( j ));

                auto ins_seq = seq;
                ins_seq.insert( i, n_str );
                xhash ^= ins_vec.hash();

                EXPECT_EQ( ins_seq, to_nucleic_acids( ins_vec ));
                EXPECT_TRUE( ins_vec.validate() );
                EXPECT_EQ( vec.size() + 1, ins_vec.size() );
            }
        }
    }

    return xhash;
}

TwobitVector::WordType test_insert_iterator( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];
        auto const  seq = to_nucleic_acids( vec );

        auto it_b = IteratorInsertions( vec );
        auto it_e = IteratorInsertions();
        while( it_b != it_e ) {

            auto h1 = it_b.hash();
            auto h2 = it_b.vector().hash();
            xhash ^= h1;

            EXPECT_EQ( h2, h1 );
            EXPECT_EQ( vec.size() + 1, it_b.vector().size() );
            EXPECT_TRUE( it_b.vector().validate() );

            ++it_b;
        }
    }

    return xhash;
}

// =================================================================================================
//     Substitutions
// =================================================================================================

char cycle_substitution( char original, size_t count )
{
    switch( original ) {
        case 'A':
            switch( count ) {
                case 0: return 'C';
                case 1: return 'G';
                case 2: return 'T';
                default:
                    throw std::runtime_error( "Got to AA" );
            }
        case 'C':
            switch( count ) {
                case 0: return 'A';
                case 1: return 'T';
                case 2: return 'G';
                default:
                    throw std::runtime_error( "Got to CC" );
            }
        case 'G':
            switch( count ) {
                case 0: return 'T';
                case 1: return 'A';
                case 2: return 'C';
                default:
                    throw std::runtime_error( "Got to GG" );
            }
        case 'T':
            switch( count ) {
                case 0: return 'G';
                case 1: return 'C';
                case 2: return 'A';
                default:
                    throw std::runtime_error( "Got to TT" );
            }
    }
    throw std::runtime_error( "Invalid char in test. Weird." );
    return '\0';
}

TwobitVector::WordType test_substitution_position( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];
        auto seq = to_nucleic_acids( vec );

        for( size_t i = 0; i < vec.size(); ++i ) {
            auto orig = seq[i];

            for( size_t j = 0; j < 3; ++j ) {
                seq[i] = cycle_substitution( orig, j );
                auto sub_vec = from_nucleic_acids( seq );

                xhash ^= sub_vec.hash();
            }

            seq[i] = orig;
        }
    }

    return xhash;
}

TwobitVector::WordType test_substitution_iterator( std::vector< TwobitVector > const& seqs)
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];
        auto const  seq = to_nucleic_acids( vec );

        auto it_b = IteratorSubstitutions( vec );
        auto it_e = IteratorSubstitutions();
        while( it_b != it_e ) {

            auto h1 = it_b.hash();
            auto h2 = it_b.vector().hash();
            EXPECT_EQ( h2, h1 );

            xhash ^= h1;

            EXPECT_EQ( vec.size(), it_b.vector().size() );
            EXPECT_TRUE( it_b.vector().validate() );
            ++it_b;
        }
    }

    return xhash;
}

// =================================================================================================
//     Main iterators tests
// =================================================================================================

TEST( TwobitVector, Deletion )
{
    auto rng = RandomNucleotideGenerator();
    for( size_t n = 0; n < 10; ++n ) {

        // Generate a sequence.
        auto seq = rng.generate_random_nt_string();
        auto vec = from_nucleic_acids( seq );

        EXPECT_EQ( seq, to_nucleic_acids( vec ));

        // Test deletion.
        for( size_t i = 0; i < seq.size(); ++i ) {
            auto del_seq = seq;
            del_seq.erase( i, 1 );

            auto del_vec = vec;
            del_vec.remove_at( i );

            EXPECT_EQ( del_seq, to_nucleic_acids( del_vec ) );
            EXPECT_EQ( vec.size() - 1, del_vec.size() );
            EXPECT_TRUE( del_vec.validate() );
        }
    }
}

TEST( TwobitVector, Hashing )
{
    // Test the iterators and their hashing shortcuts.

    // Generate sequences.
    size_t n = 5;
    std::vector< TwobitVector > sequences;
    auto rng = RandomNucleotideGenerator();
    for( size_t i = 0; i < n; ++i ) {
        auto seq = rng.generate_random_nt_string();
        auto vec = from_nucleic_acids( seq );
        sequences.push_back( vec );
    }

    // Run position-wise functions.
    TwobitVector::WordType pxhash = 0;
    pxhash ^= test_delete_position( sequences );
    pxhash ^= test_insert_position( sequences );
    pxhash ^= test_substitution_position( sequences );

    // Run iterators.
    TwobitVector::WordType ixhash = 0;
    ixhash ^= test_delete_iterator( sequences );
    ixhash ^= test_insert_iterator( sequences );
    ixhash ^= test_substitution_iterator( sequences );

    // Both need to give the same hash.
    EXPECT_EQ( pxhash, ixhash );
}

TEST( TwobitVector, Bitstring )
{
    std::string seq  = "CGACAAAGGTTTTCGGTGCTTAACGTCTTAAGGGTCCCGGT";
    auto vec = from_nucleic_acids( seq );
    std::string bits =
        "10 00 00 11 11 01 11 10 01 00 00 11 11 01 10 11 "
        "10 10 01 11 11 11 11 10 10 00 00 00 01 00 10 01\n"
        "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
        "00 00 00 00 00 00 00 11 10 10 01 01 01 11 10 10\n";

    EXPECT_EQ( bits, bitstring( vec ));
}

// =================================================================================================
//     K-mer counting
// =================================================================================================

using OccVec = std::vector<size_t>;

TEST( TwobitVector, KmerOccurrences )
{
    // Homopolymers
    {
        std::string const seq  = "AAAA";
        auto const vec = from_nucleic_acids( seq );

        // 1-mers
        auto const occ1 = kmer_occurrences( vec, 1 );
        EXPECT_EQ( 4, occ1.size() );
        EXPECT_EQ( OccVec({ 4, 0, 0, 0 }), occ1 );

        // 2-mers
        auto const occ2 = kmer_occurrences( vec, 2 );
        EXPECT_EQ( 16, occ2.size() );
        EXPECT_EQ( OccVec({ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }), occ2 );
    }

    // Dimer repeats
    {
        std::string const seq  = "ACACAC";
        auto const vec = from_nucleic_acids( seq );

        // 1-mers
        auto const occ1 = kmer_occurrences( vec, 1 );
        EXPECT_EQ( 4, occ1.size() );
        EXPECT_EQ( OccVec({ 3, 3, 0, 0 }), occ1 );

        // 2-mers
        auto const occ2 = kmer_occurrences( vec, 2 );
        EXPECT_EQ( 16, occ2.size() );
        EXPECT_EQ( OccVec({ 0, 3, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }), occ2 );
    }
}

TEST( TwobitVector, KmerOccurrencesEmptySequence )
{
    // Empty sequence: all counts must be zero, but result vector still has size 4^k

    std::string const seq = "";
    auto const vec = from_nucleic_acids( seq );

    // k = 1
    auto const occ1 = kmer_occurrences( vec, 1 );
    ASSERT_EQ( 4u, occ1.size() );
    EXPECT_TRUE( std::all_of(
        occ1.begin(), occ1.end(),
        []( size_t x ){ return x == 0; }
    ));

    // k = 2
    auto const occ2 = kmer_occurrences( vec, 2 );
    ASSERT_EQ( 16u, occ2.size() );
    EXPECT_TRUE( std::all_of(
        occ2.begin(), occ2.end(),
        []( size_t x ){ return x == 0; }
    ));
}

TEST( TwobitVector, KmerOccurrencesSimpleACGT )
{
    // Simple mixed sequence "ACGT" to check that indexing of k-mers behaves as
    // expected for 1-mers, 2-mers and full-length 4-mers.

    std::string const seq = "ACGT";
    auto const vec = from_nucleic_acids( seq );

    // 1-mers: A, C, G, T -> each once
    auto const occ1 = kmer_occurrences( vec, 1 );
    ASSERT_EQ( 4u, occ1.size() );
    EXPECT_EQ( OccVec({ 1, 1, 1, 1 }), occ1 );

    // 2-mers: AC, CG, GT
    //
    // Encoding is: index = 4 * first + second, with A=0,C=1,G=2,T=3
    //  AC -> 4*0 + 1 = 1
    //  CG -> 4*1 + 2 = 6
    //  GT -> 4*2 + 3 = 11
    auto const occ2 = kmer_occurrences( vec, 2 );
    ASSERT_EQ( 16u, occ2.size() );
    OccVec exp2( 16, 0 );
    exp2[1]  = 1;  // AC
    exp2[6]  = 1;  // CG
    exp2[11] = 1;  // GT
    EXPECT_EQ( exp2, occ2 );

    // 4-mers: only one k-mer, ACGT
    //
    // Streaming encoding:
    // ((((0)*4+1)*4+2)*4+3) = 27
    auto const occ4 = kmer_occurrences( vec, 4 );
    ASSERT_EQ( 256u, occ4.size() );

    size_t const total4 = std::accumulate(
        occ4.begin(), occ4.end(), size_t{0}
    );
    EXPECT_EQ( 1u, total4 );          // exactly one 4-mer
    EXPECT_EQ( 1u, occ4[27] );        // that 4-mer is ACGT at index 27
}

TEST( TwobitVector, KmerOccurrencesKLongerThanSequence )
{
    // k larger than sequence length: no k-mer is ever "complete", so all counts
    // must stay zero, but the result size is still 4^k.

    std::string const seq = "ACGT";
    auto const vec = from_nucleic_acids( seq );

    // length(seq) = 4, use k = 5 -> no k-mers
    auto const occ5 = kmer_occurrences( vec, 5 );
    ASSERT_EQ( 1024u, occ5.size() );  // 4^5
    EXPECT_TRUE( std::all_of(
        occ5.begin(), occ5.end(),
        []( size_t x ){ return x == 0; }
    ));
}

TEST( TwobitVector, KmerOccurrencesKEqualsSequenceLength )
{
    // k equal to sequence length: exactly one k-mer, total count must be 1.

    std::string const seq = "AAAA";
    auto const vec = from_nucleic_acids( seq );

    auto const occ4 = kmer_occurrences( vec, 4 );
    ASSERT_EQ( 256u, occ4.size() ); // 4^4

    size_t const total = std::accumulate(
        occ4.begin(), occ4.end(), size_t{0}
    );
    EXPECT_EQ( 1u, total );         // exactly one 4-mer in total

    // For "AAAA", the index of AAAA is 0 (A=0), so the first entry must be 1
    EXPECT_EQ( 1u, occ4[0] );
}

TEST( TwobitVector, KmerOccurrencesCustomCounterType )
{
    // Template parameter: use a smaller counter type, make sure it compiles and
    // returns the correct type and values.

    std::string const seq = "AAAA";
    auto const vec = from_nucleic_acids( seq );

    auto const occ = kmer_occurrences<uint16_t>( vec, 1 );

    static_assert(
        std::is_same<decltype(occ), const std::vector<uint16_t>>::value,
        "kmer_occurrences<T> must return std::vector<T>"
    );

    ASSERT_EQ( 4u, occ.size() );
    EXPECT_EQ( uint16_t{4}, occ[0] ); // four A's
    EXPECT_EQ( uint16_t{0}, occ[1] );
    EXPECT_EQ( uint16_t{0}, occ[2] );
    EXPECT_EQ( uint16_t{0}, occ[3] );
}
