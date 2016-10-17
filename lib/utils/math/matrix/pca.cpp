/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/math/matrix/pca.hpp"

#include "utils/math/matrix/statistics.hpp"

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

void tridiagonal_ql_algorithm( Matrix<double>& data, TridiagonalDecompositionData& tri )
{
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
                if( iter++ == 30 ) {
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

void principal_component_analysis( Matrix<double> const& data )
{
    auto symmat = correlation_matrix( data );
    auto tri    = reduce_to_tridiagonal_matrix( symmat );
    tridiagonal_ql_algorithm( symmat, tri );

    auto m = symmat.cols();

    printf("\nEigenvalues:\n");
	for (int j = m-1; j >= 0; j--) {
		printf("%18.5f\n", tri.eigenvalues[j]); }
	printf("\n(Eigenvalues should be strictly positive; limited\n");
	printf("precision machine arithmetic may affect this.\n");
	printf("Eigenvalues are often expressed as cumulative\n");
	printf("percentages, representing the 'percentage variance\n");
	printf("explained' by the associated axis or principal component.)\n");

	printf("\nEigenvectors:\n");
	printf("(First three; their definition in terms of original vbes.)\n");
	for (size_t j = 0; j < m; j++) {
		for (size_t i = 1; i <= 3; i++)  {
			printf("%12.4f", symmat(j, m-i));  }
		printf("\n");  }
}

} // namespace utils
} // namespace genesis
