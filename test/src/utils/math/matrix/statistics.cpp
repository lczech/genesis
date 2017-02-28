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
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <sstream>

#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"
#include "genesis/utils/math/matrix/statistics.hpp"

using namespace genesis;
using namespace utils;

// ================================================================================================
//     Standardization
// ================================================================================================

inline Matrix<double> read_stats_csv_data( std::string const& filename, size_t rows, size_t cols )
{
    // Read Iris dataset.
    auto        reader = CsvReader();
    std::string infile = environment->data_dir + filename;
    auto        table  = reader.from_file( infile );

    // Create data matrix.
    auto data = Matrix<double>( rows, cols );
    for( size_t i = 0; i < rows; ++i ) {
        for( size_t j = 0; j < cols; ++j ) {
            data( i, j ) = stod( table[i][j] );
        }
    }

    return data;
}

inline void test_check_standardization_results(
    Matrix<double> expected,
    Matrix<double> actual,
    std::vector<MeanStddevPair> stats
) {
    ASSERT_EQ( 150, expected.rows() );
    ASSERT_EQ(   4, expected.cols() );
    ASSERT_EQ( 150, actual.rows() );
    ASSERT_EQ(   4, actual.cols() );
    ASSERT_EQ(   4, stats.size() );

    // Check standardized values.
    for( size_t r = 0; r < actual.rows(); ++r ) {
        for( size_t c = 0; c < actual.cols(); ++c ) {
            EXPECT_NEAR( expected( r, c ), actual( r, c ), 0.000001);
        }
    }

    // Check stats.
    std::vector<double> means   = { 5.843333, 3.054,    3.758667, 1.198667 };
    std::vector<double> stddevs = { 0.825301, 0.432147, 1.758529, 0.760613 };
    for( size_t i = 0; i < 4; ++i ) {
        EXPECT_NEAR( means[i],   stats[i].mean,   0.000001 );
        EXPECT_NEAR( stddevs[i], stats[i].stddev, 0.000001 );
    }
}

TEST( Matrix, Standardize )
{
    // Read Iris dataset and expected standardized data.
    NEEDS_TEST_DATA;
    auto data    = read_stats_csv_data( "utils/matrix/iris.data.csv", 150, 4 );
    auto stddata = read_stats_csv_data( "utils/matrix/iris.standardized.csv", 150, 4 );

    // Run standardization and check.
    auto stats = standardize_cols( data );
    test_check_standardization_results( stddata, data, stats );
}

TEST( Matrix, StandardizeMeans )
{
    // Read Iris dataset and expected standardized data.
    NEEDS_TEST_DATA;
    auto data    = read_stats_csv_data( "utils/matrix/iris.data.csv", 150, 4 );
    auto stddata = read_stats_csv_data( "utils/matrix/iris.standardized_means.csv", 150, 4 );

    auto stats = standardize_cols( data, true, false );
    test_check_standardization_results( stddata, data, stats );
}

TEST( Matrix, StandardizeStddevs )
{
    // Read Iris dataset and expected standardized data.
    NEEDS_TEST_DATA;
    auto data    = read_stats_csv_data( "utils/matrix/iris.data.csv", 150, 4 );
    auto stddata = read_stats_csv_data( "utils/matrix/iris.standardized_stddevs.csv", 150, 4 );

    auto stats = standardize_cols( data, false, true );
    test_check_standardization_results( stddata, data, stats );
}

// ================================================================================================
//     Correlation and Covariance
// ================================================================================================

TEST( Matrix, Correlation )
{
    // Read Iris dataset and expected standardized data.
    NEEDS_TEST_DATA;
    auto data = read_stats_csv_data( "utils/matrix/iris.data.csv", 150, 4 );

    auto actual = correlation_matrix( data );

    auto expected = Matrix<double>( 4, 4, {
        1, -0.109369, 0.871754, 0.817954,
        -0.109369, 1, -0.420516, -0.356544,
        0.871754, -0.420516, 1, 0.962757,
        0.817954, -0.356544, 0.962757, 1
    });

    ASSERT_EQ( expected.rows(), actual.rows() );
    ASSERT_EQ( expected.cols(), actual.cols() );

    // Check values.
    for( size_t r = 0; r < actual.rows(); ++r ) {
        for( size_t c = 0; c < actual.cols(); ++c ) {
            EXPECT_NEAR( expected( r, c ), actual( r, c ), 0.000001);
        }
    }
}

TEST( Matrix, Covariance )
{
    // Read Iris dataset and expected standardized data.
    NEEDS_TEST_DATA;
    auto data = read_stats_csv_data( "utils/matrix/iris.data.csv", 150, 4 );

    auto actual = covariance_matrix( data );

    auto expected = Matrix<double>( 4, 4, {
        0.681122, -0.039007, 1.265191, 0.513458,
        -0.039007, 0.186751, -0.319568, -0.117195,
        1.265191, -0.319568, 3.092425, 1.287745,
        0.513458, -0.117195, 1.287745, 0.578532
    });

    ASSERT_EQ( expected.rows(), actual.rows() );
    ASSERT_EQ( expected.cols(), actual.cols() );

    // Check values.
    for( size_t r = 0; r < actual.rows(); ++r ) {
        for( size_t c = 0; c < actual.cols(); ++c ) {
            EXPECT_NEAR( expected( r, c ), actual( r, c ), 0.000001);
        }
    }
}
