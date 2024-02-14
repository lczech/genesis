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

#include "genesis/utils/math/correlation.hpp"
#include "genesis/utils/text/string.hpp"

#include <cmath>
#include <ctime>
#include <limits>
#include <random>

using namespace genesis::utils;

// =================================================================================================
//     Spearman's Correlation Coefficient
// =================================================================================================

TEST( Math, SpearmansRankCorrelationCoeffcient )
{
    // Example from https://en.wikipedia.org/wiki/Spearman%27s_rank_correlation_coefficient
    auto iq = std::vector<double>({
        106, 86, 100, 101, 99, 103, 97, 113, 112, 110
    });
    auto tv = std::vector<double>({
        7, 0, 27, 50, 28, 29, 20, 12, 6, 17
    });
    EXPECT_DOUBLE_EQ( -0.17575757575757575, spearmans_rank_correlation_coefficient( iq, tv ) );

    // Add some nan values to it. Nothing should change.
    auto iq2 = std::vector<double>({
        106, 86, (1.0/0.0), 100, 101, 99, 103, 97, 113, 112, 110, (1.0/0.0)
    });
    auto tv2 = std::vector<double>({
        7, 0, 0, 27, 50, 28, 29, 20, 12, 6, 17, 100
    });
    EXPECT_DOUBLE_EQ( -0.17575757575757575, spearmans_rank_correlation_coefficient( iq2, tv2 ) );
}

TEST( Math, FisherTransformation )
{
    double const inf = std::numeric_limits<double>::infinity();

    EXPECT_FLOAT_EQ( -inf,      fisher_transformation( -1 ));
    EXPECT_FLOAT_EQ( -1.47222,  fisher_transformation( -0.9 ));
    EXPECT_FLOAT_EQ( -0.549306, fisher_transformation( -0.5 ));
    EXPECT_FLOAT_EQ( 0.0,       fisher_transformation( 0 ));
    EXPECT_FLOAT_EQ( 0.549306,  fisher_transformation( 0.5 ));
    EXPECT_FLOAT_EQ( 1.47222,   fisher_transformation( 0.9 ));
    EXPECT_FLOAT_EQ( inf,       fisher_transformation( 1 ));
}

// =================================================================================================
//     Kendall's Correlation Coefficient
// =================================================================================================

// Helper to get all verions of the function that we want to test.
std::vector<double> compute_all_taus_(
    std::vector<double> const& x,
    std::vector<double> const& y
){
    std::vector<double> result;
    result.push_back( kendalls_tau_correlation_coefficient_naive( x, y, KendallsTauMethod::kTauA ));
    result.push_back( kendalls_tau_correlation_coefficient( x, y, KendallsTauMethod::kTauA ));
    result.push_back( kendalls_tau_correlation_coefficient_naive( x, y, KendallsTauMethod::kTauB ));
    result.push_back( kendalls_tau_correlation_coefficient( x, y, KendallsTauMethod::kTauB ));
    result.push_back( kendalls_tau_correlation_coefficient_naive( x, y, KendallsTauMethod::kTauC ));
    result.push_back( kendalls_tau_correlation_coefficient( x, y, KendallsTauMethod::kTauC ));
    return result;
}

TEST( Math, KendallsTauCorrelationCoeffcient )
{
    // Boundary test cases
    {
        // List too short or only ties
        EXPECT_TRUE( std::isnan( kendalls_tau_correlation_coefficient( {}, {} )));
        EXPECT_TRUE( std::isnan( kendalls_tau_correlation_coefficient( {1}, {2} )));
        EXPECT_TRUE( std::isnan( kendalls_tau_correlation_coefficient( {0, 0}, {0, 0} )));
        EXPECT_TRUE( std::isnan( kendalls_tau_correlation_coefficient( {1, 1}, {2, 2} )));

        // Perfect correlation
        EXPECT_FLOAT_EQ( +1.0, kendalls_tau_correlation_coefficient( {1, 2}, {3, 4} ) );
        EXPECT_FLOAT_EQ( -1.0, kendalls_tau_correlation_coefficient( {1, 2}, {2, 1} ) );
    }

    // Simple test case
    {
        // std::vector<double> x = {0.1, 0.2, 0.3, 0.4};
        // std::vector<double> y = {1, 3, 2, 4};

        // Expected: 0.4666666
        std::vector<double> x = {3, 1, 4, 2, 6, 5};
        std::vector<double> y = {1, 2, 3, 4, 5, 6};

        // For this basic example without ties and other shennenigans,
        // all three versions yield the same result.
        auto const result = compute_all_taus_( x, y );
        EXPECT_FLOAT_EQ( 0.4666666666, result[0] );
        EXPECT_FLOAT_EQ( 0.4666666666, result[1] );
        EXPECT_FLOAT_EQ( 0.4666666666, result[2] );
        EXPECT_FLOAT_EQ( 0.4666666666, result[3] );
        EXPECT_FLOAT_EQ( 0.4666666666, result[4] );
        EXPECT_FLOAT_EQ( 0.4666666666, result[5] );

        // Also test the iterator-based overload
        auto const other = kendalls_tau_correlation_coefficient(
            x.begin(), x.end(), y.begin(), y.end()
        );
        EXPECT_FLOAT_EQ( 0.4666666666, other );
    }

    // Faulty values
    {
        std::vector<double> x{ 583.383, 599.453, 965.732, 503.768 };
        std::vector<double> y{ 254.717, 734.824, 197.133, 30.4815 };

        // std::vector<double> x{ 58, 59, 96, 50 };
        // std::vector<double> y{ 25, 73, 19, 3 };

        // std::vector<double> x{ 5, 6, 8, 4 };
        // std::vector<double> y{ 3, 7, 2, 1 };

        // For this basic example without ties and other shennenigans,
        // all three versions yield the same result.
        auto const result = compute_all_taus_( x, y );
        EXPECT_FLOAT_EQ( 0.333333333, result[0] );
        EXPECT_FLOAT_EQ( 0.333333333, result[1] );
        EXPECT_FLOAT_EQ( 0.333333333, result[2] );
        EXPECT_FLOAT_EQ( 0.333333333, result[3] );
        EXPECT_FLOAT_EQ( 0.333333333, result[4] );
        EXPECT_FLOAT_EQ( 0.333333333, result[5] );
    }
}

TEST( Math, KendallsTauCorrelationCoeffcientFuzzy )
{
    // Number and size of tests
    size_t const num_fuzzy_tests = 100;
    size_t const num_elem = 100;

    // Setup random generator
    auto const seed = std::time(nullptr);
    std::default_random_engine engine( seed );
    std::uniform_int_distribution<> rand_bool(0,1);
    std::uniform_int_distribution<> rand_nan(0,10);

    // Run with different maxima, and rounding or not,
    // so that we test ties more thoroughly.
    std::vector<double> max_vals{ 10.0, 1000.0 };
    for( auto rand_max : max_vals ) {
        for( size_t n = 0; n < num_fuzzy_tests; ++n ) {
            std::uniform_real_distribution<double> rand_double( 0.0, rand_max );
            bool const do_round = rand_bool(engine);
            bool const with_nans = rand_bool(engine);

            // Data vectors
            std::vector<double> x( num_elem );
            std::vector<double> y( num_elem );

            // Generate random numbers for fuzzy testing
            for( size_t i = 0; i < num_elem; ++i ) {
                x[i] = rand_double(engine);
                y[i] = rand_double(engine);
                if( do_round ) {
                    x[i] = std::round(x[i]);
                    y[i] = std::round(y[i]);
                }
                if( with_nans && rand_nan(engine) == 0 ) {
                    x[i] = std::numeric_limits<double>::quiet_NaN();
                }
                if( with_nans && rand_nan(engine) == 0 ) {
                    y[i] = std::numeric_limits<double>::quiet_NaN();
                }
            }

            // LOG_DBG << "rand_max " << rand_max;
            // LOG_DBG << " do_round " << do_round;
            // LOG_DBG << " with_nans " << with_nans;
            // LOG_DBG << "x " << genesis::utils::join( x );
            // LOG_DBG << "y " << genesis::utils::join( y );
            // LOG_BOLD;

            // Test the naive (slow) algorithm against Knight's algorithm, for all adjustments.
            auto const result_xy = compute_all_taus_( x, y );
            EXPECT_FLOAT_EQ( result_xy[0], result_xy[1] ) << "seed: " << seed;
            EXPECT_FLOAT_EQ( result_xy[2], result_xy[3] ) << "seed: " << seed;
            EXPECT_FLOAT_EQ( result_xy[4], result_xy[5] ) << "seed: " << seed;

            // Also test symmetry.
            auto const result_yx = compute_all_taus_( y, x );
            for( size_t i = 0; i < result_xy.size(); ++i ) {
                EXPECT_FLOAT_EQ( result_xy[i], result_yx[i] ) << "seed: " << seed;
            }
        }
    }
}

TEST( Math, KendallsTauCorrelationCoeffcientTies )
{
    // Real world data with a lot of ties...
    std::vector<double> meta{
        0, 8, 8, 8, 10, 10, 10, 8, 1, 0, 8, 8, 0, 8, 9, 9, 8, 0, 8, 10, 6, 10, 8, 8, 8, 8, 0, 10, 8,
        1, 7, 8, 8, 8, 7, 8, 0, 0, 0, 8, 8, 3, 0, 8, 1, 0, 0, 2, 8, 8, 0, 8, 0, 8, 8, 0, 0, 0, 10,
        0, 0, 1, 0, 0, 0, 0, 8, 7, 8, 0, 8, 8, 10, 8, 8, 2, 7, 0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 8, 8,
        10, 8, 8, 0, 8, 10, 0, 0, 6, 4, 10, 0, 8, 8, 10, 0, 1, 8, 0, 8, 0, 1, 0, 0, 2, 6, 4, 0, 1,
        10, 8, 0, 7, 8, 8, 8, 0, 9, 0, 1, 10, 8, 8, 0, 8, 8, 4, 5, 8, 0, 0, 10, 8, 0, 8, 7, 8, 8, 8,
        9, 8, 5, 8, 0, 2, 7, 10, 8, 0, 0, 8, 0, 0, 8, 10, 8, 8, 0, 8, 0, 8, 0, 10, 8, 8, 4, 6, 8, 8,
        0, 8, 8, 4, 0, 0, 9, 0, 0, 4, 7, 5, 8, 10, 8, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8, 0, 9, 0, 0, 0,
        0, 8, 0, 0, 8, 8, 3, 8, 0, 3, 0, 8
    };
    std::vector<double> edge{
        -1, -0.96, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -0.99, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -0.99, -1, -1, -1, -1, -1,
        -0.98, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -0.97, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    auto const result = compute_all_taus_( meta, edge );
    EXPECT_FLOAT_EQ( result[0], result[1] );
    EXPECT_FLOAT_EQ( result[2], result[3] );
    EXPECT_FLOAT_EQ( result[4], result[5] );
}

// TEST( Math, KendallsTauCorrelationCoeffcientMem )
// {
//     // Number and size of tests
//     size_t const num_elem = 1000000;
//
//     // Setup random generator
//     auto const seed = std::time(nullptr);
//     std::default_random_engine engine( seed );
//     std::uniform_int_distribution<> rand_nan(0,10);
//     std::uniform_real_distribution<double> rand_double( 0.0, 1000.0 );
//
//     // Data vectors
//     std::vector<double> x( num_elem );
//     std::vector<double> y( num_elem );
//
//     // Generate random numbers for fuzzy testing
//     for( size_t i = 0; i < num_elem; ++i ) {
//         x[i] = rand_double(engine);
//         y[i] = rand_double(engine);
//         if( rand_nan(engine) == 0 ) {
//             x[i] = std::numeric_limits<double>::quiet_NaN();
//         }
//         if( rand_nan(engine) == 0 ) {
//             y[i] = std::numeric_limits<double>::quiet_NaN();
//         }
//     }
//
//     LOG_DBG << kendalls_tau_correlation_coefficient( x, y );
// }
