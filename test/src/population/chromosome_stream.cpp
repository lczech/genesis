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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/formats/simple_pileup_input_stream.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/streams/variant_input_stream.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/chromosome_stream.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

using namespace genesis::population;
using namespace genesis::utils;

void test_window_stream_chromosome_stream_chromosome_empty_()
{
    // LOG_DBG << "empty";
    std::string const infile = environment->data_dir + "population/empty.pileup";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make a generic input stream over the data stream,
    // and set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    auto win_it = make_default_chromosome_stream( data_gen.begin(), data_gen.end() );

    // Run the tests.
    std::vector<size_t> pos_per_chr;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        EXPECT_EQ( 1, it->first_position() );
        EXPECT_EQ( 1, it->last_position() );

        pos_per_chr.push_back(0);
        // LOG_DBG << "chr " << it->chromosome();
        for( auto const& elem : *it ) {
            // LOG_DBG1 << "at " << elem.position;
            (void) elem;
            ++pos_per_chr.back();
        }
        // LOG_DBG << "done " << cnt;

        EXPECT_EQ( 1, it->first_position() );
        EXPECT_EQ( 1, it->last_position() );
    }

    // Tests
    EXPECT_EQ( 0, pos_per_chr.size() );
}

void test_window_stream_chromosome_stream_chromosome_single_(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = nullptr
) {
    // LOG_DBG << "single";
    std::string const infile = environment->data_dir + "population/78.pileup.gz";
    // std::string const infile = environment->data_dir + "population/example.pileup";

    // Make a generic input stream over the data stream,
    // and set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto data_gen = make_variant_input_stream_from_pileup_file( infile );
    auto win_it = make_default_chromosome_stream( data_gen.begin(), data_gen.end() );
    win_it.sequence_dict( sequence_dict );

    // Run the tests.
    std::vector<size_t> pos_per_chr;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        EXPECT_EQ( 1, it->first_position() );
        if( sequence_dict ) {
            EXPECT_EQ( 8000000, it->last_position() );
        } else {
            EXPECT_EQ( 1, it->last_position() );
        }

        pos_per_chr.push_back(0);
        // LOG_DBG << "chr " << it->chromosome();
        for( auto const& elem : *it ) {
            // LOG_DBG1 << "at " << elem.position;
            (void) elem;
            ++pos_per_chr.back();
        }
        // LOG_DBG << "done " << cnt;

        EXPECT_EQ( 1, it->first_position() );
        if( sequence_dict ) {
            EXPECT_EQ( 8000000, it->last_position() );
        } else {
            EXPECT_EQ( 7850275, it->last_position() );
        }
    }

    // Tests
    ASSERT_EQ( 1, pos_per_chr.size() );
    EXPECT_EQ( 50000, pos_per_chr[0] );
}

void test_window_stream_chromosome_stream_chromosome_multi_(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = nullptr
) {
    // Avoid compiler complaints about unused variable.
    (void) sequence_dict;

    // Multiple chromosomes
    #ifdef GENESIS_HTSLIB

    // LOG_DBG << "multi";
    std::string const infile = environment->data_dir + "population/ex1.bam";

    // Make a generic input stream over the data stream,
    // and set up the window iterator. Rename to `win_it` to use it with the below test code.
    auto data_gen = make_variant_input_stream_from_sam_file( infile );
    auto win_it = make_default_chromosome_stream( data_gen.begin(), data_gen.end() );
    win_it.sequence_dict( sequence_dict );

    // Run the tests.
    std::vector<size_t> pos_per_chr;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        EXPECT_EQ( 1, it->first_position() );
        if( !sequence_dict ) {
            EXPECT_EQ( 1, it->last_position() );
        }

        pos_per_chr.push_back(0);
        // LOG_DBG << "chr " << it->chromosome();
        for( auto const& elem : *it ) {
            // LOG_DBG1 << "at " << elem.position;
            (void) elem;
            ++pos_per_chr.back();
        }
        // LOG_DBG << "done " << cnt;

        EXPECT_EQ( 1, it->first_position() );
        if( sequence_dict ) {
            if( it->chromosome() == "seq1" ) {
                EXPECT_EQ( 2000, it->last_position() );
            } else {
                EXPECT_EQ( 3000, it->last_position() );
            }
        } else {
            if( it->chromosome() == "seq1" ) {
                EXPECT_EQ( 1569, it->last_position() );
            } else {
                EXPECT_EQ( 1567, it->last_position() );
            }
        }
    }

    // Tests
    ASSERT_EQ( 2, pos_per_chr.size() );
    EXPECT_EQ( 1569, pos_per_chr[0] );
    EXPECT_EQ( 1567, pos_per_chr[1] );

    #endif
}

TEST( WindowStream, ChromosomeStreamChromosome )
{
    using namespace genesis::sequence;

    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Without sequence dict.
    test_window_stream_chromosome_stream_chromosome_empty_();
    test_window_stream_chromosome_stream_chromosome_single_();
    test_window_stream_chromosome_stream_chromosome_multi_();

    // Repeat single with sequence dict.
    auto single_dict = std::make_shared<SequenceDict>();
    single_dict->add( "2R", 8000000 );
    test_window_stream_chromosome_stream_chromosome_single_( single_dict );

    // Repeat multi with sequence dict.
    auto multi_dict = std::make_shared<SequenceDict>();
    multi_dict->add( "seq1", 2000 );
    multi_dict->add( "seq2", 3000 );
    test_window_stream_chromosome_stream_chromosome_multi_( multi_dict );
}

TEST( WindowStream, ChromosomeStreamWholeGenome )
{
    // Almost exactly the same as above... we could refactor this to avoid the duplication...
    // Good enough for now, fix later.

    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Empty file
    {
        std::string const infile = environment->data_dir + "population/empty.pileup";
        // std::string const infile = environment->data_dir + "population/example.pileup";

        // Make a generic input stream over the data stream,
        // and set up the window iterator. Rename to `win_it` to use it with the below test code.
        auto data_gen = make_variant_input_stream_from_pileup_file( infile );
        auto win_it = make_default_genome_stream( data_gen.begin(), data_gen.end() );

        // Run the tests.
        std::vector<size_t> pos_per_chr;
        for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
            pos_per_chr.push_back(0);
            // LOG_DBG << "chr " << it->chromosome();
            for( auto const& elem : *it ) {
                // LOG_DBG1 << "at " << elem.position;
                (void) elem;
                ++pos_per_chr.back();
            }
            // LOG_DBG << "done " << cnt;
        }

        // Tests
        EXPECT_EQ( 0, pos_per_chr.size() );
    }

    // Single chromosome
    {
        std::string const infile = environment->data_dir + "population/78.pileup.gz";
        // std::string const infile = environment->data_dir + "population/example.pileup";

        // Make a generic input stream over the data stream,
        // and set up the window iterator. Rename to `win_it` to use it with the below test code.
        auto data_gen = make_variant_input_stream_from_pileup_file( infile );
        auto win_it = make_default_genome_stream( data_gen.begin(), data_gen.end() );

        // Run the tests.
        std::vector<size_t> pos_per_chr;
        for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
            pos_per_chr.push_back(0);
            // LOG_DBG << "chr " << it->chromosome();
            for( auto const& elem : *it ) {
                // LOG_DBG1 << "at " << elem.position;
                (void) elem;
                ++pos_per_chr.back();
            }
            // LOG_DBG << "done " << cnt;
        }

        // Tests
        ASSERT_EQ( 1, pos_per_chr.size() );
        EXPECT_EQ( 50000, pos_per_chr[0] );
    }

    // Multiple chromosomes
    #ifdef GENESIS_HTSLIB
    {
        std::string const infile = environment->data_dir + "population/ex1.bam";

        // Make a generic input stream over the data stream,
        // and set up the window iterator. Rename to `win_it` to use it with the below test code.
        auto data_gen = make_variant_input_stream_from_sam_file( infile );
        auto win_it = make_default_genome_stream( data_gen.begin(), data_gen.end() );

        // Run the tests.
        std::vector<size_t> pos_per_chr;
        for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
            pos_per_chr.push_back(0);
            // LOG_DBG << "chr " << it->chromosome();
            for( auto const& elem : *it ) {
                // LOG_DBG1 << "at " << elem.position;
                (void) elem;
                ++pos_per_chr.back();
            }
            // LOG_DBG << "done " << cnt;
        }

        // Tests
        ASSERT_EQ( 1, pos_per_chr.size() );
        EXPECT_EQ( 3136, pos_per_chr[0] );
    }
    #endif
}
