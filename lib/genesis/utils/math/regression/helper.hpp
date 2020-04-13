#ifndef GENESIS_UTILS_MATH_REGRESSION_HELPER_H_
#define GENESIS_UTILS_MATH_REGRESSION_HELPER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @brief Internal helper structure for GLMs to calcualte the residual degrees of freedom.
 */
struct GlmFreedom
{
    /**
     * @brief Number of valid priors (Nu).
     */
    size_t valid_entries = 0;

    /**
     * @brief Number of empty strata.
     */
    size_t empty_strata = 0;

    /**
     * @brief Maximum stratum found (S).
     */
    size_t max_stratum = 1;

    /**
     * @brief Calculate the degrees of freedom (dfr).
     */
    long degrees_of_freedom( size_t rank ) const
    {
        auto const vi = static_cast<long>( valid_entries );
        auto const mi = static_cast<long>( max_stratum );
        auto const ei = static_cast<long>( empty_strata );
        auto const ri = static_cast<long>( rank );

        return vi - mi + ei - ri;
    }
};

/**
 * @brief (Weighted) mean and centering.
 *
 * If @p centering is `false`, write @p y_output to contain the (strata) (weighted) means.
 * If @p centering is `true`, center the input @p y_input around these means, i.e., calculaute either
 * the "fitted value" or the residual from a model in which only strata are fitted.
 *
 * The @p weights and @p strata can be empty.
 * @p y_input and @p y_output can be the same vector.
 */
GlmFreedom weighted_mean_centering(
    std::vector<double> const& y_input,
    std::vector<double> const& weights,
    std::vector<size_t> const& strata,
    bool                       with_intercept,
    bool                       centering,
    std::vector<double>&       y_output
);

/**
 * @brief Calculate the residuals from (weighted) regression through the origin.
 *
 * The @p weights can be empty.
 * The results are written to @p y_output.
 * @p y_input and @p y_output can be the same vector.
 * Returns the regression coefficient.
 */
double weighted_residuals(
    std::vector<double> const& x_input,
    std::vector<double> const& y_input,
    std::vector<double> const& weights,
    std::vector<double>&       y_output
);

/**
 * @brief (Weighted) sum of squares.
 *
 * The @p weights can be empty, in which case the simple sum of squares of @p x_input is returned.
 */
double weighted_sum_of_squares(
    std::vector<double> const& x_input,
    std::vector<double> const& weights = std::vector<double>{}
);

/**
 * @brief (Weighted) inner product of two vectors.
 *
 * The @p weights can be empty, in which case the simple inner product of @p x_input
 * and @p y_input is returned.
 */
double weighted_inner_product(
    std::vector<double> const& x_input,
    std::vector<double> const& y_input,
    std::vector<double> const& weights = std::vector<double>{}
);

/**
* @brief (Weighted) sum of a vector of values.
*
* The @p weights can be empty, in which case the simple sum of @p x_input is returned.
*/
double weighted_sum(
    std::vector<double> const& x_input,
    std::vector<double> const& weights = std::vector<double>{}
);

} // namespace utils
} // namespace genesis

#endif // include guard
