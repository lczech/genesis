/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/random.hpp"

#include "genesis/utils/core/options.hpp"

#include <random>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Sampling
// ================================================================================================

std::vector<size_t> select_without_replacement( size_t const k, size_t const n )
{
    // Following http://stackoverflow.com/a/311716/4184258
    // Knuth's variable names: n=N, k=n

    std::vector<size_t> result;

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
    double u;

    while( m < k ) {
        // call a uniform( 0, 1 ) random number generator
        u = distribution( engine );

        if( (n - t) * u >= k - m ) {
            t++;
        } else {
            result.push_back( t );
            t++;
            m++;
        }
    }

    return result;
}

} // namespace utils
} // namespace genesis
