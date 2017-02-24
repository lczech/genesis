#ifndef GENESIS_PLACEMENT_FUNCTION_EPCA_H_
#define GENESIS_PLACEMENT_FUNCTION_EPCA_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"
#include "utils/math/matrix.hpp"
#include "utils/math/matrix/pca.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class Tree;

    class DefaultNodeData;
    class DefaultEdgeData;

    using DefaultTree = Tree;
}

namespace placement {
    class PlacementEdgeData;
    class PlacementNodeData;

    class Sample;
    class SampleSet;
}

namespace placement {

// =================================================================================================
//     Edge PCA
// =================================================================================================

std::vector<double> epca_imbalance_vector( Sample const& smp );

utils::Matrix<double> epca_imbalance_matrix( SampleSet const& samples );

/**
 * @brief Perform a component-wise transformation of the imbalance matrix used for epca().
 *
 * All entries of the Matrix are transformed inplace, using
 *
 * \f[
 *     \varphi_\kappa(x) = \mathrm{sgn}(x) \cdot |x|^\kappa
 * \f]
 *
 * where the `kappa` (\f$\kappa\f$) parameter can be any non-negative number. This parameter scales
 * between ignoring abundance information (`kappa` = 0), using it linearly (`kappa` = 1), and
 * emphasizing it (`kappa` > 1).
 *
 * @param[in,out] imbalance_matrix Matrix to transform inplace.
 * @param[in]     kappa            Scaling value for abundance information. Has to be > 0.
 */
void epca_splitify_transform(
    utils::Matrix<double>& imbalance_matrix,
    double                 kappa = 1.0
);

void epca_filter_constant_columns(
    utils::Matrix<double>& imbalance_matrix,
    double                 epsilon = 1e-5
);

utils::PcaData epca(
    SampleSet const& samples,
    double kappa      = 1.0,
    double epsilon    = 1e-5,
    size_t components = 0
);

} // namespace placement
} // namespace genesis

#endif // include guard
