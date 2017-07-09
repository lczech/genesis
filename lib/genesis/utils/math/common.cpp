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

#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace genesis {
namespace utils {

// =================================================================================================
//     Statistics
// =================================================================================================

MeanStddevPair mean_stddev( std::vector<double> const& vec, double epsilon )
{
    // Prepare result.
    MeanStddevPair result;
    result.mean   = 0.0;
    result.stddev = 0.0;

    // Calculate mean.
    for( size_t i = 0; i < vec.size(); ++i ) {
        result.mean += vec[i];
    }
    result.mean /= static_cast<double>( vec.size() );

    // Calculate column std dev.
    for( size_t i = 0; i < vec.size(); ++i ) {
        result.stddev += (( vec[i] - result.mean ) * ( vec[i] - result.mean ));
    }
    result.stddev /= static_cast<double>( vec.size() );
    result.stddev = std::sqrt( result.stddev );

    // The following in an inelegant (but usual) way to handle near-zero values,
    // which later would cause a division by zero.
    assert( result.stddev >= 0.0 );
    if( result.stddev <= epsilon ){
        result.stddev = 1.0;
    }

    return result;
}

/**
 * @brief Local helper function to get the median in between a range.
 *
 * Both l and r are inclusive.
 *
 * Yes, that would be very much a perfect use case for a proper template function.
 * But I am too lazy right now.
 */
double median( std::vector<double> const& vec, size_t l, size_t r )
{
    assert( l < vec.size() && r < vec.size() && l <= r );

    // Size of the interval.
    size_t const sz = r - l + 1;

    // Even or odd size? Median is calculated differently.
    if( sz % 2 == 0 ) {

        // Get the two middle positions.
        size_t pl = l + sz / 2 - 1;
        size_t pu = l + sz / 2;
        assert( pl < vec.size() && pu < vec.size() );

        return ( vec[pl] + vec[pu] ) / 2.0;

    } else {

        // Int division, rounds down. This is what we want.
        size_t p = l + sz / 2;
        assert( p < vec.size() );

        return vec[p];
    }
}

double median( std::vector<double> const& vec )
{
    // Checks.
    if( ! std::is_sorted( vec.begin(), vec.end() )) {
        throw std::runtime_error( "Vector has to be sorted for median calculation." );
    }
    if( vec.size() == 0 ) {
        return 0.0;
    }

    // Use local helper function, which takes the range inclusively.
    return median( vec, 0, vec.size() - 1 );
}

Quartiles quartiles( std::vector<double> const& vec )
{
    // Prepare result.
    Quartiles result;

    // Checks.
    if( ! std::is_sorted( vec.begin(), vec.end() )) {
        throw std::runtime_error( "Vector has to be sorted for quartiles calculation." );
    }
    if( vec.size() == 0 ) {
        return result;
    }

    // Set min, 50% and max.
    result.q0 = vec[0];
    result.q2 = median( vec, 0, vec.size() - 1 );
    result.q4 = vec[ vec.size() - 1 ];

    // Even or odd size? Quartiles are calculated differently.
    // This could be done shorter, but this way feels more expressive.
    if( vec.size() % 2 == 0 ) {

        // Even: Split exaclty in halves.
        result.q1 = median( vec, 0, vec.size() / 2 - 1 );
        result.q3 = median( vec, vec.size() / 2, vec.size() - 1 );

    } else {

        // Odd: Do not include the median value itself.
        result.q1 = median( vec, 0, vec.size() / 2 - 1 );
        result.q3 = median( vec, vec.size() / 2 + 1, vec.size() - 1 );
    }

    return result;
}

} // namespace utils
} // namespace genesis
