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
//     Generalized Linear Model
// =================================================================================================

/* Fit GLM, possibly with a stratification in the RHS

Input:

family       GLM family (see below)
link         Link function (see below)
N            # units
M            # X variables
P            # X variables for which parameter estimates required
S            # strata (0 means no intercept)
y            y-variable (N-vector)
prior        prior weights (if present)
X            If M>0, N*M matrix of X variables
stratum      If S>1, stratum assignments coded 1...S (N-vector)
maxit        Maximum number of iterations of IRLS algorithm
conv         Proportional change in weighted sum of squares residuals to
             declare convergence
r2max        Maximum value of R^2 between an X variable and previous variables
             before it is dropped as aliased
init         If true (non-zero), the iteration starts from initial estimates
             of fitted values (see below). This option has no effect if
             no iteration is required

Output:

rank         rank of X after regression on strata
Xb           orthogonal basis for X space (N*rank matrix)
fitted       fitted values
resid        working residuals (on linear predictor scale) (N-vector)
weights      weights (N-vector)
scale        scale factor (scalar)
df_resid     residual degrees of freedom

Output for coefficients to be estimated (can be set NULL if not required):

P_est        # coefficients which could be estimated
which        which columns in the X matrix were estimated (first = 0)
betaQ        vector of parameter estimates (in terms of basis matrix, Xb)
tri          upper unit triangular transformation matrix, with Xb-tr.Xb
             placed in the diagonal

Return

0            convergence
1            no convergence after maxit iterations

*/

// int family, int link, int N, int M, int P, int S, const double *y,
// const double *prior, const double *X, const int *stratum, int maxit,
// double epsilon, double r2max, int init,
// int *rank, double *Xb,
// double *fitted, double *resid, double *weights, double *scale,
// int *df_resid, int *P_est, int *which, double *betaQ, double *tri

GlmOutput glm_fit(
    int family, int link, int S, std::vector<double> const& y,
    std::vector<double> const& prior, Matrix<double> const& X, std::vector<int> const& stratum, size_t maxit,
    double epsilon, double r2max, int init
) {

    // Some shortcuts.
    auto const N = y.size();
    auto const M = X.cols();

    // Error checks.
    if( X.rows() != N ) {
        throw std::invalid_argument( "glm_fit: size of rows of X is not size of y." );
    }
    if( ! prior.empty() && prior.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of prior is not size of y." );
    }
    assert( prior.empty() || prior.size() == N );
    if( ! stratum.empty() && stratum.size() != N ) {
        throw std::invalid_argument( "glm_fit: size of stratum is not size of y." );
    }
    assert( stratum.empty() || stratum.size() == N );

    // Prepare results.
    GlmOutput result;
    result.Xb = Matrix<double>( N, M );
    result.fitted = std::vector<double>( N );
    result.resid = std::vector<double>( N );
    result.weights = std::vector<double>( N );
    result.which = std::vector<double>( M );
    result.betaQ = std::vector<double>( M );
    result.tri = std::vector<double>( (M * ( M+1 )) / 2 );

    double eta = 1.0 - r2max; // Singularity threshold
    int Nu, irls;
    int dfr = 0;
    int empty = 0;
    result.scale = 1.0; // Default scale factor

    // Is iteration necessary?
    irls = (M > 0) && !((family == GAUSSIAN) && (link == IDENTITY));

    if (!init || !irls) {
        // Fit intercept and/or strata part of model
        empty = weighted_centering( y, prior, stratum, S, false, result.fitted );
    }

    Nu = 0;
    int invalid = 0;
    for( size_t i = 0; i < N; i++ ) {
        double mu = result.fitted[i];
        double ri, wi;
        double pi = prior.empty() ? 1.0 : prior[i];
        if (!(pi))
            wi = ri = 0.0;
        else {
            Nu++;
            double Vmu = varfun(family, mu);
            if (link == family) {
                ri = (y[i] - mu) / Vmu;
                wi = pi * Vmu;
            } else {
                double D = dlink(link, mu);
                ri = D * (y[i] - mu);
                wi = pi / (D * D * Vmu);
            }
        }
        result.weights[i] = wi;
        result.resid[i] = ri;
    }

    /* If M>0, include covariates */

    int convg = 0;
    result.num_iterations = 0;
    if (M) {
        convg = 0;
        if (irls) {

            /* IRLS algorithm */

            auto yw = std::vector<double>( N );
            double logL_prev = 0.0;
            while (result.num_iterations < maxit && !convg) {
                double logL = 0.0;
                for( size_t i = 0; i < N; i++ ) {
                    yw[i] = result.resid[i] + linkfun(family, result.fitted[i]);
                }
                empty = weighted_centering( yw, result.weights, stratum, S, true, result.resid );

                // Columns in Xb matrix
                result.rank = 0;

                // Orthogonal basis matrix
                size_t xb_col = 0;
                auto xb_tmp = std::vector<double>();

                // Loop over columns of X matrix
                for( size_t i = 0, ii = 0, ij = 0; i < M; ++i ) {
                    // Center
                    weighted_centering( X.col(i), result.weights, stratum, S, 1, xb_tmp );
                    result.Xb.col( xb_col ) = xb_tmp;

                    // Corrected SSQ
                    double ssx = weighted_sum_of_squares(
                        result.Xb.col( xb_col ), result.weights
                    );

                    // Regress on earlier columns
                    double ssr = ssx;
                    if (result.rank) {
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

                        // Residual SSQ
                        ssr = weighted_sum_of_squares( result.Xb.col( xb_col ), result.weights );
                    }

                    if ((ssx > 0.0) && (ssr / ssx > eta)) {
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
                        // Drop off-diagonal elements of tri
                        ij -= result.rank;
                    }
                }

                double wss = 0.0;
                Nu = 0;
                for (size_t i = 0; i < N; i++) {
                    double D, Vmu, ri, wi;
                    double mu = invlink(link, yw[i] - result.resid[i]);
                    result.fitted[i] = validmu(family, mu);
                    double pi = prior.empty() ? 1.0 : prior[i];
                    logL += pi * loglik(family, y[i], mu);
                    if (!(pi && (result.weights[i] > 0.0)))
                        wi = ri = 0.0;
                    else {
                        Vmu = varfun(family, mu);
                        Nu++;
                        if (link == family) {
                            ri = (y[i] - mu) / Vmu;
                            wi = pi * Vmu;
                        } else {
                            D = dlink(link, mu);
                            ri = D * (y[i] - mu);
                            wi = pi / (D * D * Vmu);
                        }
                        wss += wi * ri * ri;
                    }
                    result.weights[i] = wi;
                    result.resid[i] = ri;
                }
                dfr = Nu - S + empty - result.rank;
                if (family > 2) {
                    result.scale = wss / (dfr);
                }
                if (result.num_iterations > 1) {
                    double dL = (logL - logL_prev) / (result.scale);
                    if (dL < 0.0)
                        convg = 2;
                    if (result.num_iterations < maxit && dL < epsilon)
                        convg = 1;
                }
                logL_prev = logL;
                result.num_iterations++;
            }
            for( size_t i = 0; i < N; i++ ) {
                result.fitted[i] = invlink(link, yw[i] - result.resid[i]);
            }

        } else {

            /* Simple linear Gaussian case */

            size_t xb_col = 0;
            auto xb_tmp = std::vector<double>();
            result.rank = 0;

            for( size_t i = 0, ii = 0, ij = 0; i < M; i++ ) {
                weighted_centering( X.col(i), result.weights, stratum, S, true, xb_tmp );
                result.Xb.col( xb_col ) = xb_tmp;

                double ssx = weighted_sum_of_squares( result.Xb.col( xb_col ), result.weights );
                double ssr = ssx;
                if (result.rank) {
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
                if(( ssx > 0.0) && (ssr / ssx > eta )) {
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
                    ij -= result.rank;
                }
            }
            double wss = weighted_sum_of_squares( result.resid, result.weights );
            for( size_t i = 0; i < N; ++i ) {
                result.fitted[i] = y[i] - result.resid[i];
            }
            dfr = Nu - S + empty - result.rank;
            result.scale = wss / (dfr);
        }
    } else {
        /* No covariates */

        if ((S > 1) && invalid) /* Need to recalculate empty stratum count    */
            empty = weighted_centering( result.fitted, result.weights, stratum, S, false, result.fitted );
        dfr = Nu - S + empty;
        if (family > 2)
            result.scale = weighted_sum_of_squares( result.resid, result.weights ) / (dfr);
        else
            result.scale = 1.0;
        result.rank = 0;
    }

    result.df_resid = dfr > 0 ? dfr : 0;
    result.converged = (irls && !convg);

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

// =================================================================================================
//     GLM Score Test
// =================================================================================================

/*

GLM score test

Input:

P         Number of new explanatory variables to be added
Z         N*P matrix containing these (destroyed)
C         If robust variance estimate to be used, number of clusters
          (if C==1, each unit forms a cluster)
cluster   If C>1, cluster assignments code 1...C (N-vector)
max_R2    For P>1, the maximum value of R^2 between each column and previous
          columns (after regression on X and strata)

For all other input arguments, see glm_fit, but note that M now coincides
with rank -- the number of columns in Xb

Output:

score       P*1 vector of scores
score-var   Upper triangle of variance-covariance matrix of scores

* /

void glm_score_test(int N, int M, int S, const int *stratum, int P,
                                        const double *Z, int C, const int *cluster,
                                        const double *resid, const double *weights,
                                        const double *Xb, double scale, double max_R2,
                                        double *score, double *score_var) {
    const double eta = 1.e-8; // First stage singularity test
    const double *Zi = Z;
    double *Zr, *Zri;
    double *U = NULL, *Ui = NULL;

    (void) max_R2;

    // Work array

    Zri = Zr = (double *)calloc(N * P, sizeof(double));
    int nc = 0;
    if (C) {
        nc = (C == 1) ? N : C;
        Ui = U = (double *)calloc(nc * P, sizeof(double));
        memset(U, 0x00, nc * P * sizeof(double));
    }

    // Main algorithm

    for (int i = 0, ij = 0; i < P; i++, Zi += N, Zri += N) {

        // Regress each column of Z on strata indicators and X basis

        double ssz = wssq(Zi, N, weights);
        wcenter(Zi, N, weights, stratum, S, 1, Zri);
        const double *Xbj = Xb;
        for (int j = 0; j < M; j++, Xbj += N)
            wresid(Zri, N, weights, Xbj, Zri);
        double ssr = wssq(Zri, N, weights);

        if (ssr / ssz > eta) { // Singularity test

            if (C) {

                // Add new column to matrix of score contributions

                if (C == 1) {
                    for (int k = 0; k < N; k++)
                        Ui[k] = Zri[k] * weights[k] * resid[k];
                } else {
                    for (int k = 0; k < N; k++) {
                        int kc = cluster[k] - 1;
                        Ui[kc] += Zri[k] * weights[k] * resid[k];
                    }
                }

                // Update score and score-variance

                score[i] = wsum(Ui, nc, NULL);
                double *Uj = U;
                for (int j = 0; j < i; j++, Uj += nc)
                    score_var[ij++] = wspr(Ui, Uj, nc, NULL);
                score_var[ij++] = wssq(Ui, nc, NULL);
            } else {

                // Update score and score-variance

                score[i] = wspr(Zri, resid, N, weights);
                double *Zrj = Zr;
                for (int j = 0; j < i; j++, Zrj += N)
                    score_var[ij++] = scale * wspr(Zri, Zrj, N, weights);
                score_var[ij++] = scale * wssq(Zri, N, weights);
            }
        } else {
            memset(Zri, 0x00, N * sizeof(double));
            score[i] = 0.0;
            for (int j = 0; j <= i; j++)
                score_var[ij++] = 0.0;
        }
        if (C)
            Ui += nc;
    }
    free(Zr);
    if (C)
        free(U);
}

// */

// =================================================================================================
//     Parameter Estimation
// =================================================================================================

/* Invert diagonal and unit upper triangular matrices stored as one array
     Result matrix can overwrite input matrix */

void inv_tri(int N, const double *tri, double *result) {
    for (int j = 0, ij = 0; j < N; j++) {
        for (int i = 0, ii1 = 1; i < j; i++, ii1 += (i + 2)) {
            double w = tri[ij];
            for (int k = i + 1, kj = ij + 1, ik = ii1; k < j; k++, kj++, ik += k) {
                w += result[ik] * tri[kj];
            }
            result[ij++] = (-w);
        }
        double diag = tri[ij];
        if (diag <= 0.0)
            throw std::runtime_error(
                "inv_tri: negative diagonal, " + std::to_string(j) + " " +
                std::to_string(ij) + " " + std::to_string(diag) + " "
            );
        result[ij++] = 1 / diag;
    }
}

/* For packed upper unit triangular matrix, U, and diagonal matrix D
     (occupying the same space, tri), calculate U.D.U-transpose and scale
     it by a connstant multiple. Result matrix can overwite input U/D matrix */

void UDUt(int N, const double *tri, double scale, double *result) {
    for (int j = 0, ij = 0, jj = 0; j < N; j++, jj += (1 + j)) {
        for (int i = 0; i <= j; i++, ij++) {
            double w = 0.0;
            for (int k = j, jk = jj, ik = jj - i + j, kk = jj; k < N;
                     k++, kk += (1 + k), jk += k, ik += k) {
                double Uik = (i == k) ? 1.0 : tri[ik];
                double Ujk = (j == k) ? 1.0 : tri[jk];
                double Dk = tri[kk];
                w += Uik * Ujk * Dk;
            }
            result[ij] = scale * w;
        }
    }
}

/* For packed upper unit triangular matrix, U, and diagonal matrix D
     (occupying the same space, tri), and packed symmetric matrix V,
     calculate U.D.V.D.U-transpose and multiply by a scale factor.
     Result matrix can overwrite input V */

void UDVDUt(int N, const double *tri, const double *V, double scale,
                        double *result) {
    for (int j = 0, ij = 0, jj = 0; j < N; jj += (2 + (j++))) {
        for (int i = 0, ii = 0; i <= j; ii += (2 + (i++)), ij++) {
            double w = 0.0;
            for (int v = j, vv = jj, jv = jj, uv = ij; v < N;
                     v++, vv += (2 + v), jv += v) {
                double Ujv = (v == j) ? 1.0 : tri[jv];
                double Dv = tri[vv];
                for (int u = i, uu = ii, iu = ii; u < N; u++, uu += (2 + u), iu += u) {
                    double Uiu = (u == i) ? 1.0 : tri[iu];
                    double Du = tri[uu];
                    w += Du * Dv * Uiu * Ujv * V[uv];
                    if (u < v)
                        uv++;
                    else
                        uv += (1 + u);
                }
                uv = vv + i + 1;
            }
            result[ij] = scale * w;
        }
    }
}

/* Obtain estimates and variance covariance matrix of estimates from output
     of glm_fit. Parameter estimates can overwrite betaQ. Robust variance is
     calculated if the "meat" matrix for the information sandwich is supplied.
     If robust variance estimate not used, var-cov matrix can overwrite tri;
     otherwise it can overwrite meatrix */

void glm_est(int P_est, const double *betaQ, double *tri, double scale,
                         const double *meatrix, double *beta, double *var_beta) {

    inv_tri(P_est, tri, tri);
    for (int i = 0, ijs = 1; i < P_est; i++, ijs += (2 + i)) {
        double w = betaQ[i];
        for (int j = i + 1, ij = ijs; j < P_est; j++, ij += j)
            w += betaQ[j] * tri[ij];
        beta[i] = w;
    }

    /* Variance covariance matrix */

    if (meatrix)
        UDVDUt(P_est, tri, meatrix, scale, var_beta);
    else
        UDUt(P_est, tri, scale, var_beta);
}

/* Calculate "meat" matrix for information sandwich */

void meat_matrix(int N, int P, int C, const int *cluster, const double *Xb,
                                 const double *resid, const double *weights, double *meatrix) {
    if (!C)
        return;
    if (C > 1) {
        double *Uc = (double*)calloc(C * P, sizeof(double));
        memset(Uc, 0x00, C * P * sizeof(double));
        for (int i = 0; i < N; i++) {
            double w = resid[i] * weights[i];
            int ic = cluster[i] - 1;
            for (int j = 0, ij = i, ijc = ic; j < P; j++, ij += N, ijc += C)
                Uc[ijc] += w * Xb[ij];
        }
        for (int i = 0, ij = 0, ik0 = 0; i < P; i++, ik0 += C) {
            for (int j = 0, jk = 0; j <= i; j++) {
                double w = 0.0;
                for (int k = 0, ik = ik0; k < C; k++)
                    w += (Uc[ik++] * Uc[jk++]);
                meatrix[ij++] = w;
            }
        }
        free(Uc);
    } else {
        memset(meatrix, 0x00, sizeof(double) * (P * (P + 1)) / 2);
        for (int i = 0; i < N; i++) {
            double w = resid[i] * weights[i];
            w = w * w;
            for (int j = 0, ij = i, jk = 0; j < P; j++, ij += N) {
                double Xbij = Xb[ij];
                for (int k = 0, ik = i; k <= j; k++, ik += N, jk++)
                    meatrix[jk] += w * Xbij * Xb[ik];
            }
        }
    }
}

} // namespace utils
} // namespace genesis
