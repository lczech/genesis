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

int wcenter(const double *y, int n, const double *weight, const int *stratum,
        int nstrata, int resid, double *ynew);

double wresid(const double *y, int n, const double *weight, const double *x,
       double *ynew);


/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

double wssq(const double *y, int n, const double *weight);

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

double wsum(const double *y, int n, const double *weight);

/**
 * @brief
 *
 * See the @link supplement_acknowledgements_code_reuse_glm Acknowledgements@endlink for details
 * on the license and original authors.
 */

double wspr(const double *y, const double *x, int n, const double *weight);

} // namespace utils
} // namespace genesis

#endif // include guard
