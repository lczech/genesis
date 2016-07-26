#ifndef GENESIS_PLACEMENT_FUNCTION_MEASURES_H_
#define GENESIS_PLACEMENT_FUNCTION_MEASURES_H_

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
 * @brief Header for Placement Measures functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class TreeNode;
    class TreeEdge;

}

namespace placement {

    class Sample;
    class SampleSet;
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;
    struct PqueryPlain;

}

namespace utils {
    template<typename T>
    class Matrix;

    class Histogram;
}

// =================================================================================================
//     Placement Measures
// =================================================================================================

namespace placement {

double pquery_distance (
    const PqueryPlain&     pqry_a,
    const PqueryPlain&     pqry_b,
    const utils::Matrix<double>&  node_distances,
    bool                   with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Earth Movers Distance
// ----------------------------------------------------------------------------

double earth_movers_distance (
    const Sample& lhs,
    const Sample& rhs,
    bool          with_pendant_length = false
);

utils::Matrix<double> earth_movers_distance(
    SampleSet const& sample_set,
    bool             with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Center of Gravity
// ----------------------------------------------------------------------------

std::pair<PlacementTreeEdge const*, double> center_of_gravity (
    const Sample& smp,
    bool          with_pendant_length = false
);

double center_of_gravity_variance (
    const Sample& smp,
    bool          with_pendant_length = false
);

double center_of_gravity_distance (
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length = false
);

// ----------------------------------------------------------------------------
//     Pairwise Distance
// ----------------------------------------------------------------------------

double pairwise_distance (
    const Sample& smp_a,
    const Sample& smp_b,
    bool          with_pendant_length = false
);

// double closest_pair_distance (
//     const Sample& smp_a,
//     const Sample& smp_b,
//     bool                with_pendant_length = false
// );

// ----------------------------------------------------------------------------
//     Node-Based Distances
// ----------------------------------------------------------------------------

double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t        histogram_bins = 25
);

utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t           histogram_bins = 25
);

// ----------------------------------------------------------------------------
//     Variance
// ----------------------------------------------------------------------------

double variance (
    const Sample& smp,
    bool          with_pendant_length = false
);

} // namespace placement
} // namespace genesis

#endif // include guard
