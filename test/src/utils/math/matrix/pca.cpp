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

#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/math/matrix/pca.hpp"

#include <string>
#include <iostream>

using namespace genesis;
using namespace utils;

inline Matrix<double> read_pca_csv_data( std::string const& filename, size_t rows, size_t cols )
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

TEST( Matrix, PCA )
{
    NEEDS_TEST_DATA;

    // Using the Iris data set of https://archive.ics.uci.edu/ml/datasets/Iris
    // and checking against the results of
    // http://sebastianraschka.com/Articles/2015_pca_in_3_steps.html

    // Read Iris dataset.
    auto data = read_pca_csv_data( "utils/matrix/iris.data.csv", 150, 4 );

    // Run PCA.
    auto pca = principal_component_analysis( data, 2 );

    // Check eigenvectors.
    auto expected_eigenvectors = Matrix<double>( 4, 2, {
        -0.522372, 0.372318,
        0.263355,  0.925556,
        -0.581254, 0.021095,
        -0.565611, 0.065416
    });

    ASSERT_EQ( expected_eigenvectors.rows(), pca.eigenvectors.rows() );
    ASSERT_EQ( expected_eigenvectors.cols(), pca.eigenvectors.cols() );

    for( size_t r = 0; r < expected_eigenvectors.rows(); ++r ) {
        for( size_t c = 0; c < expected_eigenvectors.cols(); ++c ) {
            EXPECT_NEAR( expected_eigenvectors( r, c ), pca.eigenvectors( r, c ), 0.000001);
        }
    }

    // Check eigenvalues.
    auto expected_eigenvalues = std::vector<double>({
        2.91081808375, 0.921220930707
    });

    ASSERT_EQ( expected_eigenvalues.size(), pca.eigenvalues.size() );

    for( size_t i = 0; i < expected_eigenvalues.size(); ++i ) {
        EXPECT_NEAR( expected_eigenvalues[ i ], pca.eigenvalues[ i ], 0.000001);
    }

    // Check projection.
    auto expected_projection = read_pca_csv_data( "utils/matrix/iris.pca_projection.csv", 150, 2 );

    ASSERT_EQ( expected_projection.rows(), pca.projection.rows() );
    ASSERT_EQ( expected_projection.cols(), pca.projection.cols() );

    for( size_t r = 0; r < expected_projection.rows(); ++r ) {
        for( size_t c = 0; c < expected_projection.cols(); ++c ) {
            EXPECT_NEAR( expected_projection( r, c ), pca.projection( r, c ), 0.000001);
        }
    }

    // printf("\nEigenvalues:\n");
    // for( auto val : pca.eigenvalues ) {
    //     printf("%18.5f\n", val );
    // }
    //
    // printf("\nEigenvectors:\n");
    // for( size_t r = 0; r < pca.eigenvectors.rows(); ++r ) {
    //     for( size_t c = 0; c < pca.eigenvectors.cols(); ++c ) {
    //         printf("%12.4f", pca.eigenvectors(r,c));
    //     }
    //     printf("\n");
    // }
    //
    // printf("\nProjections of row-points on first 3 prin. comps.:\n");
    // for (size_t i = 0; i < pca.projection.rows(); i++) {
    //     for (size_t j = 0; j < pca.projection.cols(); j++)  {
    //         printf("%12.4f", pca.projection(i,j));
    //     }
    //     printf("\n");
    // }
}
