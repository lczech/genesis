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

#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"

using namespace genesis;
using namespace utils;

TEST(Matrix, Copy)
{
    auto m1 = Matrix<int>(2, 2, {1, 2, 3, 4});

    auto m2 = m1;
    auto m3(m1);

    EXPECT_EQ(m1, m2);
    EXPECT_EQ(m1, m3);

    EXPECT_TRUE(m1 == m2);
    EXPECT_TRUE(m1 == m3);

    EXPECT_FALSE(m1 != m2);
    EXPECT_FALSE(m1 != m3);

    m1(0, 1) = 9;

    EXPECT_NE(m1, m2);
    EXPECT_NE(m1, m3);

    EXPECT_FALSE(m1 == m2);
    EXPECT_FALSE(m1 == m3);

    EXPECT_TRUE(m1 != m2);
    EXPECT_TRUE(m1 != m3);

    EXPECT_EQ(9, m1(0, 1));
    EXPECT_EQ(2, m2(0, 1));
    EXPECT_EQ(2, m3(0, 1));
}

TEST(Matrix, InitializerList)
{
    EXPECT_THROW(
        Matrix<int>(2, 2, {1, 2, 3}),
        std::length_error
    );

    EXPECT_NO_THROW(
        Matrix<int>(2, 2, {1, 2, 3, 4})
    );

    EXPECT_THROW(
        Matrix<int>(2, 2, {1, 2, 3, 4, 5}),
        std::length_error
    );
}

TEST(Matrix, Access)
{
    auto m = Matrix<int>(2, 2, {1, 2, 3, 4});

    EXPECT_EQ(2, m.rows());
    EXPECT_EQ(2, m.cols());

    EXPECT_EQ(2, m(0, 1));

    EXPECT_THROW(
        m.at(0, 2),
        std::out_of_range
    );
}

TEST( Matrix, SwapRowsCols )
{
    auto m = Matrix<int>(4, 4, {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16
    });

    auto mre = Matrix<int>(4, 4, {
         1,  2,  3,  4,
        13, 14, 15, 16,
         9, 10, 11, 12,
         5,  6,  7,  8
    });

    swap_rows( m, 1, 3 );
    EXPECT_EQ( mre, m );

    auto mce = Matrix<int>(4, 4, {
         2,  1,  3,  4,
        14, 13, 15, 16,
        10,  9, 11, 12,
         6,  5,  7,  8
    });

    swap_cols( m, 1, 0 );
    EXPECT_EQ( mce, m );
}

TEST(Matrix, Ostream)
{
    auto m = Matrix<int>(2, 2, {1, 2, 3, 4});
    std::ostringstream s;
    s << m;
    EXPECT_EQ("1 2\n3 4\n", s.str());
}

TEST( Matrix, TriangularIndices )
{
    size_t const n = 1000;
    size_t k = 0;

    for( size_t i = 0; i < n; ++i ) {
        for( size_t j = i+1; j < n; ++j ) {
            auto p = triangular_indices( k, n );
            auto k2 = triangular_index( i, j, n );

            ASSERT_EQ( i, p.first );
            ASSERT_EQ( j, p.second );
            ASSERT_EQ( k, k2 );

            ++k;
        }
    }

    ASSERT_EQ( triangular_size( n ), k );

    // I totally mistrusted those weird double conversions done in the triangular index functions.
    // They are dealing with integer numbers, and are meant for indices. Using doubles just feels
    // so wrong (but necessary, because of the math involved).
    //
    // Anyway, to gain some trust in them, I ran experiements. I can now confirm that they work
    // for ALL matrix sizes < 65,000 (i.e., 65k rows times 65k columns), and for matrices of
    // the sizes 1,000,000, 2,000,000 and 4,000,000. This seems to be enough.
    //
    // Below are the programs used for those tests.
}

// =================================================================================================
//     Test Triangular Indices For All Matrix Sizes.
// =================================================================================================

/*
#include "genesis/genesis.hpp"

#include <fstream>
#include <string>
#include <unordered_map>

using namespace genesis;
using namespace genesis::utils;

int main( int argc, char** argv )
{
    // Activate logging.
    utils::Logging::log_to_stdout();
    utils::Logging::log_to_file( "triangular.log" );
    utils::Logging::details.time = true;

    size_t const num_threads = 4;

    utils::Options::get().command_line( argc, argv );
    utils::Options::get().number_of_threads( num_threads );
    LOG_BOLD << utils::Options::get().info();
    LOG_BOLD;

    LOG_INFO << "Started";

    size_t m = 0;
    size_t err = 0;

    while( true ) {
        if( m % 1000 == 0 ) {
            LOG_INFO << "At m " << m;
        }

        #pragma omp parallel for
        for( size_t t = 0; t < num_threads; ++t ) {
            size_t const n = m + t;
            size_t k = 0;

            for( size_t i = 0; i < n; ++i ) {
                for( size_t j = i+1; j < n; ++j ) {
                    auto p = triangular_indices( k, n );

                    if( i != p.first ) {
                        #pragma omp critical(log_error)
                        {
                            LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": i!=p.first=" << p.first;
                            ++err;
                        }
                    }
                    if( j != p.second ) {
                        #pragma omp critical(log_error)
                        {
                            LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": j!=p.second=" << p.second;
                            ++err;
                        }
                    }
                    if( k != triangular_index( i, j, n ) ) {
                        #pragma omp critical(log_error)
                        {
                            LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": k="
                                     << k << "!=triangular_index( i, j, n )=" << triangular_index( i, j, n );
                            ++err;
                        }
                    }

                    ++k;
                }
            }

            if( triangular_size( n ) !=  k ){
                #pragma omp critical(log_error)
                {
                    LOG_WARN << "Error at n=" << n << ": k=" << k << "!=triangular_size( n )=" << triangular_size( n );
                    ++err;
                }
            }
        }

        if( err > 100 ) {
            break;
        }

        m += 4;
    }

    LOG_INFO << "Finished";
    return 0;
}
*/

// =================================================================================================
//     Test Triangular Indices For Some Really Big Matrix Sizes.
// =================================================================================================

/*
#include "genesis/genesis.hpp"

#include <fstream>
#include <string>
#include <unordered_map>

using namespace genesis;
using namespace genesis::utils;

int main( int argc, char** argv )
{
    // Activate logging.
    utils::Logging::log_to_stdout();
    utils::Logging::log_to_file( "triangular2.log" );
    utils::Logging::details.time = true;


    utils::Options::get().command_line( argc, argv );
    LOG_BOLD << utils::Options::get().info();
    LOG_BOLD;

    LOG_INFO << "Started";

    size_t n = 1000000;
    size_t err = 0;

    while( true ) {
        LOG_INFO << "At n " << n;

        size_t k = 0;

        for( size_t i = 0; i < n; ++i ) {
            for( size_t j = i+1; j < n; ++j ) {
                auto p = triangular_indices( k, n );

                if( i != p.first ) {
                    LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": i!=p.first=" << p.first;
                    ++err;
                }
                if( j != p.second ) {
                    LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": j!=p.second=" << p.second;
                    ++err;
                }
                if( k != triangular_index( i, j, n ) ) {
                    LOG_WARN << "Error at n=" << n << ", i=" << i << ", j=" << j << ": k="
                             << k << "!=triangular_index( i, j, n )=" << triangular_index( i, j, n );
                    ++err;
                }

                ++k;
            }
        }

        if( triangular_size( n ) !=  k ){
            LOG_WARN << "Error at n=" << n << ": k=" << k << "!=triangular_size( n )=" << triangular_size( n );
            ++err;
        }

        if( err > 100 ) {
            break;
        }

        n *= 2;
    }

    LOG_INFO << "Finished";
    return 0;
}
*/
