/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup utils
 */

#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =============================================================================
//     Binomial Coefficient
// =============================================================================

size_t binomial_coefficient( size_t n, size_t k )
{
    // Error cases.
    if( n == 0 ) {
        throw std::invalid_argument( "Cannot compute binomial coefficient with n == 0" );
    }
    if( k == 0 ) {
        throw std::invalid_argument( "Cannot compute binomial coefficient with k == 0" );
    }
    if( k > n ) {
        throw std::invalid_argument( "Cannot compute binomial coefficient with k > n" );
    }

    // Since C(n, k) = C(n, n-k), we can shortcut.
    if( k > n - k ) {
        k = n - k;
    }

    // We follow http://csharphelper.com/blog/2014/08/calculate-the-binomial-coefficient-n-choose-k-efficiently-in-c/
    // In short, C(n, k) = n/k * C(n-1, k-1), which we can continue until C(n-(k-1), 1) to get
    // a long chain of product. Reading these products from right to left, we always have binomial
    // coefficients of smaller n,k, and use these as our intermediates to ensure that we don't
    // get an overflow here.

    size_t result = 1;
    for( size_t i = 1; i <= k; i++)
    {
        // Multiply with overflow check.
        assert( n > k - i );
        if( result >= std::numeric_limits<size_t>::max() / (n - (k - i)) ) {
            throw std::runtime_error(
                "Cannot compute binomial coefficient with n == " + std::to_string(n) +
                " and k == " + std::to_string(k) + " due to numerical overflow."
            );
        }
        result *= n - (k - i);

        // As we are computing the product "backwards", we always have a valid intermediate value,
        // that is, the division by i is always an integer division (which we assert).
        assert( result % i == 0 );
        result /= i;
    }
    return result;
}

} // namespace utils
} // namespace genesis
