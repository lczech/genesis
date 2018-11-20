#ifndef GENESIS_UTILS_MATH_MDS_H_
#define GENESIS_UTILS_MATH_MDS_H_

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

// ================================================================================================
//     Settings for Functions
// ================================================================================================

/**
 * @brief Choice of algorithm to use for Multi-Dimensional Scaling (MDS).
 *
 * @see multi_dimensional_scaling() for details on the function and for the license.
 * @see More information is avaialbe in the
 * @link supplement_acknowledgements_code_reuse_mds Acknowledgements@endlink.
 */
enum class MdsAlgorithm
{
    /**
     * @brief Use the UCF implementation (recommended).
     *
     * This is a re-implementation of Laurens van der Maaten's MDS in his
     * [Matlab Toolbox for Dimensionality Reduction](http://crcv.ucf.edu/source/dimension).
     */
    kUcf,

    /**
     * @brief Use the SMACOF implementation.
     *
     * This is a re-implementation of Michael Bronstein's SMACOF in his
     * [Matlab Toolbox for Surface Comparison and Analysis](http://tosca.cs.technion.ac.il/).
     */
    kSmacof
};

// ================================================================================================
//     Multi-Dimensional Scaling
// ================================================================================================

/**
 * @brief Multi-Dimensional Scaling (MDS).
 *
 * Run MDS with randomly initialized embedding values.
 *
 * @see @link multi_dimensional_scaling( Matrix<double> const&, Matrix<double> const&, size_t, size_t, MdsAlgorithm ) multi_dimensional_scaling()@endlink
 * for details on the function, its parameters, and the license of the implementation.
 */
Matrix<double> multi_dimensional_scaling(
    Matrix<double> const& distances,
    size_t                dimensions = 2,
    size_t                iterations = 100,
    MdsAlgorithm          algorithm = MdsAlgorithm::kUcf
);

/**
 * @brief Multi-Dimensional Scaling (MDS).
 *
 * The function expectes a square @p distances matrix as well as a set of initial embedding values.
 * See @link multi_dimensional_scaling( Matrix<double> const&, size_t, size_t, MdsAlgorithm ) multi_dimensional_scaling()@endlink
 * for a version that uses randomly initialized embedding values.
 * The function then runs multi-dimensional scaling (MDS) for a given number of @p dimensions,
 * and optimizes the embeddings in @p iterations. It offers two choices for the MDS @p algorithm,
 * of which we recommend the default one, see ::MdsAlgorithm.
 *
 * The implementation is based on the
 * [SimpleMatrix library](https://sites.google.com/site/simpmatrix/) by
 * [Quan Wang](https://github.com/wq2012), with his explicit permission to use this code here.
 * The copyright (C) of the implementation is held by Quan Wang, 2013.
 * We adapted the implementation to our @link genesis::utils::Matrix Matrix@endlink class and
 * changed the error reporting mechanism to exceptions.
 * For further details, see the
 * @link supplement_acknowledgements_code_reuse_mds Acknowledgements@endlink.
 */
Matrix<double> multi_dimensional_scaling(
    Matrix<double> const& distances,
    Matrix<double> const& initial_values,
    size_t                dimensions = 2,
    size_t                iterations = 100,
    MdsAlgorithm          algorithm = MdsAlgorithm::kUcf
);

} // namespace utils
} // namespace genesis

#endif // include guard
