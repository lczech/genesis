#ifndef GENESIS_PLACEMENT_FUNCTION_NHD_H_
#define GENESIS_PLACEMENT_FUNCTION_NHD_H_

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
 * @ingroup placement
 */

#include <cstddef>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace placement {

    class Sample;
    class SampleSet;

}

namespace tree {

    class Tree;

}

namespace utils {

    template<typename T>
    class Matrix;

}

namespace placement {

// =================================================================================================
//     Node Distance Histogram
// =================================================================================================

/**
 * @brief Simple histogram data structure with equal sized bins.
 *
 * This struct is used as a fast and light-weight alternative to the proper utils::Histogram class
 * for calcualting Node Histogram distances.
 */
struct NodeDistanceHistogram
{
    double min;
    double max;
    std::vector<double> bins;
};

/**
 * @brief Collection of NodeDistanceHistogram%s that describes one Sample.
 */
struct NodeDistanceHistogramSet
{
    std::vector<NodeDistanceHistogram> histograms;
};

// =================================================================================================
//     Basic Functions
// =================================================================================================

/**
 * @brief Calcualte the NodeDistanceHistogramSet representing a single Sample, given the necessary
 * matrices of this Sample.
 *
 * This is a basic function that is mainly used for speedup in applications. The two matrices
 * only depend on the tree, but not on the placement data, so for a set of Samples with the same
 * tree, they only need to be calculated once.
 */
NodeDistanceHistogramSet node_distance_histogram_set(
    Sample const& sample,
    utils::Matrix<double> const& node_distances,
    utils::Matrix<signed char> const& node_sides,
    size_t const  histogram_bins
);

/**
 * @brief Given the histogram sets that describe two Sample%s, calculate their distance.
 */
double node_histogram_distance(
    NodeDistanceHistogramSet const& lhs,
    NodeDistanceHistogramSet const& rhs
);

/**
 * @brief Given the histogram sets that describe a set of Sample%s, calculate their pairwise
 * distance matrix.
 */
utils::Matrix<double> node_histogram_distance(
    std::vector<NodeDistanceHistogramSet> const& histogram_sets
);

// =================================================================================================
//     High Level Functions
// =================================================================================================

/**
* @brief Calculate the Node Histogram Distance of two Sample%s.
*
* The necessary matrices of the Samples are calculated, then their NodeDistanceHistogramSet are
* build, and finally the distance is calcualted. Basically, this is a high level
* function that simply chains node_distance_histogram_set() and
* @link node_histogram_distance( NodeDistanceHistogramSet const&, NodeDistanceHistogramSet const& ) node_histogram_distance()@endlink
* for convenience.
*/
double node_histogram_distance(
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins = 25
);

/**
* @brief Calculate the Node Histogram Distance of every pair of Sample%s in the SampleSet.
*
* This is a high level convenience function that takes a whole SampleSet, calculates
* the necessary matrices, builds the histograms, and calculates their distances.
*/
utils::Matrix<double> node_histogram_distance(
    SampleSet const& sample_set,
    size_t const     histogram_bins = 25
);

} // namespace placement
} // namespace genesis

#endif // include guard
