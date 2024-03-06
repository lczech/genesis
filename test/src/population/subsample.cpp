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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/functions/subsample.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/math/random.hpp"

using namespace genesis::population;
using namespace genesis::sequence;
using namespace genesis::utils;

template<typename Transformer>
void test_base_counts_subsampling_(
    Transformer transformer
) {
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    auto const n = 50000;
    for( size_t i = 0; i < n; ++i ) {
        BaseCounts sample;
        sample.a_count = permuted_congruential_generator( 1000 );
        sample.c_count = permuted_congruential_generator( 1000 );
        sample.g_count = permuted_congruential_generator( 1000 );
        sample.t_count = permuted_congruential_generator( 1000 );
        auto const old_sum = nucleotide_sum( sample );

        // Some cases will be below the max sum, but that's okay and needs testing as well.
        auto const max = permuted_congruential_generator( 1000 );

        // LOG_DBG << sample.a_count << ":" << sample.c_count << ":" << sample.g_count << ":" << sample.t_count << " --> " << max;

        // Run the function and its tests
        transformer( sample, max );
        auto const new_sum = nucleotide_sum( sample );
        EXPECT_LE( new_sum, old_sum );
        EXPECT_LE( new_sum, max );
        if( old_sum >= max ) {
            EXPECT_EQ( new_sum, max );
        } else {
            EXPECT_EQ( new_sum, old_sum );
        }

        // LOG_DBG << sample.a_count << ":" << sample.c_count << ":" << sample.g_count << ":" << sample.t_count;
    }
}

TEST( BaseCounts, Subscale )
{
    test_base_counts_subsampling_<void(*)(BaseCounts&, size_t)>( transform_subscale );
}

TEST( BaseCounts, SubsampleWithReplacement )
{
    test_base_counts_subsampling_<void(*)(BaseCounts&, size_t)>( transform_subsample_with_replacement );
}

TEST( BaseCounts, SubsampleWithoutReplacement )
{
    test_base_counts_subsampling_<void(*)(BaseCounts&, size_t)>( transform_subsample_without_replacement );
}
