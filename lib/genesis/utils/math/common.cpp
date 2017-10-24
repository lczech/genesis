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

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"

#include "genesis/utils/core/logging.hpp"

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

double pearson_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    if( vec_a.size() != vec_b.size() ) {
        throw std::runtime_error( "Vectors need to have same size." );
    }

    // Calculate column means.
    double mean1 = 0.0;
    double mean2 = 0.0;
    for( size_t i = 0; i < vec_a.size(); ++i ) {
        mean1 += vec_a[ i ];
        mean2 += vec_b[ i ];
    }
    mean1 /= static_cast<double>( vec_a.size() );
    mean2 /= static_cast<double>( vec_b.size() );

    // Calculate PCC parts.
    double numerator = 0.0;
    double stddev1   = 0.0;
    double stddev2   = 0.0;
    for( size_t i = 0; i < vec_a.size(); ++i ) {
        double const d1 = vec_a[ i ] - mean1;
        double const d2 = vec_b[ i ] - mean2;
        numerator += d1 * d2;
        stddev1   += d1 * d1;
        stddev2   += d2 * d2;
    }

    // Calcualte PCC, and assert that it is in the correct range
    // (or not a number, which can happen if the std dev is 0.0, e.g. in all-zero vectors).
    auto const pcc = numerator / ( std::sqrt( stddev1 ) * std::sqrt( stddev2 ) );
    assert(( -1.0 <= pcc && pcc <= 1.0 ) || ( ! std::isfinite( pcc ) ));
    return pcc;
}

double spearmans_rank_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    // Get the ranking of both vectors.
    auto const ranks_a = ranking_fractional( vec_a );
    auto const ranks_b = ranking_fractional( vec_b );

    return pearson_correlation_coefficient( ranks_a, ranks_b );
}

// =================================================================================================
//     Ranking
// =================================================================================================

std::vector<size_t> ranking_standard( std::vector<double> const& vec )
{
    // Prepare result, and get the sorting order of the vector.
    auto result = std::vector<size_t>( vec.size(), 1 );
    auto const order = stable_sort_indices( vec.begin(), vec.end() );

    // Shortcuts for better readability.
    auto ordered_value = [&]( size_t i ){
        return vec[ order[i] ];
    };
    auto ordered_result = [&]( size_t i ) -> size_t& {
        return result[ order[i] ];
    };

    // Calculate ranks.
    for( size_t i = 1; i < vec.size(); ++i ) {

        // Same values get the same rank. The next bigger one continues at the current i.
        if( ordered_value( i ) == ordered_value( i - 1 ) ) {
            ordered_result( i ) = ordered_result( i - 1 );
        } else {
            ordered_result( i ) = i + 1;
        }
    }

    return result;
}

std::vector<size_t> ranking_modified( std::vector<double> const& vec )
{
    // Prepare result, and get the sorting order of the vector.
    auto result = std::vector<size_t>( vec.size(), 1 );
    auto const order = stable_sort_indices( vec.begin(), vec.end() );

    // Shortcuts for better readability.
    auto ordered_value = [&]( size_t i ){
        return vec[ order[i] ];
    };
    auto ordered_result = [&]( size_t i ) -> size_t& {
        return result[ order[i] ];
    };

    // Calculate ranks. The loop variable is incremented at the end.
    for( size_t i = 0; i < vec.size(); ) {

        // Look ahead: How often does the value occur?
        size_t j = 1;
        while( i+j < vec.size() && ordered_value(i+j) == ordered_value(i) ) {
            ++j;
        }

        // Set the j-next entries.
        for( size_t k = 0; k < j; ++k ) {
            ordered_result( i + k ) = i + j;
        }

        // We can skip the j-next loop iterations, as we just set their values
        i += j;
    }

    return result;
}

std::vector<size_t> ranking_dense( std::vector<double> const& vec )
{
    // Prepare result, and get the sorting order of the vector.
    auto result = std::vector<size_t>( vec.size(), 1 );
    auto const order = stable_sort_indices( vec.begin(), vec.end() );

    // Shortcuts for better readability.
    auto ordered_value = [&]( size_t i ){
        return vec[ order[i] ];
    };
    auto ordered_result = [&]( size_t i ) -> size_t& {
        return result[ order[i] ];
    };

    // Calculate ranks.
    for( size_t i = 1; i < vec.size(); ++i ) {

        // Same values get the same rank. The next bigger one continues by incrementing.
        if( ordered_value( i ) == ordered_value( i - 1 ) ) {
            ordered_result( i ) = ordered_result( i - 1 );
        } else {
            ordered_result( i ) = ordered_result( i - 1 ) + 1;
        }
    }

    return result;
}

std::vector<size_t> ranking_ordinal( std::vector<double> const& vec )
{
    // Prepare result, and get the sorting order of the vector.
    auto result = std::vector<size_t>( vec.size(), 1 );
    auto const order = stable_sort_indices( vec.begin(), vec.end() );

    // Shortcuts for better readability.
    auto ordered_result = [&]( size_t i ) -> size_t& {
        return result[ order[i] ];
    };

    // Calculate ranks. This is simply the order plus 1 (as ranks are 1-based).
    for( size_t i = 0; i < vec.size(); ++i ) {
        ordered_result( i ) = i + 1;
    }

    return result;
}

std::vector<double> ranking_fractional( std::vector<double> const& vec )
{
    // Prepare result, and get the sorting order of the vector.
    auto result = std::vector<double>( vec.size(), 1 );
    auto const order = stable_sort_indices( vec.begin(), vec.end() );

    // Shortcuts for better readability.
    auto ordered_value = [&]( size_t i ){
        return vec[ order[i] ];
    };
    auto ordered_result = [&]( size_t i ) -> double& {
        return result[ order[i] ];
    };

    // Calculate the average of the sum of numbers in the given inclusive range.
    auto sum_avg = []( size_t l, size_t r )
    {
        assert( l <= r );

        // Example:  l == 7, r == 9
        // We want:  (7 + 8 + 9) / 3 = 8.0
        // Upper:    1+2+3+4+5+6+7+8+9 = 45
        // Lower:    1+2+3+4+5+6       = 21
        // Diff:     45 - 21 = 24
        // Count:    9 - 7 + 1 = 3
        // Result:   24 / 3 = 8
        auto const upper = r * ( r + 1 ) / 2;
        auto const lower = ( l - 1 ) * l / 2;
        return static_cast<double>( upper - lower ) / static_cast<double>( r - l + 1 );
    };

    // Calculate ranks. The loop variable is incremented at the end.
    for( size_t i = 0; i < vec.size(); ) {

        // Look ahead: How often does the value occur?
        size_t j = 1;
        while( i+j < vec.size() && ordered_value(i+j) == ordered_value(i) ) {
            ++j;
        }

        // Set the j-next entries.
        auto entry = sum_avg( i + 1, i + j );
        for( size_t k = 0; k < j; ++k ) {
            ordered_result( i + k ) = entry;
        }

        // We can skip the j-next loop iterations, as we just set their values
        i += j;
    }

    return result;
}

} // namespace utils
} // namespace genesis
