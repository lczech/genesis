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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/function/subsample.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/math/random.hpp"

using namespace genesis::population;
using namespace genesis::sequence;
using namespace genesis::utils;

template<typename Transformer>
void test_sample_counts_subsampling_(
    Transformer transformer,
    bool skip_if_below_target_depth
) {
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    auto const n = 50000;
    // auto const n = 10;
    for( size_t i = 0; i < n; ++i ) {
        // LOG_DBG << "===========================";

        SampleCounts sample;
        sample.a_count = permuted_congruential_generator( 100 );
        sample.c_count = permuted_congruential_generator( 100 );
        sample.g_count = permuted_congruential_generator( 100 );
        sample.t_count = permuted_congruential_generator( 100 );
        sample.n_count = permuted_congruential_generator( 100 );
        sample.d_count = permuted_congruential_generator( 100 );
        auto const old_sum = sample_counts_sum( sample );

        // Some cases will be below the target sum, but that's okay and needs testing as well.
        auto const target = permuted_congruential_generator( 600 );

        // LOG_DBG << sample.a_count << ":" << sample.c_count << ":" << sample.g_count << ":" << sample.t_count << ":" << sample.n_count << ":" << sample.d_count << " @ " << old_sum << " --> " << target;

        // Run the function and recount
        transformer( sample, target );
        auto const new_sum = sample_counts_sum( sample );

        // LOG_DBG << sample.a_count << ":" << sample.c_count << ":" << sample.g_count << ":" << sample.t_count << ":" << sample.n_count << ":" << sample.d_count << " @ " << new_sum;

        // We test this for sub-sampling, and for re-sampling.
        // In the former case, we expect the counts to not change if their sum did not exceed
        // the target. In the latter case, we expect them to always match the target coverage.
        if( skip_if_below_target_depth ) {
            EXPECT_LE( new_sum, old_sum );
            EXPECT_LE( new_sum, target );
            if( old_sum >= target ) {
                EXPECT_EQ( new_sum, target );
            } else {
                EXPECT_EQ( new_sum, old_sum );
            }
        } else {
            EXPECT_EQ( new_sum, target );
        }
    }
}

TEST( SampleCounts, Subscale )
{
    test_sample_counts_subsampling_<void(*)(SampleCounts&, size_t)>(
        subscale_counts, true
    );
}

TEST( SampleCounts, Rescale )
{
    test_sample_counts_subsampling_<void(*)(SampleCounts&, size_t)>(
        rescale_counts, false
    );
}

TEST( SampleCounts, SubsampleWithReplacement )
{
    test_sample_counts_subsampling_<void(*)(SampleCounts&, size_t)>(
        subsample_counts_with_replacement, true
    );
}

TEST( SampleCounts, Resample )
{
    test_sample_counts_subsampling_<void(*)(SampleCounts&, size_t)>(
        resample_counts, false
    );
}

TEST( SampleCounts, SubsampleWithoutReplacement )
{
    test_sample_counts_subsampling_<void(*)(SampleCounts&, size_t)>(
        subsample_counts_without_replacement, true
    );
}
