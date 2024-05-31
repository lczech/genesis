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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/distribution.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <functional>

using namespace genesis;
using namespace utils;

TEST( Math, SelectWithoutReplacement )
{
    // Repeated tests, because of randomness
    size_t const num_tests = 1000;
    for( size_t i = 0; i < num_tests; ++i ) {

        // Test all combinations of k and n for some range.
        for( size_t n = 0; n < 10; ++n ) {
            for( size_t k = 0; k <= n; ++k ) {
                // Make the numbers
                auto const sel = select_without_replacement( k, n );

                // Test the numbers
                EXPECT_EQ( k, sel.size() );
                for( auto e : sel ) {
                    EXPECT_LT( e, n );
                }
                EXPECT_FALSE( contains_duplicates( sel ));
            }
        }
    }
}

TEST( Math, SelectWithoutReplacementRand )
{
    size_t const k = 2; // sample size
    size_t const n = 5; // population size
    size_t const r = 10000; // number of runs

    auto histogram = std::vector<size_t>( n, 0 );

    for( size_t i = 0; i < r; ++i ) {
        auto selection = select_without_replacement( k, n );
        for( auto const& s : selection ) {
            ++histogram[ s ];
        }
    }

    // We cannot really test the validity automatically, as it is random.
    // Thus, just do a simple test whether we generated the correct number of elements.
    size_t sum = 0;
    for( size_t i = 0; i < histogram.size(); ++i ) {
        // LOG_DBG << i << ": " << histogram[ i ];
        sum += histogram[ i ];
    }
    EXPECT_EQ( k * r, sum );
}

TEST( Math, PermutedCongruentialGenerator )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    size_t const num_tests = 1000000;
    // size_t cnt_bool = 0;

    for( size_t i = 0; i < num_tests; ++i ) {
        // Test max
        EXPECT_GE( 100, permuted_congruential_generator( 100 ));


        // Test min max
        auto const min_max = permuted_congruential_generator( 10, 100 );
        EXPECT_LE( 10, min_max );
        EXPECT_GE( 100, min_max );

        // cnt_bool += permuted_congruential_generator_bool( 10 );

        // Always true cases
        EXPECT_EQ( 0, permuted_congruential_generator( 0 ));
        EXPECT_EQ( 0, permuted_congruential_generator( 0, 0 ));
        EXPECT_EQ( 1, permuted_congruential_generator( 1, 1 ));
        EXPECT_EQ( 3, permuted_congruential_generator( 3, 3 ));
        EXPECT_TRUE( permuted_congruential_generator_bool( 1 ));
    }

    // LOG_DBG << "cnt_bool " << cnt_bool;

    // Error cases
    EXPECT_ANY_THROW( permuted_congruential_generator( 5, 3 ));
    EXPECT_ANY_THROW( permuted_congruential_generator_bool( 0 ));
}

void test_multivariate_distribution_(
    std::function<std::vector<size_t>(std::vector<size_t> const&, size_t)> const& distrib
) {
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    size_t const num_tests = 10;
    size_t const num_repeats = 10000;

    for( size_t i = 0; i < num_tests; ++i ) {
        // LOG_DBG << "==================";

        // We want to test the distribution with a range of magnitudes. Just random picking
        // is likely to include some higher magnitutes more than not, so we also draw a magnitude,
        // to ensure that some tests stay in small number realm.
        auto const magnitude = permuted_congruential_generator( 1, 10 );

        // Make some weights. We use ints for simplicty.
        auto const p_size = permuted_congruential_generator( 1, 10 );
        auto p = std::vector<size_t>( p_size );
        size_t sum_p = 0;
        for( size_t j = 0; j < p_size; ++j ) {
            p[j] = permuted_congruential_generator( 0, std::pow( 10, magnitude ) );
            sum_p += p[j];
        }

        // Get number of values to draw. We draw from the same order of magnitude, so that we
        // cover a wide range of draws. We might draw without replacement,
        // so we need to make sure to not draw more than there are items in the set.
        size_t n;
        do {
            n = permuted_congruential_generator( 0, std::pow( 10, magnitude ) );
        } while( n > sum_p );

        // Repeated drawing, so that we get an average of the distributions
        // that is close to the original. We currently do not explicitly test the statistical
        // properties of the distributions. That would involve re-implementing those tests from
        // the original GPL code that our implementation is based on, and we trust that they
        // did proper testing. Hence, for simplicity, we here just check that the general setup
        // works and that we get values as expected, by averaging over a large number of draws.
        // As far as I can tell, if this works, then at least the mean of the distribution has
        // to be correct. The variance and other moments and properties of the distribution might
        // still be off, but as said, we leave that testing to the original implementations.
        auto accu = std::vector<size_t>( p_size, 0 );
        for( size_t j = 0; j < num_repeats; ++j ) {
            auto const x = distrib( p, n );
            if( j == 0 ) {
                // LOG_DBG << join(p) << " -(" << n << ")-> " << join(x);
            }

            // Some static tests for invariants of the result.
            auto const sum_x = std::accumulate( x.begin(), x.end(), static_cast<size_t>(0) );
            EXPECT_EQ( p_size, x.size() );
            EXPECT_EQ( sum_x, n );

            // Keep track of the sum of all draws
            for( size_t k = 0; k < p_size; ++k ) {
                accu[k] += x[k];
            }
        }

        // Now test that the average of all draws is close to the original.
        // We normalize the values, and then test that the result is absolutely
        // within a few percent of the expectation. We need to be a bit lenient here,
        // as this is non-deterministic, and we want to avoid spuriously failing tests.
        // In a longer test with num_tests == 10000, we observed only 4 and 6 tests respectively
        // for the multinomial and the hypergeomtric distrbibution where a 1% absolute
        // difference was exceeded (the maximum was 0.0131048 for the hypergeomtric distribution)
        // with the current setup here.
        // So we use 1.5% absolute difference here as a threshold that is good enough for testing
        // accuracy, while also not likely to trigger failing test too often.
        auto const sum_a = std::accumulate( accu.begin(), accu.end(), static_cast<size_t>(0) );
        for( size_t k = 0; k < p_size; ++k ) {
            auto const avg_pk = static_cast<double>(p[k]) / sum_p;
            auto const avg_ak = static_cast<double>(accu[k]) / sum_a;
            // LOG_DBG << avg_pk << " =?= " << avg_ak;

            if( std::isfinite(avg_pk) && std::isfinite(avg_ak) ) {
                EXPECT_LE( std::abs( avg_pk - avg_ak ), 0.015 );
            }
        }
    }
}

TEST( Math, SelectMultinomial )
{
    test_multivariate_distribution_(
        []( std::vector<size_t> const& p, size_t n ) {
            return multinomial_distribution( p, n );
        }
    );
}

TEST( Math, SelectMultivariateHypergeometric )
{
    test_multivariate_distribution_(
        []( std::vector<size_t> const& p, size_t n ) {
            return multivariate_hypergeometric_distribution( p, n );
        }
    );
}
