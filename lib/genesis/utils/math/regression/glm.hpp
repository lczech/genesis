#ifndef GENESIS_UTILS_MATH_REGRESSION_GLM_H_
#define GENESIS_UTILS_MATH_REGRESSION_GLM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Generalized Linear Model
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
    bool         mean_deviance = false;
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
     */
    std::vector<double> betaQ;

    /**
     * @brief Upper unit triangular transformation matrix, with Xb - tr.Xb placed in the diagonal
     * (size `(M * (M+1)) / 2`).
     */
    std::vector<double> tri;

    double null_deviance = 0.0;
    double deviance = 0.0;
};

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

} // namespace utils
} // namespace genesis

#endif // include guard
