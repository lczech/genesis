/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include "lib/utils/formats/csv/reader.hpp"
#include "lib/utils/math/matrix/pca.hpp"

#include <string>
#include <iostream>

using namespace genesis;
using namespace utils;

TEST( Math, PCA )
{
    NEEDS_TEST_DATA;

    // Using the Iris data set of https://archive.ics.uci.edu/ml/datasets/Iris
    // and checking against the results of
    // http://sebastianraschka.com/Articles/2015_pca_in_3_steps.html

    // Read Iris dataset.
    auto reader = CsvReader();
    std::string infile = environment->data_dir + "utils/pca/iris.data.csv";
    auto        table  = reader.from_file( infile );

    ASSERT_EQ( 150, table.size() );
    ASSERT_EQ(   5, table[0].size() );

    // Create data matrix.
    auto data = Matrix<double>( 150, 4 );
    for( size_t i = 0; i < 150; ++i ) {
        for( size_t j = 0; j < 4; ++j ) {
            data( i, j ) = stod( table[i][j] );
        }
    }

    principal_component_analysis( data );
}
