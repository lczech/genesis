/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/dataframe/reader.hpp"

using namespace genesis::utils;

TEST( Math, SimpleLinearRegression )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/linear_regression.csv";
    auto reader = DataframeReader<double>();
    reader.names_from_first_col(false);
    auto const data = reader.read( from_file( infile ));

    // Perform regression
    auto const slr = simple_linear_regression(
        data["x"].begin(), data["x"].end(),
        data["y"].begin(), data["y"].end()
    );

    // Check results
    EXPECT_NEAR( 0.67483, slr.slope,     0.00001 );
    EXPECT_NEAR( 1.09682, slr.intercept, 0.00001 );
}
