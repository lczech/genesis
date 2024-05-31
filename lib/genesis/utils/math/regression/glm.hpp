#ifndef GENESIS_UTILS_MATH_REGRESSION_GLM_H_
#define GENESIS_UTILS_MATH_REGRESSION_GLM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
#include "genesis/utils/math/regression/family.hpp"
#include "genesis/utils/math/regression/link.hpp"

#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     GLM Data Structures
// =================================================================================================

struct GlmExtras
{
    std::vector<double> initial_fittings;
    std::vector<double> prior_weights;
    bool                with_intercept = true;

    /**
     * @brief Strata assignments coded `1...S`.
     */
    std::vector<size_t> strata;

    enum ResidualType
    {
        kDefault,
        kPearsonResiduals,
        kDevianceResiduals
    };

    ResidualType residual_type = ResidualType::kDefault;

    /**
     * @brief Calculate mean `null_deviance` and mean `deviance` instead of their sums.
     *
     * By default, (`mean_deviance = false`), we calculate the `null_deviance` and `deviance`
     * as the sum of unit deviances (see GlmFamily::unit_deviance). If we however set
     * `mean_deviance = true`, we divide these values by the number of data points,
     * that is, we calculate their mean.
     */
    bool mean_deviance = false;
};

struct GlmControl
{
    /**
     * @brief Maximum number of iterations to run the IRLS algorithm for (if needed).
     */
    size_t max_iterations = 25;

    /**
     * @brief Proportional change in weighted sum of squares residuals to declare convergence
     * between two iterations of the IRLS algorithm.
     */
    double epsilon = 1.e-5;

    /**
     * @brief Threshold for singluarities. Internally used as `eta = 1.0 - max_r2`.
     *
     * Maximum value of `R^2` between an X variable and previous variables it is dropped as aliased.
     */
    double max_r2 = 0.99;
};

struct GlmOutput
{
    bool converged = false;
    size_t num_iterations = 0;

    /**
     * @brief Rank of X after regression on strata.
     */
    size_t rank = 0;

    /**
     * @brief Residual degrees of freedom.
     */
    size_t df_resid = 0;

    /**
     * @brief Scale factor (scalar).
     */
    double scale = 1.0;

    /**
     * @brief Orthogonal basis for X space (`N * M` matrix, with `N * rank` being used).
     */
    Matrix<double> Xb;

    /**
     * @brief Fitted values (size `N`).
     */
    std::vector<double> fitted;

    /**
     * @brief Working residuals (on linear predictor scale) (size `N`).
     */
    std::vector<double> resid;

    /**
     * @brief Weights (size `N`)
     */
    std::vector<double> weights;

    /**
     * @brief Which columns in the X matrix were estimated (first = 0) (size `M`).
     */
    std::vector<double> which;

    /**
     * @brief Vector of parameter estimates (in terms of basis matrix, Xb) (size `M`).
     *
     * Use glm_estimate_betas() to transform this back into the basis of the original predictors.
     */
    std::vector<double> betaQ;

    /**
     * @brief Upper unit triangular transformation matrix, with Xb - tr.Xb placed in the diagonal
     * (size `(M * (M+1)) / 2`).
     */
    std::vector<double> tri;

    /**
     * @brief Null deviance.
     *
     * Goodness of fit of a model with only the intercept (no predictors) compared to a
     * perfect model, indicating how well the model with no predictors explains the variability
     * in the response variable.
     */
    double null_deviance = 0.0;

    /**
     * @brief Deviance.
     *
     * Difference in goodness of fit between the fitted model and the perfect model,
     * quantifying how well the model with predictors explains the variability in the response
     * variable compared to the null model.
     */
    double deviance = 0.0;
};

// =================================================================================================
//     GLM Fit
// =================================================================================================

/**
 * @brief Fit a Generalized Linear Model (GLM).
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */
GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmFamily const&           family,
    GlmLink const&             link,
    GlmExtras const&           extras = {},
    GlmControl const&          control = {}
);

/**
 * @brief Fit a Generalized Linear Model (GLM).
 *
 * Uses the canonical link function of the provided distribution family.
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */
GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmFamily const&           family,
    GlmExtras const&           extras = {},
    GlmControl const&          control = {}
);

/**
 * @brief Fit a Generalized Linear Model (GLM) using a linear gaussian model.
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */
GlmOutput glm_fit(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmExtras const&           extras = {},
    GlmControl const&          control = {}
);

// =================================================================================================
//     GLM Output
// =================================================================================================

/**
 * @brief Compute the beta estimates resulting from a glm_fit().
 *
 * The GlmOutput::betaQ result expresses the betas in terms of the GlmOutput::Xb basis space,
 * which is an orthogonal representaton of the original predictor matrix. To turn this into betas
 * expressed in the original predictor column space, this function inverts the triangular
 * transformation matrix GlmOutput::tri, and uses this to transform the betaQ into betas.
 */
std::vector<double> glm_estimate_betas( GlmOutput const& output );

// /**
//  * @brief Obtain beta estimates and variance covariance matrix of estimates from output the output
//  * of glm_fit().
//  *
//  * The resulting variance covariance matrix is a packed symmetric matrix with the size of the
//  * number of predictor variables (which is the size of the betas).
//  * Robust variance is calculated if the "meat" matrix for the information sandwich is supplied.
//  */
// std::pair<std::vector<double>, std::vector<double>> glm_estimate_betas_and_var_covar(
//     GlmOutput const& output,
//     std::vector<double> const& meat = std::vector<double>{}
// );

/**
 * @brief Compute the intercept resulting from a glm_fit().
 *
 * This takes the input and output of the glm_fit(), as well as the list of @p betas in the original
 * predictor column space, which is computed by glm_estimate_betas().
 *
 * Assumes identity link function such as from glm_link_identity(). See the other overload of the
 * function to specify the link function.
 */
double glm_estimate_intercept(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmOutput const&           output,
    std::vector<double> const& betas
);

/**
 * @brief Compute the intercept resulting from a glm_fit().
 *
 * Overload where the link function can be specified, for links other than identity.
 *
 * Note that this function computes the intercept in the original space of the predictors and
 * response. For instance, assuming the data has been generated like this:
 *
 *     beta_0 = 0.5
 *     beta_1 = 0.3
 *     X = np.linspace(start=1, stop=10, num=10)
 *     Y = np.exp(beta_0 + beta_1 * X)
 *
 * and the GLM has accordingly been run with glm_family_poisson()/glm_link_log(), that is, with a
 * log link. Then, this function here computes the intercept as originally specified, i.e., `0.5`.
 */
double glm_estimate_intercept(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmLink const&             link,
    GlmOutput const&           output,
    std::vector<double> const& betas
);

/**
 * @brief Compute the model coefficients of a glm_fit().
 *
 * This simply calls glm_estimate_intercept() and glm_estimate_betas(), and stores the results
 * in a vector, i.e., the intercept, followed by the beta coefficients. This is hence a
 * convenience function for cases where all coefficients are needed. See these two functions
 * for specifics on the values that they are computing.
 *
 * Assumes identity link function such as from glm_link_identity(). See the other overload of the
 * function to specify the link function.
 */
std::vector<double> glm_coefficients(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmOutput const&           output
);

/**
 * @brief Compute the model coefficients of a glm_fit().
 *
 * Overload where the link function can be specified, for links other than identity.
 *
 * Note that the functions glm_estimate_intercept() and glm_estimate_betas() compute their
 * values in the original space of the predictors and response, and _not_ on the link scale.
 * This is particularly important when using a link function other than identity. See
 * glm_estimate_intercept() for an example of the value computed.
 */
std::vector<double> glm_coefficients(
    Matrix<double> const&      x_predictors,
    std::vector<double> const& y_response,
    GlmLink const&             link,
    GlmOutput const&           output
);

} // namespace utils
} // namespace genesis

#endif // include guard
