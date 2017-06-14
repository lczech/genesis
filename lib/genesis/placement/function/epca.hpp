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

#include "genesis/placement/placement_tree.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/pca.hpp"

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

/**
 * @brief Calculate the imbalance of placement mass for each @link PlacementTreeEdge Edge@endlink
 * of the given Sample.
 *
 * The entries of the vector are the difference between the distribution of mass on either side of
 * the edge for the given Sample. Specifically, it is the amount of mass on the distal (non-root)
 * side of the edge minus the amount of mass on the proximal side.
 *
 * The vector is indexed using the @link PlacementTreeEdge::index() index()@endlink of the edges.
 * This is different from how how [guppy](https://matsen.github.io/pplacer/generated_rst/guppy.html)
 * indexes the edges, namely by using their @link PlacementEdgeData::edge_num() edge_nums@endlink.
 * See https://matsen.github.io/pplacer/generated_rst/guppy_splitify.html for details on the guppy
 * edge imbalance matrix. We chose to use our internal edge index instead, as it is consistent and
 * needs no checking for correctly labeled edge nums.
 *
 * @see epca_imbalance_matrix() for the @link utils::Matrix Matrix@endlink of imbalances for a whole
 * SampleSet.
 */
std::vector<double> epca_imbalance_vector( Sample const& sample );

/**
 * @brief Calculate the imbalance matrix of placment mass for all Sample%s in a SampleSet.
 *
 * The first step to perform @link epca() Edge PCA@endlink is to make a
 * @link utils::Matrix Matrix@endlink with rows indexed by the Sample%s, and columns by the
 * @link PlacementTreeEdge Edges@endlink of the Tree. Each entry of this matrix is the
 * difference between the distribution of mass on either side of an edge for a Sample.
 * Specifically, it is the amount of mass on the distal (non-root) side of the edge minus the amount
 * of mass on the proximal side.
 *
 * The matrix is row-indexed according to the Sample%s in the SampleSet.
 *
 * If @p include_leaves is set to `false` (default), the columns for edges belonging to leaves of
 * the tree are left out. Their value is `-1.0` anyway, as there is no mass on the distal side of
 * those edges. Hence, they are constant for all Samples and have no effect on the Edge PCA result.
 * In this case, the matrix is column-indexed so that each inner edge of the Tree has one column
 * in the Matrix. See epca_imbalance_vector() for more details.
 *
 * If @p include_leaves is set to `true`, the constant values for leaf edges are also included.
 * In this case, the matrix is column-indexed according to the edge indices of the Tree.
 * This is for example useful if the indexing is needed later. The columns can then also be filtered
 * out using epca_filter_constant_columns().
 */
utils::Matrix<double> epca_imbalance_matrix( SampleSet const& samples, bool include_leaves = false );

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

/**
 * @brief Filter out columns that have nearly constant values, measured using an @p epsilon.
 *
 * The @link utils::Matrix Matrix@endlink is modified so that all columns `c` with
 * `max(c) - min(c) <= epsilon` are removed.
 *
 * The function returns a sorted list of all column indices of the original matrix that are kept,
 * i.e., that have a greater min-max difference than @p epsilon. This is useful for e.g.,
 * visualising the result of an @link epca() Edge PCA@endlink.
 *
 * @param[in,out] imbalance_matrix Matrix to filter inplace.
 * @param         epsilon          Maximum deviation for what is considered constant.
 * @return                         List of original column indices that are kept (not filtered).
 */
std::vector<size_t> epca_filter_constant_columns(
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
