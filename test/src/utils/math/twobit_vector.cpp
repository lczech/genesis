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

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/utils/math/twobit_vector.hpp"
#include "genesis/utils/math/twobit_vector/functions.hpp"
#include "genesis/utils/math/twobit_vector/iterator_deletions.hpp"
#include "genesis/utils/math/twobit_vector/iterator_insertions.hpp"
#include "genesis/utils/math/twobit_vector/iterator_substitutions.hpp"

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
            }
        case 'C':
            switch( count ) {
                case 0: return 'A';
                case 1: return 'T';
                case 2: return 'G';
            }
        case 'G':
            switch( count ) {
                case 0: return 'T';
                case 1: return 'A';
                case 2: return 'C';
            }
        case 'T':
            switch( count ) {
                case 0: return 'G';
                case 1: return 'C';
                case 2: return 'A';
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
//     Main Tests
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
