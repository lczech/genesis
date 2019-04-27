/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

} // namespace utils
} // namespace genesis
