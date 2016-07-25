#ifndef GENESIS_PLACEMENT_PQUERY_DISTANCES_H_
#define GENESIS_PLACEMENT_PQUERY_DISTANCES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class TreeNode;
    class TreeEdge;

}

namespace placement {

    class  Pquery;
    class  PqueryPlacement;
    struct PqueryPlain;

    class  PlacementEdgeData;
    class  PlacementNodeData;

}

namespace utils {
    template <typename T>
    class Matrix;
}

// =================================================================================================
//     Distance Measuring
// =================================================================================================

namespace placement {
/*
// double distance (
//     const PqueryPlacement& placement_a,
//     const PqueryPlacement& placement_b
// );

double distance (
    const PqueryPlacement& placement_a,
    const PqueryPlacement& placement_b,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const Pquery& pquery,
    const utils::Matrix<double>& node_distance_matrix
);

// double distance (
//     const PqueryPlain& pqry_a,
//     const PqueryPlain& pqry_b
// );

double distance (
    const PqueryPlain& pqry_a,
    const PqueryPlain& pqry_b,
    const utils::Matrix<double>& node_distance_matrix
);

// double distance (
//     const Pquery& pqry_a,
//     const Pquery& pqry_b
// );

double distance (
    const Pquery& pqry_a,
    const Pquery& pqry_b,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const PqueryPlacement& placement,
    const PlacementTreeNode& node,
    const utils::Matrix<double>& node_distance_matrix
);

double distance (
    const Pquery& pquery,
    const PlacementTreeNode& node,
    const utils::Matrix<double>& node_distance_matrix
);
*/

} // namespace placement
} // namespace genesis

#endif // include guard
