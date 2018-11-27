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
     * @brief Threshold for singluarities. Used as `eta = 1.0 - r2max`.
     */
    double r2max = 0.99;
};

struct GlmExtras
{
    std::vector<double> initial_fittings;
    std::vector<double> priors;
    std::vector<int>    strata;
};

struct GlmOutput
{
    bool converged = false;
    size_t num_iterations = 0;

    size_t rank = 0;
    double scale = 1.0;
    size_t df_resid = 0;

    Matrix<double> Xb; // size N * M
    std::vector<double> fitted; // size N
    std::vector<double> resid; // size N
    std::vector<double> weights; // size N

    std::vector<double> which; // size M
    std::vector<double> betaQ; // size M
    std::vector<double> tri; // size (M * (M+1)) / 2
};

/* Fit a base model */

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

GlmOutput glm_fit(
    int family, int link, int S,
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
