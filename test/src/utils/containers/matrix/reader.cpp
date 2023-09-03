/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <sstream>
#include <limits>

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/reader.hpp"
#include "genesis/utils/containers/matrix/simple_reader.hpp"

using namespace genesis;
using namespace utils;

// ================================================================================================
//     Reader
// ================================================================================================

template<class Reader>
void test_matrix_reader_( Reader const& reader, std::string const& infile )
{
    auto const inf = std::numeric_limits<double>::infinity();
    auto const nan = std::numeric_limits<double>::quiet_NaN();

    auto expected = Matrix<double>( 10, 3, {
        0, 0, 4.5,
        1, 8, 5,
        1, 8, 4.7,
        1, 8, 5.3,
        1, 10, 5.5,
        1, 10, 5.3,
        1, 10, 5.3,
        1, 8, 5.3,
        inf, inf, nan,
        -inf, -inf, -nan
    });

    auto const matrix = reader.read( from_file( infile ));

    // Basic checks.
    ASSERT_EQ( expected.rows(), matrix.rows() );
    ASSERT_EQ( expected.cols(), matrix.cols() );

    // Check values.
    for( size_t r = 0; r < matrix.rows(); ++r ) {
        for( size_t c = 0; c < matrix.cols(); ++c ) {
            if( std::isfinite( expected( r, c ))) {
                EXPECT_NEAR( expected( r, c ), matrix( r, c ), 0.000001);
            }
            if( std::isinf( expected( r, c ))) {
                EXPECT_TRUE( std::isinf( matrix( r, c )));
            }
            if( std::isnan( expected( r, c ))) {
                EXPECT_TRUE( std::isnan( matrix( r, c )));
            }
        }
    }
}

TEST( Matrix, Reader )
{
    NEEDS_TEST_DATA;

    // Read simple matrix.
    auto reader = MatrixReader<double>( " " );
    test_matrix_reader_( reader, environment->data_dir + "utils/matrix/simple.mat" );

    // Change settings and read matrix with tabs, headers, ec.
    reader.csv_reader().separator_chars("\t");
    reader.skip_first_col(true);
    reader.skip_first_row(true);
    test_matrix_reader_( reader, environment->data_dir + "utils/matrix/headers.mat" );
}

TEST( Matrix, SimpleReader )
{
    NEEDS_TEST_DATA;

    // Read simple matrix.
    auto reader = MatrixSimpleReader<double>( ' ' );
    test_matrix_reader_( reader, environment->data_dir + "utils/matrix/simple.mat" );

    // Change settings and read matrix with tabs, headers, ec.
    reader.separator_char( '\t' );
    reader.skip_first_col( true );
    reader.skip_first_row( true );
    test_matrix_reader_( reader, environment->data_dir + "utils/matrix/headers.mat" );
}
