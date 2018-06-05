#ifndef GENESIS_TREE_MASS_TREE_SQUASH_CLUSTERING_H_
#define GENESIS_TREE_MASS_TREE_SQUASH_CLUSTERING_H_

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
 * @ingroup tree
 */

#include "genesis/tree/mass_tree/tree.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Squash Clustering
// =================================================================================================

/**
 * @brief Perform Squash Clustering.
 *
 * The class performs squash clustering and stores the results.
 */
class SquashClustering
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    struct Cluster
    {
        /**
         * @brief The MassTree that this cluster represetns.
         *
         * In the beginning of the algorithm, those are simply the MassTree%s of the Sample%s. Those
         * are then successively merged to form bigger clusters.
         */
        MassTree tree;

        /**
         * @brief How many end points (Sample%s) does this cluster represent?
         *
         * We need this information for calculating the weighted average of the Sample masses when
         * merging two clusters.
         */
        size_t count;

        /**
         * @brief Is this cluster active, i.e., is it not yet part of a larger cluster?
         *
         * Only active clusters are considered for merging.
         */
        bool active;

        /**
         * @brief Distances from this cluster to all clusters with a lower index in the `clusters`
         * vector.
         *
         * We don't store the distances in a global distance matix, but in a vector for each cluster
         * instead, as this makes it trivial to keep track of the data when merging clusters.
         * No need to keep track of which row belongs to which cluster etc.
         */
        std::vector<double> distances;
    };

    struct Merger
    {
        /**
         * @brief Index of the first data point in the cluster.
         */
        size_t index_a;

        /**
        * @brief Distance of the first data point to the cluster node.
        */
        double distance_a;

        /**
         * @brief Index of the second data point in the cluster.
         */
        size_t index_b;

        /**
        * @brief Distance of the second data point to the cluster node.
        */
        double distance_b;
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    SquashClustering() = default;
    ~SquashClustering() = default;

    SquashClustering( SquashClustering const& ) = default;
    SquashClustering( SquashClustering&& )      = default;

    SquashClustering& operator= ( SquashClustering const& ) = default;
    SquashClustering& operator= ( SquashClustering&& )      = default;

    // -------------------------------------------------------------------------
    //     Public Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Perfom Squash Clustering.
     *
     * See [the guppy documentation](http://matsen.github.io/pplacer/generated_rst/guppy_squash.html#guppy-squash)
     * and [the corresponding paper](http://arxiv.org/abs/1107.5095) for details on this algorithm.
     *
     * The funciton takes MassTree%s as input, which are consumed. The optional parameter @p p is used
     * as exponent to calculate the earth_movers_distance(). See there for details.
     */
    void run( std::vector<MassTree>&& trees );

    /**
     * @brief Build a Newick-format tree for visualizing the result of a squash_clustering().
     *
     * The resulting Tree is a tree of samples, i.e., each leaf node represents one MassTree that was
     * used as input for the Squash Clustering. The @p labels vector needs to contain the labels for
     * those tips, in the order of elements that was used for running squash_clustering().
     */
    std::string tree_string( std::vector<std::string> const& labels ) const;

    SquashClustering& p( double value )
    {
        p_ = value;
        return *this;
    }

    double p() const
    {
        return p_;
    }

    std::vector<Cluster> const& clusters() const
    {
        return clusters_;
    }

    std::vector<Merger> const& mergers() const
    {
        return mergers_;
    }

    /**
     * @brief Clear the clusters() and mergers() data.
     */
    void clear();

    // -------------------------------------------------------------------------
    //     Progress Report
    // -------------------------------------------------------------------------

    std::function<void( void )>                   report_initialization;
    std::function<void( size_t i, size_t total )> report_step;

    // -------------------------------------------------------------------------
    //     Private Functions
    // -------------------------------------------------------------------------

private:

    void init_( std::vector<MassTree>&& trees );
    std::pair<size_t, size_t> min_entry_() const;
    void merge_clusters_( size_t i, size_t j );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double p_ = 1.0;

    std::vector<Cluster> clusters_;
    std::vector<Merger>  mergers_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
