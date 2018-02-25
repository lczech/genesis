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

#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/dataframe/reader.hpp"

using namespace genesis::utils;

TEST( Containers, DataframeMatrix )
{
    NEEDS_TEST_DATA;

    auto const infile = environment->data_dir + "utils/matrix/headers.mat";

    auto reader = DataframeReader<double>();
    reader.csv_reader().separator_chars( "\t" );
    auto df = reader.from_file( infile );

    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    // Index access
    EXPECT_EQ( 4.5, df( 0, 2 ));
    EXPECT_EQ( 1.0, df( 8, 0 ));

    // Name acces
    EXPECT_EQ( 4.7, df( "gamma", "Third" ));
    EXPECT_EQ( 0.0, df( "kappa", "First" ));

    // for( size_t c = 0; c < df.cols(); ++c ) {
    //     std::cout << df.col_name(c) << " ";
    // }
    // std::cout << "\n";
    //
    // for( size_t r = 0; r < df.rows(); ++r ) {
    //     std::cout << df.row_name( r ) << " ";
    //
    //     for( size_t c = 0; c < df.cols(); ++c ) {
    //         std::cout << df( r, c ) << " ";
    //     }
    //
    //     std::cout << "\n";
    // }
}

TEST( Containers, DataframeCsv )
{
    NEEDS_TEST_DATA;

    auto const infile = environment->data_dir + "utils/csv/table.csv";

    auto reader = DataframeReader<double>();
    auto df = reader.from_file( infile );

    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    // Index access
    EXPECT_EQ( 4.5, df( 0, 2 ));
    EXPECT_EQ( 1.0, df( 8, 0 ));

    // Name acces
    EXPECT_EQ( 4.7, df( "gamma", "Third" ));
    EXPECT_EQ( 0.0, df( "kappa", "First" ));
}
