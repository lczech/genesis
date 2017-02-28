/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/matrix/pca.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/matrix/operators.hpp"
#include "genesis/utils/math/matrix/statistics.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Reduce to Tridiagonal Matrix
// ================================================================================================

TridiagonalDecompositionData reduce_to_tridiagonal_matrix( Matrix<double>& data )
{
    if( data.rows() != data.cols() ) {
        throw std::runtime_error(
            "Expecting symmetrical matrix for reduce_to_tridiagonal_matrix()"
        );
    }

    // Init result.
    TridiagonalDecompositionData tri;
    tri.eigenvalues   = std::vector<double>( data.cols() );
    tri.intermediates = std::vector<double>( data.cols() );

    for( size_t i = data.cols()-1; i >= 1; --i ) {
        size_t l     = i - 1;
        double h     = 0.0;
        double scale = 0.0;

        if( l > 0 ) {
            for( size_t k = 0; k <= l; ++k ) {
                scale += fabs(data( i, k ));
            }

            if( scale == 0.0 ) {
                tri.intermediates[i] = data( i, l );
            } else {
                for( size_t k = 0; k <= l; ++k ) {
                    data( i, k ) /= scale;
                    h += data( i, k ) * data( i, k );
                }

                double f = data( i, l );
                double g = ( f > 0 ? -sqrt(h) : sqrt(h) );
                tri.intermediates[i] = scale * g;
                h -= f * g;
                data( i, l ) = f - g;
                f = 0.0;

                for( size_t j = 0; j <= l; ++j ) {
                    data( j, i ) = data( i, j )/h;
                    g = 0.0;

                    for( size_t k = 0; k <= j; ++k ) {
                        g += data( j, k ) * data( i, k );
                    }
                    for( size_t k = j+1; k <= l; ++k ) {
                        g += data( k, j ) * data( i, k );
                    }

                    tri.intermediates[j] = g / h;
                    f += tri.intermediates[j] * data( i, j );
                }

                double hh = f / (h + h);
                for( size_t j = 0; j <= l; ++j ) {
                    f = data( i, j );
                    tri.intermediates[j] = g = tri.intermediates[j] - hh * f;
                    for( size_t k = 0; k <= j; ++k ) {
                        data( j, k ) -= (f * tri.intermediates[k] + g * data( i, k ));
                    }
                }
            }
        } else {
            tri.intermediates[i] = data( i, l );
        }

        tri.eigenvalues[i] = h;
    }

    tri.eigenvalues[0]   = 0.0;
    tri.intermediates[0] = 0.0;

    for( size_t i = 0; i < data.cols(); ++i ) {
        int l = i - 1;
        if( tri.eigenvalues[i] ) {
            for( int j = 0; j <= l; ++j ) {
                double g = 0.0;
                for( int k = 0; k <= l; ++k ) {
                    g += data( i, k ) * data( k, j );
                }
                for( int k = 0; k <= l; ++k ) {
                    data( k, j ) -= g * data( k, i );
                }
            }
        }

        tri.eigenvalues[i] = data( i, i );
        data( i, i ) = 1.0;
        for( int j = 0; j <= l; ++j ) {
            data( i, j ) = 0.0;
            data( j, i ) = 0.0;
        }
    }

    return tri;
}

// ================================================================================================
//     Tridiagonal QL Algorithm
// ================================================================================================

void tridiagonal_ql_algorithm(
    Matrix<double>&               data,
    TridiagonalDecompositionData& tri,
    size_t                        max_iterations
) {
    if( data.rows() != data.cols() ) {
        throw std::runtime_error(
            "Expecting symmetrical matrix for tridiagonal_ql_algorithm()"
        );
    }
    if( tri.eigenvalues.size() != data.cols() || tri.intermediates.size() != data.cols() ) {
        throw std::runtime_error(
            "Expecting TridiagonalDecompositionData vectors of the same size"
            " as the data matrix in tridiagonal_ql_algorithm()"
        );
    }

    // Some shorthands.
    auto& d = tri.eigenvalues;
    auto& e = tri.intermediates;
    int n  = static_cast<int>( data.rows() );

    for( int i = 1; i < n; ++i ) {
        e[i-1] = e[i];
    }
    e[n-1] = 0.0;

    for( int l = 0; l < n; ++l ) {
        size_t iter = 0;
        int m = 0;

        do {
            for( m = l; m < n-1; ++m ) {
                double dd = fabs(d[m]) + fabs(d[m+1]);
                if (fabs(e[m]) + dd == dd) {
                    break;
                }
            }

            if( m != l ) {
                if( max_iterations > 0 && iter++ == max_iterations ) {
                    throw std::runtime_error( "No convergence in tridiagonal_ql_algorithm()." );
                }

                double s = 1.0;
                double c = 1.0;
                double p = 0.0;
                double g = (d[l+1] - d[l]) / (2.0 * e[l]);
                double r = sqrt((g * g) + 1.0);
                auto sign = ( (g) < 0 ? -fabs(r) : fabs(r) );
                g = d[m] - d[l] + e[l] / (g + sign);

                for( int i = m-1; i >= l; --i ) {
                    double f = s * e[i];
                    double b = c * e[i];

                    if (fabs(f) >= fabs(g)) {
                        c = g / f;
                        r = sqrt((c * c) + 1.0);
                        e[i+1] = f * r;
                        c *= (s = 1.0/r);
                    } else {
                        s = f / g;
                        r = sqrt((s * s) + 1.0);
                        e[i+1] = g * r;
                        s *= (c = 1.0/r);
                    }

                    g = d[i+1] - p;
                    r = (d[i] - g) * s + 2.0 * c * b;
                    p = s * r;
                    d[i+1] = g + p;
                    g = c * r - b;
                    for( int k = 0; k < n; ++k ) {
                        f = data( k, i+1 );
                        data( k, i+1 ) = s * data( k, i ) + c * f;
                        data( k, i ) = c * data( k, i ) - s * f;
                    }
                }

                d[l] = d[l] - p;
                e[l] = g;
                e[m] = 0.0;
            }
        } while (m != l);
    }
}

// ================================================================================================
//     Principal Component Analysis
// ================================================================================================

PcaData principal_component_analysis(
    Matrix<double> const& data,
    size_t                components,
    PcaStandardization    standardization
) {
    // Prepare result struct.
    PcaData result;

    // Normalize data and get correlation/covariance matrix.
    // We manually run the normalization step here, because we need the normalized data later.
    // The resulting `symmat` is then then same as if after performing
    // correlation_matrix / covariance_matrix / sums_of_squares_and_cross_products_matrix
    // (depending on the settings for the standardization).
    // The `symmat` is later overwritten by the tridiagonal decomposition algorithm and then
    // contains the eigenvectors.
    auto standardized_data = data;
    if( standardization == PcaStandardization::kCorrelation ) {
        standardize_cols( standardized_data, true, true );
    } else if( standardization == PcaStandardization::kCovariance ) {
        standardize_cols( standardized_data, true, false );
    }
    auto symmat = sums_of_squares_and_cross_products_matrix( standardized_data );
    for( auto& elem : symmat ) {
        elem /= static_cast<double>( standardized_data.rows() );
    }

    // As symmaat is later overwritten, we store the correlation/covariance for later.
    // auto covar = symmat;

    // Get number of desired PCA components.
    if( components == 0 ) {
        components = standardized_data.cols();
    }
    if( components > standardized_data.cols() ) {
        throw std::runtime_error(
            "Cannot calculate more PCA components than the original data has columns."
        );
    }

    // Eigenvalue Decompostion.
    auto tri = reduce_to_tridiagonal_matrix( symmat );
    tridiagonal_ql_algorithm( symmat, tri );

    // Some checks.
    assert( tri.eigenvalues.size()   == standardized_data.cols() );
    assert( tri.intermediates.size() == standardized_data.cols() );
    assert( symmat.rows()            == standardized_data.cols() );
    assert( symmat.cols()            == standardized_data.cols() );

    // Sort Eigenvalues and Eigenvectors and store in result struct.
    auto sorted_indices = sort_indices(
        tri.eigenvalues.begin(),
        tri.eigenvalues.end(),
        std::greater<double>()
    );
    result.eigenvalues  = std::vector<double>( components );
    result.eigenvectors = Matrix<double>( symmat.rows(), components );
    for( size_t c = 0; c < components; ++c ) {
        result.eigenvalues[c] = tri.eigenvalues[ sorted_indices[c] ];

        for( size_t r = 0; r < symmat.rows(); ++r ) {
            result.eigenvectors( r, c ) = symmat( r, sorted_indices[c] );
        }
    }

    // Store projections of row-points on pricipal components into result.
    // This is a simple matrix multiplication of the normalized data with the eigenvectors.
    result.projection = matrix_multiplication( standardized_data, result.eigenvectors );

    // Experimental: Calculate column projection.
    // See original implememtation for code:
    // https://github.com/davidhalter-archive/ardour/blob/master/libs/qm-dsp/maths/pca/pca.c
    //
    // auto col_proj = Matrix<double>( data.cols(), data.cols() );
    // /* Form projections of col.-points on first three prin. components. */
    // /* Store in 'covar', overwriting what was stored in this. */
    // for( size_t j = 0; j < data.cols(); ++j ) {
    //     for( size_t i = 0; i < 3; ++i ) {
    //         col_proj(j,i) = 0.0;
    //         for( size_t k2 = 0; k2 < data.cols(); ++k2 ) {
    //             col_proj(j,i) += covar(j,k2) * result.eigenvectors( k2, data.cols()-i-1 );
    //         }
    //         if( result.eigenvalues[data.cols()-i-1] > 0.0005 ) {
    //             /* Guard against zero eigenvalue */
    //             col_proj(j,i) /= sqrt(result.eigenvalues[data.cols()-i-1]);   /* Rescale */
    //         } else {
    //             col_proj(j,i) = 0.0;    /* Standard kludge */
    //         }
    //     }
    // }
    //
    // printf("\nProjections of column-points on first 3 prin. comps.:\n");
    // for (size_t j = 0; j < data.cols(); j++) {
    //     for ( size_t k = 0; k < 3; k++)  {
    //         printf("%12.4f", col_proj(j,k));
    //     }
    //     printf("\n");
    // }

    return result;
}

} // namespace utils
} // namespace genesis
