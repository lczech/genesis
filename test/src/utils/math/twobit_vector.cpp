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

#include <random>
#include <string>
#include <vector>
// #include <sstream>

#include "lib/utils/math/twobit_vector.hpp"
#include "lib/utils/math/twobit_vector/functions.hpp"
#include "lib/utils/math/twobit_vector/iterator_deletions.hpp"
#include "lib/utils/math/twobit_vector/iterator_insertions.hpp"
#include "lib/utils/core/options.hpp"
#include "lib/utils/text/string.hpp"

using namespace genesis;
using namespace utils;

class RandomNucleotideGenerator
{
public:

    RandomNucleotideGenerator()
        : nucleotids_distrib({ 1, 1, 1, 1 })
        // , length_distrib( 10, 2 )
        , length_distrib( 400, 200 )
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

TwobitVector::WordType test_delete_position( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];

        for( size_t i = 0; i < vec.size(); ++i ) {
            // LOG_DBG << "=======================================";
            // LOG_DBG << "i " << i;

            // auto del_seq = seq;
            // del_seq.erase( i, 1 );
            // // LOG_DBG << del_seq;

            auto del_vec = vec;
            del_vec.remove_at( i );
            // LOG_DBG << to_nucleic_acids( del_vec );

            // EXPECT_EQ( del_seq, to_nucleic_acids( del_vec ) );
            // EXPECT_EQ( vec.size() - 1, del_vec.size() );
            // EXPECT_TRUE( del_vec.validate() );

            xhash ^= del_vec.hash();

            // LOG_DBG << del_vec.hash();

            // counter += del_vec.size();
        }
    }

    LOG_DBG << bitstring( xhash );
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
            // LOG_DBG;

            // auto h1 = it_b.hash();
            // auto h2 = it_b.vector().hash();
            // LOG_DBG << h1 << " " << h2 << " " << ( h1 == h2 ? "." : "x" );
            // EXPECT_EQ( h2, h1 );

            xhash ^= it_b.hash();

            // EXPECT_EQ( vec.size() - 1, it_b.vector().size() );
            // EXPECT_TRUE( it_b.vector().validate() );
            ++it_b;

            // counter += it_b.position();
        }
    }

    LOG_DBG << bitstring( xhash );
    return xhash;
}

TwobitVector::WordType test_insert_position( std::vector< TwobitVector > const& seqs )
{
    TwobitVector::WordType xhash = 0;

    for( size_t is = 0; is < seqs.size(); ++is ) {
        auto const& vec = seqs[ is ];
        auto const  seq = to_nucleic_acids( vec );
        // LOG_DBG << "o " << to_nucleic_acids( vec );

        for( size_t i = 0; i < vec.size() + 1; ++i ) {
            for( size_t j = 0; j < 4; ++j ) {
                // auto n_str = std::string( 1, translate_to_nucleic_acid( j ));

                auto ins_vec = vec;
                ins_vec.insert_at( i, TwobitVector::ValueType( j ));

                // auto ins_seq = seq;
                // ins_seq.insert( i, n_str );

                xhash ^= ins_vec.hash();

                // LOG_DBG << "i " << to_nucleic_acids( ins_vec );
                // LOG_DBG << "s " << ins_seq;

                // EXPECT_EQ( ins_seq, to_nucleic_acids( ins_vec ));
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
            // LOG_DBG;
            // LOG_DBG << to_nucleic_acids( it_b.vector() );
            //
            // LOG_DBG << "i " << twobit_to_bitstring( it_b.hash() );
            // LOG_DBG << "v " << twobit_to_bitstring( it_b.vector().hash() );

            auto h1 = it_b.hash();
            // auto h2 = it_b.vector().hash();
            // LOG_DBG << h1 << " " << h2 << " " << ( h1 == h2 ? "." : "x" );
            // EXPECT_EQ( h2, h1 );

            xhash ^= h1;

            EXPECT_EQ( vec.size() + 1, it_b.vector().size() );
            EXPECT_TRUE( it_b.vector().validate() );
            ++it_b;

            // counter += it_b.position();
        }
    }

    return xhash;
}

TEST( TwobitVector, Copy )
{
    std::vector< TwobitVector > sequences;

    // std::string g_seq = "CAT";
    // auto g_vec = from_nucleic_acids( g_seq );

    // sequences.push_back( g_vec );
    // test_insert_iterator( sequences );
    // return;
    //
    // test_insert_position( sequences );
    // return;

    // TwobitVector::WordType t = 8;
    // LOG_DBG << "t " << twobit_to_bitstring(t);
    // return;

    auto rng = RandomNucleotideGenerator();
    for( size_t i = 0; i < 1000; ++i ) {
        auto seq = rng.generate_random_nt_string();
        auto vec = from_nucleic_acids( seq );
        sequences.push_back( vec );
    }

    LOG_TIME << "start";
    // auto pxhash = test_delete_position( sequences );
    auto pxhash = test_insert_position( sequences );
    LOG_TIME << "done pos";
    // auto ixhash = test_delete_iterator( sequences );
    auto ixhash = test_insert_iterator( sequences );
    LOG_TIME << "done it";

    EXPECT_EQ( pxhash, ixhash );

    // auto seq = rng.generate_random_nt_string();
    // auto vec = from_nucleic_acids( seq );

    // size_t counter = 0;


    // for( size_t n = 0; n < 100000; ++n ) {

        // std::string seq = "CGACAAAGGTTTTCGGTGCTTAACGTCTTAAGGGTCCCGGT";

        // LOG_DBG << "iteration " << n << ", length " << seq.size();
        // LOG_DBG << seq;

        // EXPECT_EQ( seq, to_nucleic_acids( vec ));

        // Output the data numbers.
        // std::string seq_out;
        // for( size_t i = 0; i < vec.data_size(); ++i ) {
        //     seq_out += utils::to_string( vec.data_at(i) ) + " ";
        // }
        // LOG_DBG << seq_out;

        // LOG_DBG << vec.hash();

        // for( size_t i = 0; i < seq.size(); ++i ) {
        //     // LOG_DBG << "=======================================";
        //     // LOG_DBG << "i " << i;
        //
        //     // auto del_seq = seq;
        //     // del_seq.erase( i, 1 );
        //     // // LOG_DBG << del_seq;
        //
        //     auto del_vec = vec;
        //     del_vec.remove_at( i );
        //     // LOG_DBG << to_nucleic_acids( del_vec );
        //
        //     // EXPECT_EQ( del_seq, to_nucleic_acids( del_vec ) );
        //     EXPECT_EQ( vec.size() - 1, del_vec.size() );
        //     EXPECT_TRUE( del_vec.validate() );
        //
        //     xhash ^= del_vec.hash();
        //
        //     // LOG_DBG << del_vec.hash();
        //
        //     // counter += del_vec.size();
        // }

        // LOG_DBG << "====";

        // size_t i = 0;
        // for( auto it : iterate_deletions( vec )) {
        //     // LOG_DBG << "=======================================";
        //     // LOG_DBG << "i " << i;
        //
        //     // auto del_seq = seq;
        //     // del_seq.erase( i, 1 );
        //     // // LOG_DBG << del_seq;
        //     // ++i;
        //
        //     // EXPECT_EQ( del_seq, to_nucleic_acids( it.vector() ));
        //     EXPECT_EQ( vec.size() - 1, it.vector().size() );
        //     EXPECT_TRUE( it.vector().validate() );
        // }

        // auto it_b = IteratorDeletions( vec );
        // auto it_e = IteratorDeletions();
        // while( it_b != it_e ) {
        //     // LOG_DBG;
        //
        //     auto h1 = it_b.hash();
        //     auto h2 = it_b.vector().hash();
        //     // LOG_DBG << h1 << " " << h2 << " " << ( h1 == h2 ? "." : "x" );
        //     EXPECT_EQ( h2, h1 );
        //
        //     // EXPECT_EQ( vec.size() - 1, it_b.vector().size() );
        //     // EXPECT_TRUE( it_b.vector().validate() );
        //     ++it_b;
        //
        //     // counter += it_b.position();
        // }
    // }

    // LOG_DBG << "counter " << counter;

    // vec.remove_at(35);
    // LOG_DBG << to_nucleic_acids( vec );


    // length 41 [0-40]
    // DBG  CGACAAAGGTTTTCGGTGCTTAACGTCTTAAG GGTCCCGGT
    // DBG  9502106874493698121 238970
    // 11 10 10 01 01 01 11 10 10
    //  t  g  g  c  c  c  t  g  g
    // 40 39 38 37 36 35 34 33 32
    //
    //  111010010101111010
    //  111010010100000000

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
