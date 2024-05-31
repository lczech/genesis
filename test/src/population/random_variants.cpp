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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/math/random.hpp"

#include <cmath>
#include <limits>

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

/**
 * @brief This is a helper function to create random Variants, to be used for testing.
 *
 * Just to avoid code duplication in the test cases.
 */
std::vector<Variant> test_create_random_variants_( size_t min_count )
{
    // Number of samples per variant
    auto const n_positions = 100;
    auto const n_samples = permuted_congruential_generator( 2, 10 );

    // Create a list of Variants and samples, with random content
    std::vector<Variant> data;
    data.resize( n_positions );
    for( size_t n_var = 0; n_var < n_positions; ++n_var ) {
        auto& variant = data[n_var];
        variant.chromosome = "1";
        variant.position = n_var + 1;
        variant.status.set(
            permuted_congruential_generator(
                static_cast<int>( VariantFilterTag::kEnd ) - 1
            )
        );

        // Fill the variant with samples
        variant.samples.resize( n_samples );
        for( size_t n_smp = 0; n_smp < n_samples; ++n_smp ) {
            auto& sample = variant.samples[n_smp];

            // Make a selection of how many of the counts we want to fill.
            // This makes sure that we are not underrepresenting low counts.
            auto const num_non_empty = permuted_congruential_generator( 4 );
            switch( num_non_empty ) {
                case 0: {
                    break;
                }
                case 1: {
                    sample.a_count = permuted_congruential_generator( 1, 10 );
                    break;
                }
                case 2: {
                    sample.a_count = permuted_congruential_generator( 1, 10 );
                    sample.c_count = permuted_congruential_generator( 1, 10 );
                    break;
                }
                case 3: {
                    sample.a_count = permuted_congruential_generator( 1, 10 );
                    sample.c_count = permuted_congruential_generator( 1, 10 );
                    sample.g_count = permuted_congruential_generator( 1, 10 );
                    break;
                }
                case 4: {
                    sample.a_count = permuted_congruential_generator( 1, 10 );
                    sample.c_count = permuted_congruential_generator( 1, 10 );
                    sample.g_count = permuted_congruential_generator( 1, 10 );
                    sample.t_count = permuted_congruential_generator( 1, 10 );
                    break;
                }
            }

            // Also set a random status
            sample.status.set(
                permuted_congruential_generator(
                    static_cast<int>( SampleCountsFilterTag::kEnd ) - 1
                )
            );

            // Lastly, to avoid missing data issues in the computation,
            // we always set the status to missing if there are no counts.
            // Our filters would usually catch that, so it's fair to do this here as well.
            if( nucleotide_sum( sample ) < min_count ) {
                sample.status.reset( SampleCountsFilterTag::kMissing );
            }
        }
    }
    return data;
}

std::vector<Variant> test_create_random_variants_()
{
    return test_create_random_variants_( 0 );
}
