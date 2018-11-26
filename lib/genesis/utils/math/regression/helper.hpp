#ifndef GENESIS_UTILS_MATH_REGRESSION_HELPER_H_
#define GENESIS_UTILS_MATH_REGRESSION_HELPER_H_

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

#include <cstddef>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Linear Algebra Helper Functions
// =================================================================================================

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

// old
int wcenter_(
    std::vector<double> const& y, int n, std::vector<double> const& weight, const int *stratum,
    int nstrata, int resid, double *ynew
);

/**
 * @brief (Weighted) mean and centering.
 *
 * If @p centering is `false`, write @p y_new to contain the (strata) (weighted) means.
 * If @p centering is `true`, center the input @p y around these means, i.e., calculaute either
 * the "fitted value" or the residual from a model in which only strata are fitted.
 *
 * The @p weights and @p strata can be empty.
 * @p y and @p y_new can be the same vector.
 */
size_t weighted_centering(
    std::vector<double> const& y,
    std::vector<double> const& weights,
    std::vector<int> const&    strata,
    bool                       with_intercept,
    bool                       centering,
    std::vector<double>&       y_new
);



// old
double wresid_(
    std::vector<double> const& y, int n, std::vector<double> const& weight, std::vector<double> const& x,
    double *ynew
);

/**
 * @brief Calculate the residuals from (weighted) regression through the origin.
 *
 * The @p weights can be empty.
 * The results are written to @p y_new.
 * @p y and @p y_new can be the same vector.
 * Returns the regression coefficient.
 */
double weighted_residuals(
    std::vector<double> const& x,
    std::vector<double> const& y,
    std::vector<double> const& weights,
    std::vector<double>&       y_new
);

/**
 * @brief (Weighted) sum of squares.
 *
 * The @p weights can be empty, in which case the simple sum of squares of @p x is returned.
 */
double weighted_sum_of_squares(
    std::vector<double> const& x,
    std::vector<double> const& weights = {}
);

/**
 * @brief (Weighted) inner product of two vectors.
 *
 * The @p weights can be empty, in which case the simple inner product of @p x and @p y is returned.
 */
double weighted_inner_product(
    std::vector<double> const& x,
    std::vector<double> const& y,
    std::vector<double> const& weights = {}
);

/**
* @brief (Weighted) sum of a vector of values.
*
* The @p weights can be empty, in which case the simple sum of @p x is returned.
*/

double weighted_sum(
    std::vector<double> const& x,
    std::vector<double> const& weights = {}
);

} // namespace utils
} // namespace genesis

#endif // include guard
