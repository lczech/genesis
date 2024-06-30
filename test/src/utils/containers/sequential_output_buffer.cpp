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

#include "genesis/utils/containers/sequential_output_buffer.hpp"
#include "genesis/utils/math/random.hpp"

#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace genesis::utils;

// =================================================================================================
//     Sequential Output Buffer
// =================================================================================================

void test_sequential_output_buffer_()
{
    // Prepare the vector where we collect elements. They are added to the results by the buffer,
    // and need to come out in sequence 0 to max_elem in the end.
    auto const max_elem = permuted_congruential_generator( 1000 );
    std::vector<size_t> results;
    results.reserve( max_elem );

    // Make a buffer that on output writes the value to the results vector.
    auto buff = SequentialOutputBuffer<size_t>(
        [&]( size_t value ){
            results.push_back( value );
        }
    );

    // Make a vector with all sequential numbers up to the max, randomized.
    auto values = std::vector<size_t>( max_elem );
    std::iota( values.begin(), values.end(), static_cast<size_t>( 0 ));
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle( values.begin(), values.end(), g );

    // Add all elements in their random order to the buffer.
    // We add each element at its own index, which makes the testing of sequentiality easy.
    for( size_t i = 0; i < max_elem; ++i ) {
        // LOG_DBG << i << " --> " << values[i];
        buff.emplace( values[i], values[i] );
    }
    buff.close();

    // Now we should have exactly the original order back in the result vec.
    EXPECT_EQ( max_elem, results.size() );
    for( size_t i = 0; i < max_elem; ++i ) {
        EXPECT_EQ( i, results[i] );
    }
}

TEST( Containers, SequentialOutputBuffer )
{
    // Same as above.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t const max_tests = 1000;
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        LOG_DBG << "=============================";
        test_sequential_output_buffer_();
    }
}
