/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/random.hpp"

#include "genesis/utils/core/options.hpp"

#include <cassert>
#include <random>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Fast Randomness
// =================================================================================================

// Implementation from https://en.wikipedia.org/wiki/Permuted_congruential_generator#Example_code

static uint64_t       pcg32_state      = 0x4d595df4d0f33173;   // Or something seed-dependent
static uint64_t const pcg32_multiplier = 6364136223846793005u;
static uint64_t const pcg32_increment  = 1442695040888963407u; // Or an arbitrary odd constant

static inline uint32_t pcg32_rotr32( uint32_t x, unsigned int r )
{
    return x >> r | x << (-r & 31);
}

uint32_t permuted_congruential_generator()
{
    uint64_t x = pcg32_state;
    auto count = static_cast<unsigned int>( x >> 59 ); // 59 = 64 - 5

    pcg32_state = x * pcg32_multiplier + pcg32_increment;
    x ^= x >> 18; // 18 = (64 - 27)/2
    return pcg32_rotr32( static_cast<uint32_t>( x >> 27 ), count ); // 27 = 32 - 5
}

uint32_t permuted_congruential_generator( uint32_t max )
{
    return permuted_congruential_generator() % ( max + 1 );
}

uint32_t permuted_congruential_generator( uint32_t min, uint32_t max )
{
    if( min > max ) {
        throw std::invalid_argument(
            "Invalid call to permuted_congruential_generator( "  + std::to_string(min) + ", " +
            std::to_string(max) + " )"
        );
    }
    return min + permuted_congruential_generator() % ( max - min + 1 );
}

bool permuted_congruential_generator_bool()
{
    return permuted_congruential_generator() % 2 == 0;
}

bool permuted_congruential_generator_bool( uint32_t chance_one_in )
{
    if( chance_one_in == 0 ) {
        throw std::invalid_argument(
            "Invalid call to permuted_congruential_generator_bool( 0 )"
        );
    }
    return permuted_congruential_generator() % chance_one_in == 0;
}

void permuted_congruential_generator_init( uint64_t seed )
{
    pcg32_state = seed + pcg32_increment;
}

// ================================================================================================
//     Sampling
// ================================================================================================

std::vector<size_t> select_without_replacement( size_t const k, size_t const n )
{
    // Following http://stackoverflow.com/a/311716/4184258
    // Knuth's variable names: k=n, n=N

    std::vector<size_t> result;
    result.reserve( k );

    if( k > n ) {
        throw std::invalid_argument(
            "Cannot select more unique elements than there are elements: k == "  +
            std::to_string(k) + " > n == " + std::to_string(n) + "."
        );
    }

    auto& engine = Options::get().random_engine();
    std::uniform_real_distribution<double> distribution( 0.0, 1.0 );

    size_t t = 0; // total input records dealt with
    size_t m = 0; // number of items selected so far

    while( m < k ) {
        // call a uniform( 0, 1 ) random number generator
        double const u = distribution( engine );

        if( (n - t) * u >= k - m ) {
            t++;
        } else {
            assert( t < n );
            result.push_back( t );
            t++;
            m++;
        }
    }
    assert( m == k );
    assert( result.size() == k );

    return result;
}

} // namespace utils
} // namespace genesis
