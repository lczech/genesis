/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/range_minimum_query.hpp"

using namespace genesis;
using namespace utils;

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Helper function that does the native RMQ calculation.
 */
size_t rmq_naive (
    std::vector< RangeMinimumQuery::IntType > const& array,
    size_t i,
    size_t j
) {
    size_t naive_min = j;
    for (size_t x = i; x < j; x++) {
        if (array[x] < array[naive_min]) {
            naive_min = x;
        }
    }
    return naive_min;
}

// =================================================================================================
//     Main Tests
// =================================================================================================

TEST( RangeMinimumQuery, Correctness )
{
    // Construct test array.
    size_t n = 10000;
    srand((unsigned) time(NULL));
	auto array = std::vector< RangeMinimumQuery::IntType >( n );
	for (size_t i = 0; i < n; i++) {
        array[i] = rand();
    }

    // Preprocessing.
   	RangeMinimumQuery rmq(array);

	// Test some combinations.
	size_t i, j;
	for (size_t x = 0; x < 1000; x++) {
		i = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
  		j = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
  		if (i > j) { size_t tmp=i; i=j;j=tmp; }

        // Compare.
        auto naive_min = array[ rmq_naive( array, i, j ) ];
        auto rmq_min   = array[ rmq.query(i,j) ];
        EXPECT_EQ( naive_min, rmq_min ) << " at indices " << i << ", " << j;

        // Check if interval of length 1 works.
        EXPECT_EQ( i, rmq.query(i,i) );
	}

    EXPECT_THROW( rmq.query( 1, 0 ), std::invalid_argument );
}

/*
TEST( RangeMinimumQuery, Speed )
{
    // Construct test array.
    size_t n = 500000;
    LOG_TIME << "Constructing array with " << n << " entries...";
    srand((unsigned) time(NULL));
    auto array = std::vector< RangeMinimumQuery::IntType >( n );
    for (size_t i = 0; i < n; i++) {
        array[i] = rand();
    }
    LOG_TIME << "Done.";

    // Preprocessing
    LOG_TIME << "Preprocessing...";
    RangeMinimumQuery rmq(array);
    LOG_TIME << "Done.";

    size_t i, j;

    // Naive.
    LOG_TIME << "Calculating with naive method.";
    for (size_t x = 0; x < 1000; x++) {
        i = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
        j = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
        if (i > j) { size_t tmp=i; i=j;j=tmp; }
        rmq_naive( array, i, j );
    }
    LOG_TIME << "Done.";

    // RMQ.
    LOG_TIME << "Calculating with RMQ.";
    for (size_t x = 0; x < 1000; x++) {
        i = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
        j = (size_t) (n * ((float) rand() / (RAND_MAX + 1.0)));
        if (i > j) { size_t tmp=i; i=j;j=tmp; }
        rmq.query(i,j);
    }
    LOG_TIME << "Done.";
}
*/
