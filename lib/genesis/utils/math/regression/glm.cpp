/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

void glm_irls(
    int family, int link,
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmExtras const&           extras,
    GlmControl const&          control,
    GlmOutput&                 result
) {
    // Some shortcuts.
    auto const N = y_response.size();
    auto const M = x_predictors.cols();

    // Already checked in main function. Assert here again for better overview.
    assert( x_predictors.rows() == N );
    assert( extras.priors.empty() || extras.priors.size() == N );
    assert( extras.strata.empty() || extras.strata.size() == N );

    // Working data.
    auto yw = std::vector<double>( N );

    // Default scale factor
    result.scale = 1.0;

    result.num_iterations = 0;
    result.converged = false;
    double logL_prev = 0.0;
    while( result.num_iterations < control.max_iterations && ! result.converged ) {
        for( size_t i = 0; i < N; i++ ) {
            yw[i] = result.resid[i] + linkfun(family, result.fitted[i]);
        }
        auto freedom = weighted_centering(
            yw, result.weights, extras.strata, extras.with_intercept, true, result.resid
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
            weighted_centering(
                x_predictors.col(i), result.weights, extras.strata,
                extras.with_intercept, true, xb_tmp
            );
            result.Xb.col( xb_col ) = xb_tmp;

            // Corrected SSQ
            double ssx = weighted_sum_of_squares(
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
        for (size_t i = 0; i < N; i++) {
            double D, Vmu, ri, wi;
            double mu = invlink(link, yw[i] - result.resid[i]);
            result.fitted[i] = validmu(family, mu);
            double pi = extras.priors.empty() ? 1.0 : extras.priors[i];
            logL += pi * loglik(family, y_response[i], mu);
            if (!(pi && (result.weights[i] > 0.0)))
                wi = ri = 0.0;
            else {
                Vmu = varfun(family, mu);
                ++freedom.valid_entries;

                if (link == family) {
                    ri = (y_response[i] - mu) / Vmu;
                    wi = pi * Vmu;
                } else {
                    D = dlink(link, mu);
                    ri = D * (y_response[i] - mu);
                    wi = pi / (D * D * Vmu);
                }
                wss += wi * ri * ri;
            }
            result.weights[i] = wi;
            result.resid[i] = ri;
        }

        int const dfr = freedom.degrees_of_freedom( result.rank );
        result.df_resid = dfr > 0 ? dfr : 0;
        if (family > 2) {
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
        result.fitted[i] = invlink( link, yw[i] - result.resid[i] );
    }
}

// =================================================================================================
//     Simple Linear Gaussian Case
// =================================================================================================

void glm_linear_gaussian(
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
        weighted_centering(
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
    int const dfr = freedom.degrees_of_freedom( result.rank );
    result.scale = wss / dfr;
    result.df_resid = dfr > 0 ? dfr : 0;

    result.converged = true;
    result.num_iterations = 0;
}

// =================================================================================================
//     Generalized Linear Model
// =================================================================================================

GlmOutput glm_fit(
    int family, int link,
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
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
    if( ! extras.priors.empty() && extras.priors.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of prior is not size of y." );
    }
    assert( extras.priors.empty() || extras.priors.size() == N );
    if( ! extras.strata.empty() && extras.strata.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of strata is not size of y." );
    }
    assert( extras.strata.empty() || extras.strata.size() == N );
    if( control.epsilon <= 0.0 || control.epsilon > 1.0 ) {
        throw std::invalid_argument( "glm_fit: epsilon has to be in ( 0.0, 1.0 ]" );
    }

    // TODO
    // with intercept and empty strata

    // Prepare results.
    GlmOutput result;
    result.Xb = Matrix<double>( N, M );
    result.fitted = std::vector<double>( N );
    result.resid = std::vector<double>( N );
    result.weights = std::vector<double>( N );
    result.which = std::vector<double>( M );
    result.betaQ = std::vector<double>( M );
    result.tri = std::vector<double>( (M * ( M+1 )) / 2 );

    // Is iteration necessary?
    bool const irls = (M > 0) && !((family == GAUSSIAN) && (link == IDENTITY));

    // Initialize the fittings.
    GlmFreedom freedom;
    if( extras.initial_fittings.empty() || ! irls ) {
        // Fit intercept and/or strata part of model
        freedom = weighted_centering(
            y_response, extras.priors, extras.strata, extras.with_intercept, false, result.fitted
        );
    } else {
        assert( irls );
        assert( extras.initial_fittings.size() == N );
        result.fitted = extras.initial_fittings;
    }

    // Prepare residuals and weights.
    freedom.valid_entries = 0;
    for( size_t i = 0; i < N; i++ ) {
        double const mu = result.fitted[i];
        double const pi = extras.priors.empty() ? 1.0 : extras.priors[i];

        if(( ! std::isfinite(pi) ) || ( pi < 0.0 )) {
            throw std::invalid_argument( "glm_fit: priors have to be non-negative." );
        } else if( pi == 0.0 ) {
            result.resid[i]   = 0.0;
            result.weights[i] = 0.0;
        } else {
            ++freedom.valid_entries;

            double const Vmu = varfun(family, mu);
            if (link == family) {
                result.resid[i]   = (y_response[i] - mu) / Vmu;
                result.weights[i] = pi * Vmu;
            } else {
                double const D = dlink(link, mu);
                result.resid[i]   = D * (y_response[i] - mu);
                result.weights[i] = pi / (D * D * Vmu);
            }
        }
    }

    // If X has data, include covariates
    if( M > 0 ) {

        // IRLS algorithm, or simple linear Gaussian case
        if (irls) {
            glm_irls( family, link, x_predictors, y_response, extras, control, result );
        } else {
            glm_linear_gaussian( x_predictors, y_response, extras, control, freedom, result );
        }

    } else {

        // No covariates
        auto const dfr = freedom.degrees_of_freedom( 0 );
        if( family > 2 ) {
            result.scale = weighted_sum_of_squares( result.resid, result.weights ) / (dfr);
        } else {
            result.scale = 1.0;
        }
        result.df_resid = dfr > 0 ? dfr : 0;
    }

    return result;
}

/*

Variance function

family:
    1        Binomial
    2        Poisson
    3        Gaussian
    4        Gamma

*/

double varfun(int family, double mu) {
    switch (family) {
    case 1:
        return ((mu * (1.0 - mu))); /* Binomial */
    case 2:
        return (mu); /* Poisson */
    case 3:
        return (1.0); /* Gaussian */
    case 4:
        return (mu * mu); /* Gamma */
    default:
        return (0.0);
    }
}

/* Valid values for fitted value, mu.

Suitable values for extreme predictions

*/

double validmu(int family, double mu) {
    const double zero = 1.e-10, one = 1.0 - 1.e-10;
    switch (family) {
    case 1: /* Binomial */
        if (mu < zero)
            return (zero);
        else if (mu > one)
            return (one);
        else
            return (mu);
    case 2: /* Poisson */
        if (mu < zero)
            return (zero);
        else
            return (mu);
    default:
        return (mu);
    }
}

/* Log likelihood contribution
     (to be multiplied by prior weight */

double loglik(int family, double y, double mu) {
    double x = 0.0;
    switch (family) {
    case 1: /* Binomial */
        return (y * log(mu) + (1. - y) * log(1. - mu));
    case 2: /* Poisson */
        return (y * log(mu) - mu);
    case 3: /* Gaussian */
        x = y - mu;
        return (x * x);
    case 4: /* Gamma */
        x = y / mu;
        return (log(x) - x);
    default:
        return std::numeric_limits<double>::quiet_NaN();
    }
}
/* Link function

Link
    1        Logit
    2        Log
    3        Identity
    4        Inverse

Note that a canonical link shares the code of the corresponding family
so that the test for canonical link is (link==family)

*/

double linkfun(int link, double mu) {
    switch (link) {
    case 1:
        return (log(mu / (1.0 - mu))); /* Logit */
    case 2:
        return (log(mu)); /* Log */
    case 3:
        return (mu); /* Identity */
    case 4:
        return (1.0 / mu); /* Inverse */
    default:
        return 0.0;
    }
}

double invlink(int link, double eta) {
    switch (link) {
    case 1:
        return (exp(eta) / (1.0 + exp(eta))); /* Logit */
    case 2:
        return (exp(eta)); /* Log */
    case 3:
        return (eta); /* Identity */
    case 4:
        return (1.0 / eta); /* Inverse */
    default:
        return (0.0);
    }
}

double dlink(int link, double mu) {
    switch (link) {
    case 1:
        return (1.0 / (mu * (1.0 - mu))); /* Logit */
    case 2:
        return (1.0 / mu); /* Log */
    case 3:
        return (1.0); /* Identity */
    case 4:
        return (1.0 / (mu * mu)); /* Inverse */
    default:
        return 0.0;
    }
}

} // namespace utils
} // namespace genesis
