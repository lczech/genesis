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

#ifdef GENESIS_HTSLIB

#include "src/common.hpp"

#include "genesis/population/formats/vcf_input_stream.hpp"
#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/plotting/af_spectrum.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/vcf_window.hpp"

#include <bitset>
#include <numeric>
#include <stdexcept>
#include <utility>

using namespace genesis::population;
using namespace genesis::utils;

struct VcfWindowResultsDP
{
    size_t chrom_start_count = 0;
    size_t chrom_finish_count = 0;
    size_t enqueue_count = 0;
    size_t dequeue_count = 0;
    size_t emission_count = 0;

    size_t enqueue_dp_info = 0;
    size_t dequeue_dp_info = 0;
    size_t emission_dp_info = 0;
    size_t dp_sum_info = 0;

    size_t enqueue_dp_format = 0;
    size_t dequeue_dp_format = 0;
    size_t emission_dp_format = 0;
    size_t dp_sum_format = 0;
};

VcfWindowResultsDP test_run_vcf_window_dp( std::string const& infile, size_t width, size_t stride )
{
    // LOG_INFO << "vcf window " << width << " " << stride;

    VcfWindowResultsDP result;

    // Make a window that counts both DP values from the info and the format fields.
    using DPData = std::pair<size_t, size_t>;
    using DPWindowGen = SlidingWindowGenerator<DPData>;
    DPWindowGen window_gen( SlidingWindowType::kInterval, width, stride );

    window_gen.add_chromosome_start_plugin( [&]( std::string const& chromosome, DPWindowGen::Accumulator& accu )
    {
        (void) chromosome;
        (void) accu;
        ++result.chrom_start_count;
    });
    window_gen.add_chromosome_finish_plugin( [&]( std::string const& chromosome, DPWindowGen::Accumulator& accu )
    {
        (void) chromosome;
        (void) accu;
        ++result.chrom_finish_count;
    });
    window_gen.add_enqueue_plugin( [&]( DPWindowGen::Entry const& entry, DPWindowGen::Accumulator& accu )
    {
        (void) entry;
        (void) accu;
        ++result.enqueue_count;

        result.enqueue_dp_info += entry.data.first;
        result.enqueue_dp_format += entry.data.second;
    });
    window_gen.add_dequeue_plugin( [&]( DPWindowGen::Entry const& entry, DPWindowGen::Accumulator& accu )
    {
        (void) entry;
        (void) accu;
        ++result.dequeue_count;

        result.dequeue_dp_info += entry.data.first;
        result.dequeue_dp_format += entry.data.second;
    });
    window_gen.add_emission_plugin( [&]( DPWindowGen::Window const& window ) {
        ++result.emission_count;

        for( auto const& entry : window ) {
            result.emission_dp_info += entry.data.first;
            result.emission_dp_format += entry.data.second;
        }
    });

    run_vcf_window<DPData>( window_gen, infile, [&]( VcfRecord const& record ){
        // ASSERT_TRUE( record.has_info( "DP" ) );
        // ASSERT_TRUE( record.has_format( "DP" ) );

        auto dp_info = record.get_info_int("DP");
        if( dp_info.size() != 1 ) {
            LOG_ERR << "dp_info.size() != 1";
        }
        // ASSERT_EQ( 1, dp_info.size() );
        result.dp_sum_info += dp_info[0];

        size_t dp_format_sum = 0;
        size_t dp_format_size = 0;
        for( auto dp : record.get_format_int("DP") ) {
            auto vals = dp.get_values();
            // ASSERT_EQ( 1, vals.size() );
            if( vals.size() != 1 ) {
                LOG_ERR << "vals.size() != 1";
            }
            // LOG_DBG1 << vals[0];

            dp_format_sum += vals[0];
            ++dp_format_size;
        }
        result.dp_sum_format += dp_format_sum;

        // ASSERT_EQ( 3, dp_format_size );
        if( dp_format_size != 3 ) {
            LOG_ERR << "dp_format_size != 3";
        }
        // LOG_INFO << record.at() << " " << dp_info[0] << " " << dp_format_sum;

        return std::pair<size_t, size_t>{ dp_info[0], dp_format_sum };
    });

    return result;
}

TEST( Vcf, Window )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    // Run with window size 1.
    // Important to get that extreme case right.
    auto const res_1 = test_run_vcf_window_dp( infile, 1, 1 );

    EXPECT_EQ( 1, res_1.chrom_start_count );
    EXPECT_EQ( 1, res_1.chrom_finish_count );
    EXPECT_EQ( 5, res_1.enqueue_count );
    EXPECT_EQ( 5, res_1.dequeue_count );
    EXPECT_EQ( 1435964, res_1.emission_count );

    EXPECT_EQ( 57, res_1.enqueue_dp_info );
    EXPECT_EQ( 57, res_1.dequeue_dp_info );
    EXPECT_EQ( 57, res_1.emission_dp_info );
    EXPECT_EQ( 57, res_1.dp_sum_info );

    EXPECT_EQ( 57, res_1.enqueue_dp_format );
    EXPECT_EQ( 57, res_1.dequeue_dp_format );
    EXPECT_EQ( 57, res_1.emission_dp_format );
    EXPECT_EQ( 57, res_1.dp_sum_format );

    // Run with window size 10.
    // Just a normal test run.
    auto const res_10 = test_run_vcf_window_dp( infile, 10, 10 );

    EXPECT_EQ( 1, res_10.chrom_start_count );
    EXPECT_EQ( 1, res_10.chrom_finish_count );
    EXPECT_EQ( 5, res_10.enqueue_count );
    EXPECT_EQ( 5, res_10.dequeue_count );
    EXPECT_EQ( 143596, res_10.emission_count );

    EXPECT_EQ( 57, res_10.enqueue_dp_info );
    EXPECT_EQ( 57, res_10.dequeue_dp_info );
    EXPECT_EQ( 57, res_10.emission_dp_info );
    EXPECT_EQ( 57, res_10.dp_sum_info );

    EXPECT_EQ( 57, res_10.enqueue_dp_format );
    EXPECT_EQ( 57, res_10.dequeue_dp_format );
    EXPECT_EQ( 57, res_10.emission_dp_format );
    EXPECT_EQ( 57, res_10.dp_sum_format );

    // Run with window size 1435963.
    // That is one less than the size of the chromsome. We expect one emission,
    // as we just drop the remaining positions of not finished intervals.
    auto const res_1435963 = test_run_vcf_window_dp( infile, 1435963, 1435963 );

    EXPECT_EQ( 1, res_1435963.chrom_start_count );
    EXPECT_EQ( 1, res_1435963.chrom_finish_count );
    EXPECT_EQ( 5, res_1435963.enqueue_count );
    EXPECT_EQ( 5, res_1435963.dequeue_count );
    EXPECT_EQ( 1, res_1435963.emission_count );

    EXPECT_EQ( 57, res_1435963.enqueue_dp_info );
    EXPECT_EQ( 57, res_1435963.dequeue_dp_info );
    EXPECT_EQ( 57, res_1435963.emission_dp_info );
    EXPECT_EQ( 57, res_1435963.dp_sum_info );

    EXPECT_EQ( 57, res_1435963.enqueue_dp_format );
    EXPECT_EQ( 57, res_1435963.dequeue_dp_format );
    EXPECT_EQ( 57, res_1435963.emission_dp_format );
    EXPECT_EQ( 57, res_1435963.dp_sum_format );

    // Run with window size 1435964.
    // That is exactly the size of the chromsome. We expect one emission.
    auto const res_1435964 = test_run_vcf_window_dp( infile, 1435964, 1435964 );

    EXPECT_EQ( 1, res_1435964.chrom_start_count );
    EXPECT_EQ( 1, res_1435964.chrom_finish_count );
    EXPECT_EQ( 5, res_1435964.enqueue_count );
    EXPECT_EQ( 5, res_1435964.dequeue_count );
    EXPECT_EQ( 1, res_1435964.emission_count );

    EXPECT_EQ( 57, res_1435964.enqueue_dp_info );
    EXPECT_EQ( 57, res_1435964.dequeue_dp_info );
    EXPECT_EQ( 57, res_1435964.emission_dp_info );
    EXPECT_EQ( 57, res_1435964.dp_sum_info );

    EXPECT_EQ( 57, res_1435964.enqueue_dp_format );
    EXPECT_EQ( 57, res_1435964.dequeue_dp_format );
    EXPECT_EQ( 57, res_1435964.emission_dp_format );
    EXPECT_EQ( 57, res_1435964.dp_sum_format );

    // Run with window size 1435965.
    // That is one more than the size of the chromsome. We expect zero emissions,
    // as not a single window will have finished. Also, as there won't be any emissions,
    // we also expect none of the values of the emission or dequeueing.
    auto const res_1435965 = test_run_vcf_window_dp( infile, 1435965, 1435965 );

    EXPECT_EQ( 1, res_1435965.chrom_start_count );
    EXPECT_EQ( 1, res_1435965.chrom_finish_count );
    EXPECT_EQ( 5, res_1435965.enqueue_count );
    EXPECT_EQ( 0, res_1435965.dequeue_count );
    EXPECT_EQ( 0, res_1435965.emission_count );

    EXPECT_EQ( 57, res_1435965.enqueue_dp_info );
    EXPECT_EQ( 0, res_1435965.dequeue_dp_info );
    EXPECT_EQ( 0, res_1435965.emission_dp_info );
    EXPECT_EQ( 57, res_1435965.dp_sum_info );

    EXPECT_EQ( 57, res_1435965.enqueue_dp_format );
    EXPECT_EQ( 0, res_1435965.dequeue_dp_format );
    EXPECT_EQ( 0, res_1435965.emission_dp_format );
    EXPECT_EQ( 57, res_1435965.dp_sum_format );
}

#endif // htslib guard
