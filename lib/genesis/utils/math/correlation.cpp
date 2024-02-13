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

#include "genesis/utils/math/correlation.hpp"

#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

/**
 * @brief Helper function for kendalls_tau_correlation_coefficient() to count the number
 * of tied pairs and the number of unique values in a list.
 */
std::pair<size_t, size_t> kendalls_tau_count_ties_and_uniques_(
    std::vector<double> const& data
) {
    // Collect all unique values, counting how often each of them occurs.
    std::unordered_map<double, size_t> unique_counts;
    for( auto val : data ) {
        ++unique_counts[val];
    }

    // The number of ties for the purposes of the algorithm needs to account for the duplicates
    // occurring in all combinations of pairs, so we use a triangular number.
    size_t tie_sum = 0;
    for(  auto const& pair : unique_counts ) {
        if( pair.second > 1 ) {
            tie_sum += pair.second * (pair.second - 1) / 2;
        }
    }
    return { tie_sum, unique_counts.size() };
}

/**
 * @brief Given counts of concordant and discordant pairs, compute the final value,
 * applying the requested adjustments.
 *
 * We take @p concordant and @p discordant as `double` here, as we only need those in the function.
 */
double kendalls_tau_method_(
    std::vector<double> const& x,
    std::vector<double> const& y,
    size_t concordant, size_t discordant,
    KendallsTauMethod method
) {
    double tau = std::numeric_limits<double>::quiet_NaN();
    size_t const n = x.size();
    assert( concordant + discordant == n * (n - 1) / 2 );

    // Compute the numerator, common to all tau methods.
    // We first cast to signed int, so that the difference computation is exact,
    // and then cast the results to double, as we are doing a division next.
    double const num = static_cast<int64_t>(concordant) - static_cast<int64_t>(discordant);

    switch( method ) {
        case KendallsTauMethod::kTauA: {
            double const den = concordant + discordant;
            if( den != 0.0 ) {
                tau = num / den;
            }
            break;
        }

        case KendallsTauMethod::kTauB: {
            // Total number of pair combinations, and numbers of ties in pairs of x and y.
            // We cannot have more ties in the pairs than there are pairs, which we assert.
            uint64_t const n0 = n * (n - 1) / 2;
            uint64_t const n1 = kendalls_tau_count_ties_and_uniques_(x).first;
            uint64_t const n2 = kendalls_tau_count_ties_and_uniques_(y).first;
            assert( n0 >= n1 && n0 >= n2 );

            // Use these to compute the Tau-b denominator.
            // We compute the differences in ints, so that they are exact, but then convert
            // to double so that the multiplication does not overflow.
            double const den = std::sqrt(
                static_cast<double>(n0 - n1) * static_cast<double>(n0 - n2)
            );
            if( std::isfinite(den) && den != 0.0 ) {
                tau = num / den;
            }
            break;
        }

        case KendallsTauMethod::kTauC: {
            // Number of unique values in x and y, and their minimum.
            double const u1 = kendalls_tau_count_ties_and_uniques_(x).second;
            double const u2 = kendalls_tau_count_ties_and_uniques_(y).second;
            double const m = std::min( u1, u2 );

            double const den = squared(n) * ( m - 1 ) / m;
            if( std::isfinite(den) && den != 0.0 && m > 0 ) {
                tau = 2.0 * num / den;
            }
            break;
        }

        default: {
            throw std::invalid_argument( "Invalid value for KendallsTauMethod" );
        }
    }

    return tau;
}

// =================================================================================================
//     Kendall Tau Correlation Coefficient
// =================================================================================================

/**
 * @brief Helper function for kendalls_tau_correlation_coefficient() to do a merge sort that
 * counts the number of inversions performed.
 */
size_t kendalls_tau_merge_count_(
    std::vector<double>& data, std::vector<double>& temp,
    size_t left, size_t mid, size_t right
) {
    size_t i = left;
    size_t j = mid;
    size_t k = left;
    size_t inversions = 0;

    // Merge sort, counting inversions (as if we were doing bubble sort)
    while( i < mid && j <= right ) {
        if( data[i] <= data[j] ) {
            temp[k++] = data[i++];
        } else {
            temp[k++] = data[j++];
            inversions += mid - i;
        }
    }

    // Copy the remaining elements
    while( i < mid ) {
        temp[k++] = data[i++];
    }
    while( j <= right ) {
        temp[k++] = data[j++];
    }

    // Copy back to the original vector
    for( i = left; i <= right; ++i ) {
        data[i] = temp[i];
    }

    return inversions;
}

/**
 * @brief Helper function for kendalls_tau_correlation_coefficient() to sort a list using merge
 * sort, while counting the number of inversions performed.
 */
size_t kendalls_tau_sort_and_count_(
    std::vector<double>& data, std::vector<double>& temp, size_t left, size_t right
) {
    if( left >= right ) {
        return 0;
    }

    // Count the number of inversions done by merge sorting the list.
    size_t const mid = left + (right - left) / 2;
    size_t const inv_l = kendalls_tau_sort_and_count_( data, temp, left, mid );
    size_t const inv_r = kendalls_tau_sort_and_count_( data, temp, mid + 1, right );
    size_t const inv_m = kendalls_tau_merge_count_( data, temp, left, mid + 1, right );

    return inv_l + inv_r + inv_m;
}

double kendalls_tau_correlation_coefficient(
    std::vector<double> const& x,
    std::vector<double> const& y,
    KendallsTauMethod method
) {
    // Errors and boundary cases
    if( x.size() != y.size() ) {
        throw std::invalid_argument(
            "kendalls_tau_correlation_coefficient: Input with differing numbers of elements."
        );
    }
    if( x.size() < 2 ) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Create a vector of indices sorted by the corresponding values in x
    size_t const n = x.size();
    std::vector<size_t> rank_x(n);
    std::iota( rank_x.begin(), rank_x.end(), 0 );
    std::sort( rank_x.begin(), rank_x.end(), [&]( size_t i, size_t j ) { return x[i] < x[j]; } );

    // Create a vector of y values sorted according to x
    std::vector<double> sorted_y(n);
    for( size_t i = 0; i < n; ++i ) {
        sorted_y[i] = y[rank_x[i]];
    }

    // Use merge sort to count inversions in sorted_y, which are discordant pairs.
    // We use a temporary vector for merge sort, to avoid re-allocating memory in each step.
    std::vector<double> temp(n);
    size_t const discordant = kendalls_tau_sort_and_count_( sorted_y, temp, 0, n - 1 );
    assert( std::is_sorted( temp.begin(), temp.end() ));

    // The number of concordant pairs is simply the total number of pairs minus the discordant.
    size_t const n0 = n * (n - 1) / 2;
    size_t const concordant = n0 - discordant;

    // Compute the final value, using corrections as needed.
    return kendalls_tau_method_( x, y, concordant, discordant, method );
}

// =================================================================================================
//     Kendall Tau Correlation Coefficient Naive
// =================================================================================================

double kendalls_tau_correlation_coefficient_naive(
    std::vector<double> const& x,
    std::vector<double> const& y,
    KendallsTauMethod method
) {
    // Boundary checks.
    if( x.size() != y.size() ) {
        throw std::invalid_argument(
            "kendalls_tau_correlation_coefficient: Input with differing numbers of elements."
        );
    }

    size_t const n = x.size();
    size_t concordant = 0;
    size_t discordant = 0;

    // Iterate through all pairs of indices and accumulate concordant and discordant pairs.
    for( size_t i = 0; i < n - 1; ++i ) {
        for( size_t j = i + 1; j < n; ++j ) {
            double const dx = x[i] - x[j];
            double const dy = y[i] - y[j];
            double const sign_prod = dx * dy;

            if( sign_prod > 0.0 ) {
                ++concordant;
            } else if( sign_prod < 0.0 ) {
                ++discordant;
            }
            // If sign_prod == 0, it's either a tie or one of the differences is 0, so we ignore it
        }
    }

    // Compute the final value, using corrections as needed.
    return kendalls_tau_method_( x, y, concordant, discordant, method );
}

} // namespace utils
} // namespace genesis
