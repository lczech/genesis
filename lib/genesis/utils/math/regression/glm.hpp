#ifndef GENESIS_UTILS_MATH_REGRESSION_GLM_H_
#define GENESIS_UTILS_MATH_REGRESSION_GLM_H_

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
 * @ingroup utils
 */

#include "genesis/utils/containers/matrix.hpp"

#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Generalized Linear Model
// =================================================================================================

/* Family */

#define BINOMIAL  1
#define POISSON   2
#define GAUSSIAN  3
#define GAMMA     4

/* Link */

#define LOGIT     1
#define LOG       2
#define IDENTITY  3
#define INVERSE   4

/* GLM definition functions */

double varfun(int, double);
double validmu(int, double);
double loglik(int, double, double);
double linkfun(int, double);
double invlink(int, double);
double dlink(int, double);

struct GlmInput
{
    int family;
    int link;
    int N;
    int M;
    int S;
    const double *y;
    const double *prior;
    const double *X;
    const int *strata;
    int init;
};

struct GlmExtras
{
    std::vector<double> initial_fittings;
    std::vector<double> priors;
    std::vector<size_t> strata;
    bool const          with_intercept = true;
};

struct GlmControl
{
    /**
     * @brief Maximum number of iterations to run the IRLS algorithm for.
     */
    size_t max_iterations = 25;

    /**
     * @brief Minimal difference in log likelihood between two iterations of the IRLS algorithm
     * that we consider for convergence.
     */
    double epsilon = 1.e-5;

    /**
     * @brief Threshold for singluarities. Internally used as `eta = 1.0 - max_r2`.
     */
    double max_r2 = 0.99;
};

struct GlmOutput
{
    bool converged = false;
    size_t num_iterations = 0;

    size_t rank = 0;
    size_t df_resid = 0;
    double scale = 1.0;

    Matrix<double> Xb; // size N * M
    std::vector<double> fitted; // size N
    std::vector<double> resid; // size N
    std::vector<double> weights; // size N

    std::vector<double> which; // size M
    std::vector<double> betaQ; // size M
    std::vector<double> tri; // size (M * (M+1)) / 2
};

/* Fit a base model */

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
strata      If S>1, strata assignments coded 1...S (N-vector)
maxit        Maximum number of iterations of IRLS algorithm
conv         Proportional change in weighted sum of squares residuals to
             declare convergence
max_r2        Maximum value of R^2 between an X variable and previous variables
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
// const double *prior, const double *X, const int *strata, int maxit,
// double epsilon, double max_r2, int init,
// int *rank, double *Xb,
// double *fitted, double *resid, double *weights, double *scale,
// int *df_resid, int *P_est, int *which, double *betaQ, double *tri

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

GlmOutput glm_fit(
    int family, int link,
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmExtras const&           extras = {},
    GlmControl const&          control = {}
);

/* Score test for additional terms */

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

void glm_score_test(int N, int M, int S, const int *strata,
            int P, const double *Z, int C, const int *cluster,
            const double *resid, const double *weights,
            const double *Xb, double scale,
            double max_r2, double *U, double *V);

/* Parameter estimation */

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

void glm_est(int P_est, const double *betaQ, double *tri,
         double scale, const double *meatrix,
         double *beta, double *var_beta);

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

void meat_matrix(int N, int P, int C, const int *cluster,
         const double *Xb, const double *resid, const double *weights,
         double *meatrix);



} // namespace utils
} // namespace genesis

#endif // include guard
