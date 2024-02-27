/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.        If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

/*
    =================================================================
            License
    =================================================================

    The implementation is based on the snp.matrix and X.snp.matrix classes by
    David Clayton <david.clayton@cimr.cam.ac.uk> and Hin-Tak Leung <htl10@users.sourceforge.net>.
    The source is in C, but is originally intended for usage in R.
    This file in particular is based on snpStats_1.32.0/src/glm_test.c
    We massively refactored the original code, for example by using vectors and matrices instead
    of pointers, and by using proper data structures instead of lists of in/out function parameters.
    Furthermore, we added some new code for calculating additional statistical values such as the deviance.

    The original code is published under the GNU General Public Licence version 3 (GPLv3).
    We use the same license, hence see above for details.

    The snp.matrix and X.snp.matrix classes.
    Copyright (C) 2008 David Clayton and Hin-Tak Leung

    The package does not seem to be maintained any more, and does not seem to
    have a proper repository. For more information, try these sites:
    https://bioconductor.org/packages/release/bioc/html/snpStats.html
    https://www.rdocumentation.org/packages/snpStats/
    http://www-gene.cimr.cam.ac.uk/clayton/software/
*/

#include "genesis/utils/math/regression/glm.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/regression/family.hpp"
#include "genesis/utils/math/regression/helper.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Iteratively Reweighted Least Squares
// =================================================================================================

static void glm_irls_(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmFamily const&           family,
    GlmLink const&             link,
    GlmExtras const&           extras,
    GlmControl const&          control,
    GlmOutput&                 result
) {
    // Some shortcuts.
    auto const N = y_response.size();
    auto const M = x_predictors.cols();

    // Already checked in main function. Assert here again for better overview.
    assert( x_predictors.rows() == N );
    assert( extras.prior_weights.empty() || extras.prior_weights.size() == N );
    assert( extras.strata.empty() || extras.strata.size() == N );

    // Working data.
    auto y_working = std::vector<double>( N );

    // Default scale factor
    result.scale = 1.0;

    result.num_iterations = 0;
    result.converged = false;
    double logL_prev = 0.0;
    while( result.num_iterations < control.max_iterations && ! result.converged ) {
        for( size_t i = 0; i < N; i++ ) {
            // TODO this used the family id instead of the link id. is that correct?
            y_working[i] = result.resid[i] + link.link( result.fitted[i] );
        }
        auto freedom = weighted_mean_centering(
            y_working, result.weights, extras.strata, extras.with_intercept, true, result.resid
        );

        // Columns in Xb matrix
        result.rank = 0;

        // Orthogonal basis matrix
        size_t xb_col = 0;
        auto xb_tmp = std::vector<double>();

        // Loop over columns of X matrix
        size_t ii = 0;
        size_t ij = 0;
        for( size_t i = 0; i < M; ++i ) {
            // Center
            weighted_mean_centering(
                x_predictors.col(i), result.weights, extras.strata,
                extras.with_intercept, true, xb_tmp
            );
            result.Xb.col( xb_col ) = xb_tmp;

            // Corrected SSQ
            double const ssx = weighted_sum_of_squares(
                result.Xb.col( xb_col ), result.weights
            );
            double ssr = ssx;

            // Regress on earlier columns
            if( result.rank > 0 ) {
                for( size_t j = 0; j < result.rank; ++j ) {

                    // Coefficient
                    double bij = weighted_residuals(
                        result.Xb.col( j ),
                        result.Xb.col( xb_col ),
                        result.weights,
                        xb_tmp
                    );
                    result.Xb.col( xb_col ) = xb_tmp;

                    // Save in off-diagonal elements of tri
                    result.tri[ij] = bij;
                    ++ij;
                }
                ssr = weighted_sum_of_squares( result.Xb.col( xb_col ), result.weights );
            }

            // Check if greater than singularity threshold
            if ((ssx > 0.0) && (ssr / ssx > 1.0 - control.max_r2)) {
                double const bQi = weighted_residuals(
                    result.Xb.col( xb_col ),
                    result.resid,
                    result.weights,
                    result.resid
                );

                ++result.rank;
                ++xb_col;
                assert(( xb_col < M ) xor ( i+1 == M ));

                // Diagonal elements of tri
                result.tri[ij] = ssr;
                result.which[ii] = i;
                result.betaQ[ii] = bQi;
                ++ii;
                ++ij;
            } else {

                // If singularity, drop off-diagonal elements of tri
                ij -= result.rank;
            }
        }

        double wss = 0.0;
        freedom.valid_entries = 0;
        double logL = 0.0;
        for( size_t i = 0; i < N; ++i ) {
            double ri;
            double wi;
            double const mu = link.inverse_link( y_working[i] - result.resid[i] );
            double const pi = extras.prior_weights.empty() ? 1.0 : extras.prior_weights[i];

            result.fitted[i] = family.rectify( mu );
            logL += pi * family.log_likelihood( y_response[i], mu );

            if( !( pi != 0.0 && ( result.weights[i] > 0.0 ))) {
                wi = 0.0;
                ri = 0.0;
            } else {
                double const Vmu = family.variance( mu );
                ++freedom.valid_entries;

                if( link.id == family.canonical_link_id ) {
                    wi = pi * Vmu;

                    switch( extras.residual_type ) {
                        case GlmExtras::kDefault: {
                            ri = ( y_response[i] - mu ) / Vmu;
                            break;
                        }
                        case GlmExtras::kPearsonResiduals: {
                            ri = ( y_response[i] - mu ) / std::sqrt( Vmu );
                            break;
                        }
                        case GlmExtras::kDevianceResiduals: {
                            auto const ud = family.unit_deviance( y_response[i], mu );
                            ri = signum( y_response[i] - mu ) * std::sqrt( ud );
                            break;
                        }
                        default: {
                            throw std::invalid_argument( "Invalid residual type specified." );
                        }
                    }
                } else {
                    double const D = link.derivative( mu );
                    wi = pi / (D * D * Vmu);
                    ri = D * (y_response[i] - mu);
                }
                wss += wi * ri * ri;
            }
            result.weights[i] = wi;
            result.resid[i] = ri;
        }

        long const dfr = freedom.degrees_of_freedom( result.rank );
        result.df_resid = dfr > 0 ? static_cast<size_t>( dfr ) : 0;
        if( family.id == GlmFamily::kGaussian || family.id == GlmFamily::kGamma ) {
            result.scale = wss / (dfr);
        }

        // Check for convergence and iterate if necessary.
        if( result.num_iterations > 1 ) {
            double dL = (logL - logL_prev) / (result.scale);
            if( dL < control.epsilon ) {
                result.converged = true;
            }
        }
        logL_prev = logL;
        ++result.num_iterations;
    }

    for( size_t i = 0; i < N; i++ ) {
        result.fitted[i] = link.inverse_link( y_working[i] - result.resid[i] );
    }
}

// =================================================================================================
//     Simple Linear Gaussian Case
// =================================================================================================

static void glm_gaussian_(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmExtras const&           extras,
    GlmControl const&          control,
    GlmFreedom const&          freedom,
    GlmOutput&                 result
) {
    // Some shortcuts.
    auto const N = y_response.size();
    auto const M = x_predictors.cols();

    // Already checked in main function. Assert here again for better overview.
    assert( x_predictors.rows() == N );
    assert( extras.strata.empty() || extras.strata.size() == N );

    size_t xb_col = 0;
    auto xb_tmp = std::vector<double>();
    result.rank = 0;

    size_t ii = 0;
    size_t ij = 0;
    for( size_t i = 0; i < M; i++ ) {
        weighted_mean_centering(
            x_predictors.col(i), result.weights, extras.strata,
            extras.with_intercept, true, xb_tmp
        );
        result.Xb.col( xb_col ) = xb_tmp;

        double ssx = weighted_sum_of_squares( result.Xb.col( xb_col ), result.weights );
        double ssr = ssx;

        // Regress on earlier columns
        if( result.rank > 0 ) {
            for( size_t j = 0; j < result.rank; ++j ) {
                double const bij = weighted_residuals(
                    result.Xb.col( j ),
                    result.Xb.col( xb_col ),
                    result.weights,
                    xb_tmp
                );
                result.Xb.col( xb_col ) = xb_tmp;

                // Off-diagonal
                result.tri[ij] = bij;
                ++ij;
            }
            ssr = weighted_sum_of_squares( result.Xb.col( xb_col ), result.weights );
        }

        // Check if we are above the singularity threshold
        if(( ssx > 0.0) && (ssr / ssx > 1.0 - control.max_r2 )) {
            double const bQi = weighted_residuals(
                result.Xb.col( xb_col ),
                result.resid,
                result.weights,
                result.resid
            );

            ++result.rank;
            ++xb_col;

            // Diagonal
            result.tri[ij] = ssr;
            result.which[ii] = i;
            result.betaQ[ii] = bQi;
            ++ij;
            ++ii;
        } else {
            // We have a singluarity. Drop the element.
            ij -= result.rank;
        }
    }

    for( size_t i = 0; i < N; ++i ) {
        result.fitted[i] = y_response[i] - result.resid[i];
    }

    double const wss = weighted_sum_of_squares( result.resid, result.weights );
    long const dfr = freedom.degrees_of_freedom( result.rank );
    result.scale = wss / dfr;
    result.df_resid = dfr > 0 ? static_cast<size_t>( dfr ) : 0;

    result.converged = true;
    result.num_iterations = 0;
}

// =================================================================================================
//     Generalized Linear Model
// =================================================================================================

GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmFamily const&           family,
    GlmLink const&             link,
    GlmExtras const&           extras,
    GlmControl const&          control
) {

    // Some shortcuts.
    auto const N = y_response.size();
    auto const M = x_predictors.cols();

    // Error checks.
    if( x_predictors.rows() != N ) {
        throw std::invalid_argument( "glm_fit: size of rows of x is not size of y." );
    }
    if( ! extras.initial_fittings.empty() && extras.initial_fittings.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of initial fittings is not size of y." );
    }
    assert( extras.initial_fittings.empty() || extras.initial_fittings.size() == N );
    if( ! extras.prior_weights.empty() && extras.prior_weights.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of prior weights is not size of y." );
    }
    assert( extras.prior_weights.empty() || extras.prior_weights.size() == N );
    if( ! extras.strata.empty() && extras.strata.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of strata is not size of y." );
    }
    assert( extras.strata.empty() || extras.strata.size() == N );
    if( control.epsilon <= 0.0 || control.epsilon > 1.0 ) {
        throw std::invalid_argument( "glm_fit: epsilon has to be in ( 0.0, 1.0 ]" );
    }
    if( control.max_r2 <= 0.0 || control.max_r2 >= 1.0 ) {
        throw std::invalid_argument( "glm_fit: max_r2 has to be in ( 0.0, 1.0 )" );
    }
    if( ! is_defined( family )) {
        throw std::invalid_argument( "glm_fit: family is not properly defined." );
    }
    if( ! is_defined( link )) {
        throw std::invalid_argument( "glm_fit: link is not properly defined." );
    }

    // TODO interactions

    // Prepare results.
    GlmOutput result;
    result.Xb      = Matrix<double>( N, M );
    result.fitted  = std::vector<double>( N );
    result.resid   = std::vector<double>( N );
    result.weights = std::vector<double>( N );
    result.which   = std::vector<double>( M );
    result.betaQ   = std::vector<double>( M );
    result.tri     = std::vector<double>( (M * ( M+1 )) / 2 );

    // Is iteration necessary?
    bool const irls = (M > 0) && !(
        ( family.id == GlmFamily::kGaussian ) && ( link.id == GlmLink::kIdentity )
    );

    // Initialize the fittings.
    GlmFreedom freedom;
    if( extras.initial_fittings.empty() || ! irls ) {
        // Fit intercept and/or strata part of model,
        // that is, set the fitted values to the (strata) (weighted) mean of the y values.
        freedom = weighted_mean_centering(
            y_response, extras.prior_weights, extras.strata, extras.with_intercept, false, result.fitted
        );
    } else {
        assert( irls );
        assert( extras.initial_fittings.size() == N );
        result.fitted = extras.initial_fittings;
    }

    // Prepare residuals and weights, and calculate null deviance.
    freedom.valid_entries = 0;
    assert( result.null_deviance == 0.0 );
    for( size_t i = 0; i < N; i++ ) {
        double const mu = result.fitted[i];
        double const pi = extras.prior_weights.empty() ? 1.0 : extras.prior_weights[i];

        // Null deviance
        auto const ud = family.unit_deviance( y_response[i], mu );
        if( std::isfinite( ud )) {
            result.null_deviance += ud;
        }

        // Residuals and weights.
        if(( ! std::isfinite(pi) ) || ( pi < 0.0 )) {
            throw std::invalid_argument( "glm_fit: prior weights have to be non-negative." );
        } else if( pi == 0.0 ) {
            result.resid[i]   = 0.0;
            result.weights[i] = 0.0;
        } else {
            assert( std::isfinite( pi ) || ( pi > 0.0 ));
            ++freedom.valid_entries;

            double const Vmu = family.variance( mu );
            if( link.id == family.canonical_link_id ) {
                switch( extras.residual_type ) {
                    case GlmExtras::kDefault: {
                        result.resid[i] = ( y_response[i] - mu ) / Vmu;
                        break;
                    }
                    case GlmExtras::kPearsonResiduals: {
                        result.resid[i] = ( y_response[i] - mu ) / std::sqrt( Vmu );
                        break;
                    }
                    case GlmExtras::kDevianceResiduals: {
                        result.resid[i] = signum( y_response[i] - mu ) * std::sqrt( ud );
                        break;
                    }
                    default: {
                        throw std::invalid_argument( "Invalid residual type specified." );
                    }
                }
                result.weights[i] = pi * Vmu;
            } else {
                double const D = link.derivative( mu );
                result.resid[i]   = D * (y_response[i] - mu);
                result.weights[i] = pi / (D * D * Vmu);
            }
        }
    }
    if( extras.mean_deviance ) {
        result.null_deviance /= static_cast<double>(N);
    }

    // If X has data, include covariates
    if( M > 0 ) {

        // IRLS algorithm, or simple linear Gaussian case
        if( irls ) {
            glm_irls_( x_predictors, y_response, family, link, extras, control, result );
        } else {
            glm_gaussian_( x_predictors, y_response, extras, control, freedom, result );
        }

        // Calcualte deviance.
        assert( result.deviance == 0.0 );
        for( size_t i = 0; i < N; i++ ) {
            auto const ud = family.unit_deviance( y_response[i], result.fitted[i] );
            if( std::isfinite( ud )) {
                result.deviance += ud;
            }
        }
        if( extras.mean_deviance ) {
            result.deviance /= static_cast<double>(N);
        }

    } else {

        // No covariates
        auto const dfr = freedom.degrees_of_freedom( 0 );
        if( family.id == GlmFamily::kGaussian || family.id == GlmFamily::kGamma ) {
            result.scale = weighted_sum_of_squares( result.resid, result.weights ) / (dfr);
        } else {
            result.scale = 1.0;
        }
        result.df_resid = dfr > 0 ? static_cast<size_t>( dfr ) : 0;
    }

    return result;
}

GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmFamily const&           family,
    GlmExtras const&           extras,
    GlmControl const&          control
) {
    if( ! family.canonical_link ) {
        throw std::runtime_error( "glm_fit: family does not provide a canonical link." );
    }
    return glm_fit( x_predictors, y_response, family, family.canonical_link(), extras, control );
}

GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmExtras const&           extras,
    GlmControl const&          control
) {
    auto family = glm_family_gaussian();
    return glm_fit( x_predictors, y_response, family, family.canonical_link(), extras, control );
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Invert diagonal and unit upper triangular matrices stored as one array.
 */
std::vector<double> glm_inv_tri_( std::vector<double> const& tri, size_t M )
{
    if( tri.size() != (M * ( M+1 )) / 2 ) {
        throw std::invalid_argument(
            "glm_inv_tri_(): Input tri vector is expected to have tridiagonal form."
        );
    }

    auto result = tri; // std::vector<double>( M, 0 );
    for( size_t j = 0, ij = 0; j < M; j++ ) {
        for( size_t i = 0, ii1 = 1; i < j; i++, ii1 += (i+2) ) {
            double w = tri[ij];
            for( size_t k = i+1, kj = ij+1, ik = ii1; k < j; k++, kj++, ik += k ) {
                w += result[ik]*tri[kj];
            }
            result[ij++] = (-w);
        }
        double diag = tri[ij];
        if( diag <= 0.0 ) {
            std::runtime_error(
                "glm_inv_tri_(): negative diagonal with j==" + std::to_string(j) +
                ", ij==" + std::to_string(ij) + ", diag==" + std::to_string(diag)
            );
        }
        result[ij++] = 1/diag;
    }
    return result;
}

/**
 * @brief Helper function to compute the betas, given that we have already inverted
 * the tri matrix.
 */
std::vector<double> glm_estimate_betas_inv_tri_(
    GlmOutput const& output,
    std::vector<double> const& inv_tri
) {
    assert( inv_tri.size() == output.tri.size() );
    if( output.betaQ.size() != output.Xb.cols() ) {
        throw std::invalid_argument(
            "Invalid size of betaQ for computing glm_estimate_betas()"
        );
    }

    auto const M = output.Xb.cols();
    auto betas = std::vector<double>( M );
    for( size_t i = 0, ijs = 1; i < M; i++, ijs += (2+i) ) {
        double w = output.betaQ[i];
        for( size_t j = i+1, ij = ijs; j < M; j++, ij += j ) {
            w += output.betaQ[j] * inv_tri[ij];
        }
        betas[i] = w;
    }
    return betas;
}

std::vector<double> glm_estimate_betas( GlmOutput const& output )
{
    auto const inv_tri = glm_inv_tri_( output.tri, output.Xb.cols() );
    return glm_estimate_betas_inv_tri_( output, inv_tri );
}

/**
 * @brief Calculate U.D.U-transpose.
 *
 * For packed upper unit triangular matrix, U, and diagonal matrix D
 * (occupying the same space, U), calculate U.D.U-transpose and scale
 * it by a constant multiple.
 */
std::vector<double> udu_transpose_( size_t M, std::vector<double> const& U, double scale )
{
    assert( U.size() == (M * ( M+1 )) / 2 );

    // TODO the below code is wrong and accesses an element ik beyond the input array.
    // It's a mess of indexing magic, and I understand what it is trying to do,
    // but lack the time to dive into why it is not doing that exactly.
    // Furthermore, with our test case, we get a symmetric, but not positive definite
    // matrix inv_tri, which means that this decomposition here is invalid anyway.
    // As that test is based on some very clean numbers, it probably is worse on more
    // messy data. Hence, for now, this will not be properly implemented, until
    // someone actually needs this.
    assert( false );

    auto result = std::vector<double>( U.size() );
    for( size_t j = 0, ij = 0, jj = 0; j < M; j++, jj += (1 + j) ) {
        for( size_t i = 0; i <= j; i++, ij++ ) {
            double w = 0.0;
            for(
                size_t k = j, jk = jj, ik = jj - i + j, kk = jj;
                k < M;
                k++, kk += (1 + k), jk += k, ik += k
            ) {
                double Uik = (i == k) ? 1.0 : U[ik];
                double Ujk = (j == k) ? 1.0 : U[jk];
                double Dk = U[kk];
                w += Uik * Ujk * Dk;
            }
            result[ij] = scale * w;
        }
    }
    return result;
}

/**
 * @brief Calculate U.D.V.D.U-transpose.
 *
 * For packed upper unit triangular matrix, U, and diagonal matrix D
 * (occupying the same space, U), and packed symmetric matrix V,
 * calculate U.D.V.D.U-transpose and multiply by a scale factor.
 */
std::vector<double> udvdu_transpose_(
    size_t M, std::vector<double> const& U, std::vector<double> const& V, double scale
) {
    assert( U.size() == (M * ( M+1 )) / 2 );

    auto result = std::vector<double>( U.size() );
    for( size_t j = 0, ij = 0, jj = 0; j < M; jj += (2 + (j++)) ) {
        for( size_t i = 0, ii = 0; i <= j; ii += (2 + (i++)), ij++ ) {
            double w = 0.0;
            for(
                size_t v = j, vv = jj, jv = jj, uv = ij;
                v < M;
                v++, vv += (2 + v), jv += v
            ) {
                double Ujv = (v == j) ? 1.0 : U[jv];
                double Dv = U[vv];
                for( size_t u = i, uu = ii, iu = ii; u < M; u++, uu += (2 + u), iu += u ) {
                    double Uiu = (u == i) ? 1.0 : U[iu];
                    double Du = U[uu];
                    w += Du * Dv * Uiu * Ujv * V[uv];
                    if( u < v ) {
                        uv++;
                    } else {
                        uv += (1 + u);
                    }
                }
                uv = vv + i + 1;
            }
            result[ij] = scale * w;
        }
    }
    return result;
}

std::pair<std::vector<double>, std::vector<double>> glm_estimate_betas_and_var_covar(
    GlmOutput const& output,
    std::vector<double> const& meat
) {
    throw std::runtime_error( "glm_estimate_betas_and_var_covar() not implemented" );

    auto const M = output.Xb.cols();
    auto const inv_tri = glm_inv_tri_( output.tri, M );
    auto const betas = glm_estimate_betas_inv_tri_( output, inv_tri );
    assert( betas.size() == M );

    std::vector<double> vars_covars;
    if( meat.empty() ) {
        vars_covars = udu_transpose_( M, inv_tri, output.scale );
    } else {
        vars_covars = udvdu_transpose_( M, inv_tri, meat, output.scale );
    }
    assert( vars_covars.size() == (M * ( M+1 )) / 2 );
    return { betas, vars_covars };
}

double glm_estimate_intercept(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmOutput const&           output,
    std::vector<double> const& betas
) {
    return glm_estimate_intercept(
        x_predictors, y_response, glm_link_identity(), output, betas
    );
}

double glm_estimate_intercept(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmLink const&             link,
    GlmOutput const&           output,
    std::vector<double> const& betas
) {
    if( betas.size() != x_predictors.cols() ) {
        throw std::invalid_argument(
            "Invalid size of betas for computing glm_estimate_intercept()"
        );
    }

    // TODO Do we need to account for strata here as well?
    // As far as I could figure this out, the estimated betaQ values are computed across strata
    // andyway, so then the intercept would be as well? Meaning that we do not need to take
    // strata into account here again. But not sure.

    // We compute the weighted averages of the y_response and each of the x_predictors,
    // using the weights as determined by glm_fit(). Then, we compute the sum of the products
    // of the betas with the weighted averages of the predictors. That, subtracted from the
    // response, is our intercept.

    // First we get the sum of the weights themselves.
    // We just "misuse" the weighted sum here to sum the weights themselves.
    auto const weight_sum = weighted_sum( output.weights );
    assert( std::isfinite( weight_sum ));

    // We first need to translate our response into the link space.
    auto y_response_transformed = y_response;
    for( auto& y : y_response_transformed ) {
        y = link.link(y);
    }

    // Now compute the weighted sum of the y_response_transformed, and divide by the weight sum,
    // i.e., compute the weighted average of the response.
    auto const y_avg = weighted_sum( y_response_transformed, output.weights ) / weight_sum;
    assert( std::isfinite( y_avg ));

    // Compute our final result by subtracting product of the beta values with the sum of the
    // weighted average of each column of x_predictors from the y_response_transformed average.
    double result = y_avg;
    for( size_t i = 0; i < x_predictors.cols(); ++i ) {
        auto const x_col_avg = weighted_sum( x_predictors.col(i), output.weights ) / weight_sum;
        assert( std::isfinite( x_col_avg ));
        result -= betas[i] * x_col_avg;
    }
    return result;
}

std::vector<double> glm_coefficients(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmOutput const&           output
) {
    return glm_coefficients( x_predictors, y_response, glm_link_identity(), output );
}

std::vector<double> glm_coefficients(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmLink const&             link,
    GlmOutput const&           output
) {
    auto coeffs = glm_estimate_betas( output );
    auto intercept = glm_estimate_intercept( x_predictors, y_response, link, output, coeffs );
    coeffs.insert( coeffs.begin(), intercept );
    return coeffs;
}

} // namespace utils
} // namespace genesis
