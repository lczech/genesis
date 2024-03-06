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

TEST( Math, SelectMultinomial )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    size_t const num_tests = 10000;
    for( size_t i = 0; i < num_tests; ++i ) {

        // Make some weights. We use ints for simplicty.
        auto const p_size = 5;
        auto p = std::vector<double>( p_size );
        for( size_t j = 0; j < p_size; ++j ) {
            p[j] = permuted_congruential_generator( 0, 10000 );
        }

        // Draw some values.
        auto const n = permuted_congruential_generator( 0, 12500 );
        auto const x = multinomial_distribution( p, n );

        // LOG_DBG << join(p) << " -(" << n << ")-> " << join(x);

        // Some static tests for invariants of the result.
        auto const sum_x = std::accumulate( x.begin(), x.end(), static_cast<size_t>(0) );
        EXPECT_EQ( p_size, x.size() );
        EXPECT_EQ( sum_x, n );
    }
}

TEST( Math, SelectMultivariateHypergeometric )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    size_t const num_tests = 10000;
    for( size_t i = 0; i < num_tests; ++i ) {

        // Make some weights. We use ints for simplicty.
        auto const p_size = permuted_congruential_generator( 1, 10 );
        auto p = std::vector<size_t>( p_size );
        size_t sum_p = 0;
        for( size_t j = 0; j < p_size; ++j ) {
            p[j] = permuted_congruential_generator( 0, 1000 );
            sum_p += p[j];
        }

        // Draw some values. We draw without replacement, so we need to make sure
        // to not draw more than there are items in the set.
        size_t n;
        do {
            n = permuted_congruential_generator( 0, 1250 );
        } while( n > sum_p );
        auto const x = multivariate_hypergeometric_distribution( p, n );

        // LOG_DBG << join(p) << " -(" << n << ")-> " << join(x);

        // Some static tests for invariants of the result.
        auto const sum_x = std::accumulate( x.begin(), x.end(), static_cast<size_t>(0) );
        EXPECT_EQ( p_size, x.size() );
        EXPECT_EQ( sum_x, n );
    }
}
