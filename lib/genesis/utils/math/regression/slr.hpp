#ifndef GENESIS_UTILS_MATH_REGRESSION_SLR_H_
#define GENESIS_UTILS_MATH_REGRESSION_SLR_H_

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

#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>

namespace genesis {
namespace utils {

// =================================================================================================
//     Simple Linear Regression
// =================================================================================================

/**
 * @brief Data structer to keep the two parameters of a linear function:
 * its @link LinearFunction::slope slope@endlink,
 * and its @link LinearFunction::intercept intercept@endlink.
 *
 * It also has a function to calcualte `y = slope * x + intercept`.
 */
struct LinearFunction
{
    double slope;
    double intercept;

    double y( double x ) const
    {
        return slope * x + intercept;
    }
};

/**
 * @brief Simple linear regression, for predicting the dependent variable `y` given the independent
 * variable `x`, using ordinary least squares regression.
 *
 * See https://en.wikipedia.org/wiki/Simple_linear_regression for an explanation.
 *
 * @see mean_squared_error() for calculating the resulting error.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
LinearFunction simple_linear_regression(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y
) {
    // Calculate means of x and y := Mean(x), Mean(y) in parallel.
    double mean_x = 0.0;
    double mean_y = 0.0;
    size_t count = 0;
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        mean_x += val_x;
        mean_y += val_y;
        ++count;
    });
    if( count == 0 ) {
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
    }
    assert( count > 0 );
    mean_x /= static_cast<double>( count );
    mean_y /= static_cast<double>( count );
    assert( std::isfinite( mean_x ));
    assert( std::isfinite( mean_y ));

    // Calculate Cov(x,y) := covariance of x and y, and Var(x) := variance of x.
    double covariance = 0.0;
    double variance_x = 0.0;
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        double const dx = val_x - mean_x;
        double const dy = val_y - mean_y;
        covariance += dx * dy;
        variance_x += dx * dx;
    });
    assert( std::isfinite( covariance ));
    assert( std::isfinite( variance_x ));

    // The linear parameters are slope := Cov(x,y) / Var(x), and intercept = Mean(y) - slope * Mean(x).
    LinearFunction result;
    result.slope = covariance / variance_x;
    result.intercept = mean_y - result.slope * mean_x;
    return result;
}

/**
 * @brief Calculate the mean squared error obtained from a linear fit of the input variables.
 *
 * The error per data point `(x,y)` is calculated as the squared differences between `y`
 * and the prediction given by the linear function @p lin_fct for `x`.
 * The function returns the mean of the errors for all data points.
 *
 * @see simple_linear_regression() for calculating such a fit.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double mean_squared_error(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y,
    LinearFunction lin_fct
) {
    double error = 0.0;
    size_t count = 0;

    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        double const e = val_y - lin_fct.y( val_x );
        error += e * e;
        ++count;
    });

    if( count == 0 ) {
        assert( error == 0.0 );
        return error;
    }
    return error / static_cast<double>( count );
}

/**
 * @brief Calculate the fraction of unexplained variance resulting from a linear fit of the input
 * variables.
 *
 * See https://en.wikipedia.org/wiki/Fraction_of_variance_unexplained for some details.
 *
 * @see simple_linear_regression() for calculating such a fit.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double fraction_of_variance_unexplained(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y,
    LinearFunction lin_fct
) {
    // Prepare mean of y and the count of valid value pairs.
    double y_mean = 0.0;
    size_t count = 0;

    // First get mean of y. We use the x iterator only to make sure that we skip invalid pairs.
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double, double y_val ){
        y_mean += y_val;
        ++count;
    });
    y_mean /= static_cast<double>( count );

    // Edge case.
    if( count == 0 ) {
        return 0.0;
    }

    // Prepare sums of squares.
    double ss_err = 0.0;
    double ss_tot = 0.0;
    // double ss_reg = 0.0;

    // Iterate again, this time calculate the components needed.
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double x_val, double y_val ){
        double const y_hat = lin_fct.y( x_val );

        double const d_err = y_val - y_hat;
        double const d_tot = y_val - y_mean;
        // double const d_reg = y_hat - y_mean;

        ss_err += d_err * d_err;
        ss_tot += d_tot * d_tot;
        // ss_reg += d_reg * d_reg;
    });

    auto const fvu = ( ss_err / ss_tot );
    // auto const fvu = ( 1.0 - ss_reg / ss_tot );
    assert( 0.0 <= fvu && fvu <= 1.0 );
    return fvu;
}

} // namespace utils
} // namespace genesis

#endif // include guard
