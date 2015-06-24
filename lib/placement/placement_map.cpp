/**
 * @brief Implementation of PlacementMap class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <utility>

#ifdef PTHREADS
#    include <thread>
#endif

#include "tree/tree_view.hpp"
#include "utils/logging.hpp"
#include "utils/matrix.hpp"
#include "utils/options.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =================================================================================================
//     Constructor & Destructor
// =================================================================================================

/**
 * @brief Copy constructor.
 */
PlacementMap::PlacementMap (const PlacementMap& other)
{
    clear();

    // use assignment operator to create copy of the tree and metadata.
    *tree_   = *other.tree_;
    metadata = other.metadata;

    // copy all data of the tree: do a preorder traversal on both trees in parallel
    PlacementTree::IteratorPreorder it_n = tree_->begin_preorder();
    PlacementTree::IteratorPreorder it_o = other.tree_->begin_preorder();
    for (
        ;
        it_n != tree_->end_preorder() && it_o != other.tree_->end_preorder();
        ++it_n, ++it_o
    ) {
        // the trees are copies of each other, they need to have the same rank. otherwise,
        // the copy constructor is not working!
        assert(it_n.node()->rank() == it_o.node()->rank());

        it_n.edge()->placements.clear();
        it_n.edge()->branch_length = it_o.edge()->branch_length;
        it_n.edge()->edge_num = it_o.edge()->edge_num;

        it_n.node()->name = it_o.node()->name;
    }

    // the trees are copies. they should take equal iterations to finish a traversal.
    assert(it_n == tree_->end_preorder() && it_o == other.tree_->end_preorder());

    // copy all (o)ther pqueries to (n)ew pqueries
    // TODO this is very similar to merge(). make this code shared somehow.
    auto en_map = edge_num_map();
    for (const auto& opqry : other.pqueries_) {
        auto npqry = make_unique<Pquery>();
        pqueries_.push_back(std::move(npqry));

        for (const auto& op : opqry->placements) {
            auto np = make_unique<PqueryPlacement>(*op);

            np->edge   = en_map[np->edge_num];
            np->edge->placements.push_back(np.get());
            np->pquery = npqry.get();
            npqry->placements.push_back(std::move(np));
        }
        for (const auto& on : opqry->names) {
            auto nn = make_unique<PqueryName>(*on);

            nn->pquery = npqry.get();
            npqry->names.push_back(std::move(nn));
        }
    }
}

/**
 * @brief Assignment operator. See Copy constructor for details.
 */
PlacementMap& PlacementMap::operator = (const PlacementMap& other)
{
    // check for self-assignment.
    if (&other == this) {
        return *this;
    }

    // the PlacementMap tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    PlacementMap tmp(other);
    std::swap(pqueries_, tmp.pqueries_);
    tree_->swap(*tmp.tree_);
    std::swap(metadata, tmp.metadata);
    return *this;
}

/**
 * @brief Destructor. Calls clear() to delete all data.
 */
PlacementMap::~PlacementMap()
{
    clear();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Creats an empty Pquery, adds it to the PlacementMap and returns a pointer to it.
 *
 * The returned pointer can then be used to add Placements and Names to the Pquery.
 */
Pquery* PlacementMap::add_pquery()
{
    pqueries_.push_back(make_unique<Pquery>());
    return pqueries_.back().get();
}

/**
 * @brief Adds the pqueries from another PlacementMap objects to this one.
 *
 * For this method to succeed, the PlacementMaps need to have the same topology, including identical
 * edge_nums and node names.
 *
 * The resulting tree is the original one of the PlacementMap on which this method was called. If
 * instead the average branch length tree is needed, see PlacementMapSet::merge_all().
 */
bool PlacementMap::merge(const PlacementMap& other)
{
    // Check for identical topology, taxa names and edge_nums.
    // We do not check here for branch_length, because usually those differ slightly.
    auto comparator = [] (
        PlacementTree::ConstIteratorPreorder& it_l,
        PlacementTree::ConstIteratorPreorder& it_r
    ) {
        return it_l.node()->name     == it_r.node()->name     &&
               it_l.edge()->edge_num == it_r.edge()->edge_num;
    };

    if (!tree_->equal(*other.tree_, comparator)) {
        LOG_WARN << "Cannot merge PlacementMaps with different reference trees.";
        return false;
    }

    // We need to assign edge pointers to the correct edge objects, so we need a mapping.
    auto en_map = edge_num_map();

    // Copy all (o)ther pqueries to (n)ew pqueries.
    for (const auto& opqry : other.pqueries_) {
        auto npqry = make_unique<Pquery>();
        for (const auto& op : opqry->placements) {
            auto np = make_unique<PqueryPlacement>(*op);

            // Assuming that the trees have identical topology (checked at the beginning of this
            // function), there will be an edge for every placement. if this assertion fails,
            // something broke the integrity of our in memory representation of the data.
            assert(en_map.count(np->edge_num) > 0);
            np->edge = en_map[np->edge_num];
            np->edge->placements.push_back(np.get());
            np->pquery = npqry.get();
            npqry->placements.push_back(std::move(np));
        }
        for (const auto& on : opqry->names) {
            auto nn = make_unique<PqueryName>(*on);
            nn->pquery = npqry.get();
            npqry->names.push_back(std::move(nn));
        }
        this->pqueries_.push_back(std::move(npqry));
    }
    return true;
}

/**
 * @brief Clears all data of this object.
 *
 * The pqueries, the tree and the metadata are deleted.
 */
void PlacementMap::clear()
{
    pqueries_.clear();
    tree_ = std::make_shared<PlacementTree>();
    metadata.clear();
}

/**
 * @brief Clears all placements of this PlacementMap.
 *
 * All pqueries are deleted. However, the Tree and metadata are left as they are, thus this is a
 * useful method for simulating placements: Take a copy of a given map, clear its placements, then
 * generate new ones using PlacementSimulator.
 */
void PlacementMap::clear_placements()
{
    for (auto it = tree_->begin_edges(); it != tree_->end_edges(); ++it) {
        (*it)->placements.clear();
    }
    pqueries_.clear();
}

// =================================================================================================
//     Helper Methods
// =================================================================================================

/**
 * @brief Returns a mapping of edge_num integers to the corresponding Edge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
PlacementMap::EdgeNumMapType PlacementMap::edge_num_map() const
{
    auto en_map = EdgeNumMapType();
    for (
        PlacementTree::ConstIteratorEdges it = tree_->begin_edges();
        it != tree_->end_edges();
        ++it
    ) {
        PlacementTree::EdgeType* edge = *it;
        assert(en_map.count(edge->edge_num) == 0);
        en_map.emplace(edge->edge_num, edge);
    }
    return std::move(en_map);
}

/**
 * @brief Returns a plain representation of all pqueries of this map.
 *
 * This method produces a whole copy of all pqueries and their placements (though, not their names)
 * in a plain POD format. This format is meant for speeding up computations that need access to
 * the data a lot - which would require several pointer indirections in the normal representation
 * of the data.
 *
 * This comes of course at the cost of reduced flexibility, as all indices are fixed in the
 * plain data structre: changing a value here will not have any effect on the original data or
 * even on the values of the pqueries. Thus, most probably this will lead to corruption. Therefore,
 * this data structure is meant for reading only.
 */
std::vector<PqueryPlain> PlacementMap::plain_queries() const
{
    auto pqueries = std::vector<PqueryPlain>(pqueries_.size());
    for (size_t i = 0; i < pqueries_.size(); ++i) {
        pqueries[i].index = i;

        const auto& opqry = pqueries_[i];
        pqueries[i].placements = std::vector<PqueryPlacementPlain>(opqry->placements.size());

        for (size_t j = 0; j < opqry->placements.size(); ++j) {
            const auto& oplace = opqry->placements[j];
            auto& place = pqueries[i].placements[j];

            place.edge_index           = oplace->edge->index();
            place.primary_node_index   = oplace->edge->primary_node()->index();
            place.secondary_node_index = oplace->edge->secondary_node()->index();

            place.branch_length        = oplace->edge->branch_length;
            place.pendant_length       = oplace->pendant_length;
            place.proximal_length      = oplace->proximal_length;
            place.like_weight_ratio    = oplace->like_weight_ratio;
        }
    }
    return pqueries;
}

/**
 * @brief Recalculates the `like_weight_ratio` of the placements of each Pquery so that their sum
 * is 1.0, while maintaining their ratio to each other.
 */
void PlacementMap::normalize_weight_ratios()
{
    for (auto& pqry : pqueries_) {
        double sum = 0.0;
        for (auto& place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
        for (auto& place : pqry->placements) {
            place->like_weight_ratio /= sum;
        }
    }
}

/**
 * @brief Removes all placements but the most likely one from all pqueries.
 *
 * Pqueries can contain multiple placements on different branches. For example, the EPA algorithm
 * of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by
 * default. The property `like_weight_ratio` weights those placement positions so that the sum over
 * all positions per pquery is 1.0.
 *
 * This function removes all but the most likely placement (the one which has the maximal
 * `like_weight_ratio`) from each Pquery. It additionally sets the `like_weight_ratio` of the
 * remaining placement to 1.0, as this one now is the only one left, thus it's "sum" has to be 1.0.
 */
void PlacementMap::restrain_to_max_weight_placements()
{
    for (auto& pqry : pqueries_) {
        // Initialization.
        double           max_w = -1.0;
        PqueryPlacement* max_p;

        for (auto& place : pqry->placements) {
            // Find the maximum of the weight ratios in the placements of this pquery.
            if (place->like_weight_ratio > max_w) {
                max_w = place->like_weight_ratio;
                max_p = place.get();
            }

            // Delete the reference from the edge to the current placement. We will later add the
            // one that points to the remaining (max weight) placement back to its edge.
            std::vector<PqueryPlacement*>::iterator it = place->edge->placements.begin();
            for (; it != place->edge->placements.end(); ++it) {
                if (*it == place.get()) {
                    break;
                }
            }

            // Assert that the edge actually contains a reference to this pquery. If not,
            // this means that we messed up somewhere else while adding/removing placements...
            assert(it != place->edge->placements.end());
            place->edge->placements.erase(it);
        }

        // Remove all but the max element from placements vector.
        auto neq = [max_p] (const std::unique_ptr<PqueryPlacement>& other)
        {
            return other.get() != max_p;
        };
        auto pend = std::remove_if (pqry->placements.begin(), pqry->placements.end(), neq);
        pqry->placements.erase(pend, pqry->placements.end());

        // Now add back the reference from the edge to the pquery.
        // Assert that we now have a single placement in the pquery (the most likely one).
        assert(pqry->placements.size() == 1 && pqry->placements[0].get() == max_p);
        max_p->edge->placements.push_back(max_p);

        // Also, set the like_weight_ratio to 1.0, because we do not have any other placements left.
        max_p->like_weight_ratio = 1.0;
    }
}

// =================================================================================================
//     Placement Mass
// =================================================================================================

/**
 * @brief Get the total number of placements in all pqueries.
 */
size_t PlacementMap::placement_count() const
{
    size_t count = 0;
    for (const auto& pqry : pqueries_) {
        count += pqry->placements.size();
    }
    return count;
}

/**
 * @brief Get the summed mass of all placements on the tree, given by their `like_weight_ratio`.
 */
double PlacementMap::placement_mass() const
{
    double sum = 0.0;
    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
    }
    return sum;
}

/**
 * @brief Returns a histogram representing how many placements have which depth with respect to
 * their closest leaf node.
 *
 * The depth between two nodes on a tree is the number of edges between them. Thus, the depth of a
 * placement (which sits on an edge of the tree) to a specific node is the number of edges between
 * this node and the closer one of the two nodes at the end of the edge where the placement sits.
 *
 * The closest leaf to a placement is thus the leaf node which has the smallest depth to that
 * placement. This function then returns a histogram of how many placements (values of the vector)
 * are there that have a specific depth (indices of the vector) to their closest leaf.
 *
 * Example: A return vector of
 *
 *     histogram[0] = 2334
 *     histogram[1] = 349
 *     histogram[2] = 65
 *     histogram[3] = 17
 *
 * means that there are 2334 placements that sit on an edge which leads to a leaf node (thus, the
 * depth of one of the nodes of the edge is 0). It has 349 placements that sit on an edge where
 * one of its nodes has one neighbour that is a leaf; and so on.
 *
 * The vector is automatically resized to the needed number of elements.
 */
std::vector<int> PlacementMap::closest_leaf_depth_histogram() const
{
    std::vector<int> hist;

    // Get a vector telling us the depth from each node to its closest leaf node.
    PlacementTree::NodeIntVectorType depths = tree_->closest_leaf_depth_vector();

    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            // Try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            int dp = depths[place->edge->primary_node()->index()].second;
            int ds = depths[place->edge->secondary_node()->index()].second;
            unsigned int ld = std::min(dp, ds);

            // Put the closer one into the histogram, resize if necessary.
            if (ld + 1 > hist.size()) {
                hist.resize(ld + 1, 0);
            }
            ++hist[ld];
        }
    }

    return hist;
}

/**
 * @brief Returns a histogram counting the number of placements that have a certain distance to
 * their closest leaf node, divided into equally large intervals between a min and a max distance.
 *
 * The distance range between min and max is divided into `bins` many intervals of equal size.
 * Then, the distance from each placement to its closest leaf node is calculated and the counter for
 * this particular distance inverval in the histogram is incremented.
 *
 * The distance is measured along the `branch_length` values of the edges, taking the
 * `pendant_length` and `proximal_length` of the placements into account. If the distances is
 * outside of the interval [min,max], the counter of the first/last bin is incremented respectively.
 *
 * Example:
 *
 *     double min      =  0.0;
 *     double max      = 20.0;
 *     int    bins     = 25;
 *     double bin_size = (max - min) / bins;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 * %
 */
std::vector<int> PlacementMap::closest_leaf_distance_histogram (
    const double min, const double max, const int bins
) const {
    std::vector<int> hist(bins, 0);
    double bin_size = (max - min) / bins;

    // get a vector telling us the distance from each node to its closest leaf node.
    PlacementTree::NodeDoubleVectorType dists = tree_->closest_leaf_distance_vector();

    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length + place->proximal_length
                      + dists[place->edge->primary_node()->index()].second;
            double ds = place->pendant_length + place->edge->branch_length - place->proximal_length
                      + dists[place->edge->secondary_node()->index()].second;
            double ld = std::min(dp, ds);

            // find the right bin. if the distance value is outside the boundaries of [min;max],
            // place it in the first or last bin, respectively.
            int bin = static_cast <int> (std::floor( (ld - min) / bin_size ));
            if (bin < 0) {
                bin = 0;
            }
            if (bin >= bins) {
                bin = bins - 1;
            }
            ++hist[bin];
        }
    }

    return hist;
}

/**
 * @brief Returns the same type of histogram as closest_leaf_distance_histogram(), but automatically
 * determines the needed boundaries.
 *
 * See closest_leaf_distance_histogram() for general information about what this function does. The
 * difference between both functions is that this one first procresses all distances from
 * placements to their closest leaf nodes to find out what the shortest and longest are, then sets
 * the boundaries of the histogram accordingly. The number of bins is then used to divide this
 * range into intervals of equal size.
 *
 * The boundaries are returned by passing two doubles `min` and `max` to the function by reference.
 * The value of `max` will actually contain the result of std::nextafter() called on the longest
 * distance; this makes sure that the value itself will be placed in the interval.
 *
 * Example:
 *
 *     double min, max;
 *     int    bins = 25;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     double bin_size = (max - min) / bins;
 *     LOG_INFO << "Histogram boundaries: [" << min << "," << max << ").";
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 *
 * It has a slightly higher time and memory consumption than the non-automatic version
 * closest_leaf_distance_histogram(), as it needs to process the values twice in order to find their
 * min and max.
 */
std::vector<int> PlacementMap::closest_leaf_distance_histogram_auto (
    double& min, double& max, const int bins
) const {
    std::vector<int> hist(bins, 0);

    // we do not know yet where the boundaries of the histogram lie, so we need to store all values
    // first and find their min and max.
    std::vector<double> distrib;
    double min_d = 0.0;
    double max_d = 0.0;

    // get a vector telling us the distance from each node to its closest leaf node.
    PlacementTree::NodeDoubleVectorType dists = tree_->closest_leaf_distance_vector();

    // calculate all distances from placements to their closest leaf and store them.
    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length + place->proximal_length
                      + dists[place->edge->primary_node()->index()].second;
            double ds = place->pendant_length + place->edge->branch_length - place->proximal_length
                      + dists[place->edge->secondary_node()->index()].second;
            double ld = std::min(dp, ds);
            distrib.push_back(ld);

            // update min and max as needed (and in first iteration). we use std::nextafter() for
            // the max in order to make sure that the max value is actually placed in the last bin.
            if (distrib.size() == 1 || ld < min_d) {
                min_d = ld;
            }
            if (distrib.size() == 1 || ld > max_d) {
                max_d = std::nextafter(ld, ld + 1);
            }
        }
    }

    // now we know min and max of the distances, so we can calculate the histogram.
    double bin_size = (max_d - min_d) / bins;
    for (double ld : distrib) {
        int bin = static_cast <int> (std::floor( (ld - min_d) / bin_size ));
        assert(bin >= 0 && bin < bins);
        ++hist[bin];
    }

    // report the min and max values to the calling function and return the histogram.
    min = min_d;
    max = max_d;
    return hist;
}

// =================================================================================================
//     Distances
// =================================================================================================

/**
 * @brief Calculates the normalized distance between two plain pqueries. It is mainly a helper
 * method for distance calculations (e.g., pairwise distance, variance).
 *
 * For each placement in the two pqueries, a distance is calculated, and their normalized sum is
 * returned. Normalization is done using the mass of placements in both pqueries.
 *
 * The distance between two placements is calculated as the shortest path between them. This
 * includes the their position on the branches, and - if specified - the pendant_length of both.
 * There are three cases that might occur:
 *
 *   1. **Both placements are on the same branch.**
 *      In this case, their distance is caluclated as their difference in proximal_lengths (plus
 *      if specified the sum of their pendant_lengths).
 *
 *   2. **The path between the placements includes the root.**
 *      The distance of a placement from its neighbouring nodes is usually given in form of the
 *      proximal_length, which is the distance of the placement to the node (at the end of its
 *      branch) that lies in direction of the root. Thus, there is an implicit notion of a root,
 *      that we need to consider. If the path between two placements contains the root, we can
 *      directly calculate their distance as the distance between the two promixal nodes plus
 *      proximal_lengths (and possibly pendant_lengths) of both placements. We call this the
 *      promixal-promixal case.
 *
 *   3. **The root is not part of the path between the placements.**
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the proximal_lengths
 *      directly, but we need to get the distal_length (away from the root) of the inner placement
 *      first. This is simply the difference between branch_length and proximal_length of that
 *      placement. Of course, this case comes in two flavours, because both placements can be the
 *      inner or outer one. They are called proximal-distal case and distal-proximal case,
 *      respectively.
 *
 * The first case is easy to detect by comparing the edge nums. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of any
 * algorithm using this method and contains the pairwise distances between all nodes of the tree.
 * Using this, we do not need to find paths between placements, but simply go to the nodes at the
 * end of the branches of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance
 * is then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the total distance between the pqueries.
 */
double PlacementMap::pquery_distance (
    const PqueryPlain&     pqry_a,
    const PqueryPlain&     pqry_b,
    const Matrix<double>&  node_distances,
    const bool             with_pendant_length
) {
    double sum = 0.0;
    double pp, pd, dp, dist;

    for (const PqueryPlacementPlain& place_a : pqry_a.placements) {
        for (const PqueryPlacementPlain& place_b : pqry_b.placements) {
            if (place_a.edge_index == place_b.edge_index) {
                // same branch case
                dist = std::abs(place_a.proximal_length - place_b.proximal_length);

            } else {
                // proximal-proximal case
                pp = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // proximal-distal case
                pd = place_a.branch_length - place_a.proximal_length
                   + node_distances(place_a.secondary_node_index, place_b.primary_node_index)
                   + place_b.proximal_length;

                // distal-proximal case
                dp = place_a.proximal_length
                   + node_distances(place_a.primary_node_index, place_b.secondary_node_index)
                   + place_b.branch_length - place_b.proximal_length;

                // find min of the three cases and
                dist = std::min(pp, std::min(pd, dp));
            }

            //  If needed, use pendant length; normalize it to the weight ratios.
            if (with_pendant_length) {
                dist += place_a.pendant_length + place_b.pendant_length;
            }
            dist *= place_a.like_weight_ratio * place_b.like_weight_ratio;
            sum  += dist;
        }
    }

    return sum;
}

/**
 * @brief Calculates the Earth Movers Distance to another sets of placements on a fixed reference
 * tree.
 */
double PlacementMap::earth_movers_distance(
    const PlacementMap& right,
    const bool with_pendant_length
) const {
    return PlacementMap::earth_movers_distance(*this, right, with_pendant_length);
}

/**
 * @brief Calculates the Earth Movers Distance between two sets of placements on a fixed reference
 * tree.
 */
double PlacementMap::earth_movers_distance(
    const PlacementMap& lhs,
    const PlacementMap& rhs,
    const bool with_pendant_length
) {
    // keep track of the total resulting distance.
    double distance = 0.0;

    // store a per-node balance of mass. each element in this map contains information about how
    // much placement mass is pushing from the direction of this node towards the root.
    // caveat: the masses that are stored here are already fully pushed towards the root, but are
    // stored here using the node at the lower end of the branch as key.
    std::unordered_map<const PlacementTree::NodeType*, double> balance;

    // use the sum of masses as normalization factor for the masses.
    double totalmass_l = lhs.placement_mass();
    double totalmass_r = rhs.placement_mass();

    // disable all debug messages for this function...
    Logging::LoggingLevel ll = Logging::max_level();
    Logging::max_level(Logging::kInfo);

    LOG_DBG << "totalmass_l " << totalmass_l;
    LOG_DBG << "totalmass_r " << totalmass_r;

    // do a postorder traversal on both trees in parallel. while doing so, move placements
    // from the leaves towards the root and store their movement (mass * distance) in balance[].
    // in theory, it does not matter where we start the traversal - however, the positions of the
    // placements are given as "proximal_length" on their branch, which always points away from the
    // root. thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account. so we do start at the root, to keep it simple.
    PlacementTree::IteratorPostorder it_l = lhs.tree_->begin_postorder();
    PlacementTree::IteratorPostorder it_r = rhs.tree_->begin_postorder();
    for (
        ;
        it_l != lhs.tree_->end_postorder() && it_r != rhs.tree_->end_postorder();
        ++it_l, ++it_r
    ) {
        LOG_DBG << "\033[1;31miteration at node " << it_l.node()->index_ << ": " << it_l.node()->name << "\033[0m";
        LOG_DBG << "current distance " << distance;

        // check whether both trees have identical topology. if they have, the ranks of all nodes
        // are the same. however, if not, at some point their ranks will differ.
        if (it_l.node()->rank() != it_r.node()->rank()) {
            LOG_WARN << "Calculating EMD on different reference trees not possible.";
            return -1.0;
        }

        // if we are at the last iteration, we reached the root, thus we have moved all masses now
        // and don't need to proceed. if we did, we would count an edge of the root again.
        if (it_l.is_last_iteration()) {
            LOG_DBG1 << "last iteration";
            // we do a check for the mass at the root here for debug purposes.
            double root_mass = 0.0;
            for (
                PlacementTree::NodeType::IteratorLinks n_it = it_l.node()->begin_links();
                n_it != it_l.node()->end_links();
                ++n_it
            ) {
                assert(balance.count(n_it.link()->outer()->node()));
                root_mass += balance[n_it.link()->outer()->node()];
            }

            LOG_DBG << "Mass at root: " << root_mass;

            continue;
        }

        // check whether the data on both reference trees is the same. this has to be done after the
        // check for last iteration / root node, because we don't want to check this for the root.
        if (it_l.node()->name     != it_r.node()->name ||
            it_l.edge()->edge_num != it_r.edge()->edge_num
        ) {
            LOG_WARN << "Inconsistent reference trees in EMD calculation.";
            return -1.0;
        }

        // we now start a "normal" earth_movers_distance caluclation on the current edge. for this,
        // we store the masses of all placements sorted by their position on the branch.
        std::multimap<double, double> edge_balance;
        LOG_DBG1 << "placing on branch...";

        // add all placements of the branch from the left tree (using positive mass)...
        for (PqueryPlacement* place : it_l.edge()->placements) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio * place->pendant_length / totalmass_l;
            }
            edge_balance.emplace(place->proximal_length, +place->like_weight_ratio / totalmass_l);

            LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_l edge   " << it_l.edge()->index_;
            LOG_DBG2 << "added dist  " << place->like_weight_ratio * place->pendant_length / totalmass_l;
            LOG_DBG2 << "new dist    " << distance;
            LOG_DBG2 << "emplaced at " << place->proximal_length << ": " << +place->like_weight_ratio / totalmass_l;
            LOG_DBG2;
        }

        // ... and the branch from the right tree (using negative mass)
        for (PqueryPlacement* place : it_r.edge()->placements) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio * place->pendant_length / totalmass_r;
            }
            edge_balance.emplace(place->proximal_length, -place->like_weight_ratio / totalmass_r);

            LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_r edge   " << it_r.edge()->index_;
            LOG_DBG2 << "added dist  " << place->like_weight_ratio * place->pendant_length / totalmass_r;
            LOG_DBG2 << "new dist    " << distance;
            LOG_DBG2 << "emplaced at " << place->proximal_length << ": " << -place->like_weight_ratio / totalmass_r;
            LOG_DBG2;
        }

        LOG_DBG1 << "placed all.";

        // distribute placement mass between children of this node, and collect the remaining mass
        // in mass_s. mass_s then contains the rest mass of the subtree that could not be
        // distributed among the children and thus has to be moved upwards.
        double mass_s = 0.0;
        PlacementTree::LinkType* link = it_l.link()->next();
        while (link != it_l.link()) {
            // we do postorder traversal, so we have seen the child nodes of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link->outer()->node()));

            mass_s += balance[link->outer()->node()];
            link = link->next();
        }
        LOG_DBG1 << "subtrees mass_s " << mass_s;
        LOG_DBG1 << "entering standard emd part...";

        // start the earth_movers_distance with the mass that is left over from the subtrees...
        double cur_pos  = it_l.edge()->branch_length;
        double cur_mass = mass_s;

        LOG_DBG1 << "cur_pos  " << cur_pos;
        LOG_DBG1 << "cur_mass " << cur_mass;

        // ... and move it along the branch, balancing it with the placements found on the branches.
        // this is basically a standard earth_movers_distance calculation along the branch.
        std::multimap<double, double>::reverse_iterator rit;
        for (rit = edge_balance.rbegin(); rit != edge_balance.rend(); ++rit) {
            LOG_DBG2 << "at " << rit->first << " with " << rit->second;
            //~ LOG_INFO << "at " << rit->first << " with " << rit->second;
            //~ assert(cur_pos >= rit->first);
            LOG_DBG2 << "is " << cur_pos << " >= " << rit->first << "? with " << rit->second;
            //~ if (cur_pos < rit->first) {
                //~ LOG_INFO << "is " << cur_pos << " >= " << rit->first << "? with " << rit->second;
                //~ return -1.0;
            //~ }
            distance += std::abs(cur_mass) * (cur_pos - rit->first);
            LOG_DBG2 << "added dist " << std::abs(cur_mass) * (cur_pos - rit->first);
            LOG_DBG2 << "new dist   " << distance;

            cur_pos   = rit->first;
            cur_mass += rit->second;

            LOG_DBG2 << "cur_pos  " << cur_pos;
            LOG_DBG2 << "cur_mass " << cur_mass;
            LOG_DBG2;
        }

        // finally, move the rest to the end of the branch and store its mass in balance[],
        // so that it can be used for the nodes further up in the tree.
        distance += std::abs(cur_mass) * cur_pos;
        balance[it_l.node()] = cur_mass;

        LOG_DBG1 << "added dist " << std::abs(cur_mass) * cur_pos;
        LOG_DBG1 << "new dist   " << distance;
        LOG_DBG1 << "balance at node " << it_l.node()->index_ << ": " << it_l.node()->name << " = " << cur_mass;
        LOG_DBG1 << "finished standard emd part";
        LOG_DBG1;
    }

    // check whether we are done with both trees.
    if (it_l != lhs.tree_->end_postorder() || it_r != rhs.tree_->end_postorder()) {
        LOG_WARN << "Inconsistent reference trees in EMD calculation.";
        return -1.0;
    }

    LOG_DBG << "final distance: " << distance;
    Logging::max_level(ll);

    return distance;
}

/**
 * @brief Calculate the Center of Gravity of the placements on a tree.
 *
 * The center of gravity is the point on the tree where all weights of the placements on the one
 * side of it times their distance from the point are equal to this sum on the other side of the
 * point. In the following example, the hat `^` marks this point on a line with two placements:
 * One has weight 1 and distance 3 from the central point, and one as weight 3 and distance 1,
 * so that their respective mass with respect to the point is the same:
 *
 *                   3
 *                   |
 *     1             |
 *     |_____________|
 *               ^
 *
 * This calculation is done for the whole tree, with the masses calculated from the
 * `like_weight_ratio` and (if specificed in the method parameter) the `pendant_length` of the
 * placements.
 */
void PlacementMap::center_of_gravity (const bool with_pendant_length) const
{
    // --------------------------------------------------------------------------------------
    // TODO What happens here with short branches and even worse with branches of length 0???
    //      This will probably screw up everything!
    // --------------------------------------------------------------------------------------

    // Store a balance of mass per link, so that each element contains the mass that lies downwards
    // the tree in the direction of this link.
    std::unordered_map<PlacementTree::LinkType*, double> balance;

    // Do a postorder traversal. Collect all placement masses and push them towards the root.
    for (
        PlacementTree::IteratorPostorder it = this->tree_->begin_postorder();
        it != this->tree_->end_postorder();
        ++it
    ) {
        // Node does not have a corresponding edge (this was e.g. the case in the old tree
        // implementation, where the root had a "virtual" up link; this is not the case any more,
        // but maybe it will be re-introduced some day, and it does no harm to leave it here).
        // Also skip the last iteration, as we would assign an unneeded value to the first child
        // of the root.
        if (!it.edge() || it.is_last_iteration()) {
            continue;
        }

        // Collect the mass that lies further down in the tree.
        double mass = 0.0;

        // Add up the masses from the current node's children.
        PlacementTree::LinkType* link = it.link()->next();
        while (link != it.link()) {
            // We do postorder traversal, so we have seen the child links of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link));

            mass += balance[link] * it.edge()->branch_length;
            link  = link->next();
        }

        // Add up the masses of placements on the current branch.
        for (PqueryPlacement* place : it.edge()->placements) {
            double p_mass = place->proximal_length;
            if (with_pendant_length) {
                p_mass += place->pendant_length;
            }
            mass += p_mass * place->like_weight_ratio;
        }

        assert(balance.count(it.link()->outer()) == 0);
        balance[it.link()->outer()] = mass;
    }

    // Now we have calculated all massed that lie down the tree as seen from the root. We can now
    // start finding the edge where the center of gravity lies. This is done by going down the tree
    // in the direction where the most mass comes from and at the same time pulling with us all the
    // masses that come from the other nodes. Once we are pulling more mass from behind us (speak:
    // up in the tree) that lies ahead of us (speak: down the tree), we have found the center edge.

    for (auto& v : balance) {
        LOG_DBG << "node " << v.first->node()->name << ", value " << v.second;
    }

    // Keep track of the link whose edge we are currently examining, as well as the one that we
    // examined previously (on iteration of the loop earlier). We start at the root.
    PlacementTree::LinkType* curr_link = tree_->root_link();
    PlacementTree::LinkType* prev_link = nullptr;

    // For asserting purposes, we keep track of the number of loop iterations we do.
    // This can never be more than the tree height (in number of nodes from root to deepest leaf)
    // plus one last iteration for going back towards the root.
    size_t num_iterations = 0;
    auto   depth_vector = tree_->node_depth_vector();
    size_t max_iterations = 1 + static_cast<size_t>(
        *std::max_element(depth_vector.begin(), depth_vector.end())
    );
    // TODO turn this thing into a function similar to tree.height(), and name it aptly (find some
    // better convention to distinguish between depth [number of nodes on a path] and distance [sum
    // of branch lengths]. maybe distance and lengths instead?!).
    // TODO once this method is established, this might be removed.

    LOG_DBG << "max it " << max_iterations;

    // Loop until the balancing edge is found.
    while (true) {
        assert (num_iterations <= max_iterations);
        ++num_iterations;

        LOG_DBG << "iteration " << num_iterations;
        LOG_DBG1 << "find max at " << curr_link->node()->name;

        // Find the direction away from the current node that has the highest mass.
        // At the same time, collect the sum of masses at the node, in order to push them
        // towards the node with highest mass later (so that the next iteration will have values
        // to work on).
        PlacementTree::LinkType* max_link = nullptr;
        double                   max_mass = -1.0;
        double                   mass_sum =  0.0;
        for (
            auto it_l = curr_link->node()->begin_links();
            it_l != curr_link->node()->end_links();
            ++it_l
        ) {
            // Make sure that we actually have a useable value.
            assert(balance.count(it_l.link()) > 0);

            LOG_DBG2 << it_l.link()->outer()->node()->name
                     << " "  << balance[it_l.link()]
                     << " " << balance[it_l.link()->outer()];
            if (balance[it_l.link()] > max_mass) {
                max_link = it_l.link();
                max_mass = balance[it_l.link()];
            }
            mass_sum += balance[it_l.link()];
        }

        // Check if we found the edge where the center of gravity lies. This is the case when the
        // the highest mass is coming from the direction where we came just from in the last
        // iteration.
        LOG_DBG1 << "moving to " << max_link->outer()->node()->name;
        if (max_link->outer() == prev_link) {
            LOG_DBG << "found at " << curr_link->node()->name;
            break;
        }

        // If we are not done yet, move down the edge.
        prev_link = max_link;
        curr_link = max_link->outer();

        LOG_DBG1 << "mass sum " << mass_sum;
        // Now we are at a node where we have calculated only the masses coming from further down in
        // the tree so far, but not the mass coming from the direction of the root (from where we
        // just came). So we need to calculate this mass now:

        // Subtract the mass of the direction where we found the most mass, so that all that is left
        // in mass_sum is the mass of all the other (not maximum) nodes. Then push it towards to
        // the end of the edge by multiplying it with the branch length.
        mass_sum -= balance[max_link];
        mass_sum *= max_link->edge()->branch_length;

        // Add masses of the placements on this edge.
        for (PqueryPlacement* place : max_link->edge()->placements) {
            double p_mass = max_link->edge()->branch_length - place->proximal_length;
            if (with_pendant_length) {
                p_mass += place->pendant_length;
            }
            mass_sum += p_mass * place->like_weight_ratio;
        }

        // Store the mass at the corresponding link.
        balance[curr_link] = mass_sum;
        LOG_DBG1 << "stored " << mass_sum << " at " << max_link->outer()->node()->name;

        LOG_DBG << "end of iteration " << num_iterations << "\n";
    }

    // Assert that the two links are actually the two ends of the same edge and that their nodes
    // are the correct ones in terms of direction to the root.
    assert(curr_link->edge() == prev_link->edge());
    assert(prev_link->node() == prev_link->edge()->primary_node());
    assert(curr_link->node() == curr_link->edge()->secondary_node());

    for (auto pair : balance) {
        LOG_DBG1 << pair.first->node()->name << ": " << pair.second << "\n";
        //~ distance += std::abs(pair.second);
    }

    LOG_DBG << "cur  " << curr_link->node()->name << " with " << balance[curr_link];
    LOG_DBG << "prev " << prev_link->node()->name << " with " << balance[prev_link];

    // At this point, we have found the central edge that balances the placement masses on the tree.
    // curr_link is at the downwards (away from the root) end of this edge, while prev_link at its
    // top (towards the root).
    // All that is left now is to find the correct position on this edge. For this, we need to
    // consider the masses that lie at both ends of the edge, as well as the placements on the edge
    // itself.

    assert(balance.count(prev_link) > 0);
    assert(balance.count(curr_link) > 0);

    // Define the two masses at the end of the edge: (p)roximal and (d)istal mass. Those are the
    // sums of the masses from the subtree that lies behind this end of the edge.
    double mass_dist = 0.0;
    double mass_prox = 0.0;

    PlacementTree::LinkType* link;
    link = curr_link->next();
    while (link != curr_link) {
        assert(balance.count(link));
        mass_dist += balance[link];
        link = link->next();
    }

    link = prev_link->next();
    while (link != prev_link) {
        assert(balance.count(link));
        mass_prox += balance[link];
        link = link->next();
    }

    LOG_DBG << "mass_dist " << mass_dist;
    LOG_DBG << "mass_prox " << mass_prox;

    // A different method of calculation those masses is to subtract all masses on the edge from
    // the balance that we already calculated. This yields the same values as the previous method,
    // but needs to do calculations for each placement on the edge, which might be many.
    // Here is the code for it. Maybe it is helpful in the future, if bugs are found.
    /*
    double mass_dist = balance[prev_link];
    double mass_prox = balance[curr_link];
    for (PqueryPlacement* place : curr_link->edge()->placements) {
        double p_mass = place->proximal_length;
        if (with_pendant_length) {
            p_mass += place->pendant_length;
        }
        mass_dist -= p_mass * place->like_weight_ratio;

        double d_mass = curr_link->edge()->branch_length - place->proximal_length;
        if (with_pendant_length) {
            d_mass += place->pendant_length;
        }
        mass_prox -= d_mass * place->like_weight_ratio;
    }
    mass_dist /= curr_link->edge()->branch_length;
    mass_prox /= curr_link->edge()->branch_length;
    */

    // A simple approximation of the solution is to calculate the balancing point on the edge
    // without considering the influence of the placements on the edge:
    // Let x the solution, measured as length from the proximal node.
    // Then we are looking for an x where the weights on both sides of it are in equilibrium:
    //         mass_prox * x = mass_dist * (branch_length - x)
    //     <=> x = (mass_dist * branch_length) / (mass_prox + mass_dist)
    // In code:
    double approx = (mass_dist * curr_link->edge()->branch_length) / (mass_prox + mass_dist);
    LOG_DBG << "approx " << approx;

    // We will do an iteration that moves along the edge, balancing the weights on both sides until
    // equilibrium is found. For this, we need to keep track of the masses on the two sides:
    // both variables hold the mass as seen from the point that we are trying to find. At first,
    // the prox_sum contains just mass_prox, while dist_sum contains all weights on the other side.
    // During the iteration later, this will change while moving along the edge, until equilibrium.
    double prox_sum = mass_prox;
    double dist_sum = balance[prev_link];
    // dist_sum can also be calcuated by initializing it with:
    //     dist_sum = mass_dist * curr_link->edge()->branch_length;
    // and then in the loop over all placements on the edge, add their weights to it:
    //     dist_sum += (place_prox + place_mass) * place->like_weight_ratio;
    // This value is however already stored in balance, so we don't need to recalculate it.

    // We store the masses of all placements on the edge, sorted by their position on it. Also, as
    // first and last element, we store the masses that we just calculated. This makes it obsolete
    // to check them as boundary cases. This map serves as basis for the iteration, and also as
    // lookup for the iteration that finds the center point.
    // Its format is: < proximal_length , < like_weight_ratio , mass or pendant_length > >
    std::multimap<double, std::pair<double, double>> edge_balance;
    edge_balance.emplace(0.0,                              std::make_pair(1.0, mass_prox));
    edge_balance.emplace(curr_link->edge()->branch_length, std::make_pair(1.0, mass_dist));

    // Now add all placements on the edge to the balance variables.
    for (PqueryPlacement* place : prev_link->edge()->placements) {
        double place_prox = place->proximal_length;

        // Some sanity checks for wrong data. We do it here because otherwise the algorithm might
        // produce weird results. Usually, this task is however up to the validate() method.
        if (place_prox > curr_link->edge()->branch_length) {
            LOG_WARN << "Placement found that has proximal_length > branch_length.";
            place_prox = curr_link->edge()->branch_length;
        }
        if (place_prox < 0.0) {
            LOG_WARN << "Placement found that has proximal_length < 0.0.";
            place_prox = 0.0;
        }

        double place_mass = 0.0;
        if (with_pendant_length) {
            place_mass += place->pendant_length;
        }

        edge_balance.emplace(place_prox, std::make_pair(place->like_weight_ratio, place_mass));
    }

    LOG_DBG << "prox_sum " << prox_sum;
    LOG_DBG << "dist_sum " << dist_sum;

    // This is the loop where we find the center of the edge.
    size_t edge_i = 0;
    for (auto& edge : edge_balance) {
        LOG_DBG1 << "at " << edge.first << " with ratio " << edge.second.first << " and mass " << edge.second.second;

        if (prox_sum > dist_sum) {
            /* code */
        }
    }
}

/**
 * @brief Calculate the pairwise distance between all placements of the two PlacementMaps.
 *
 * @param  other               The second PlacementMap to which the distances shall be calculated to.
 * @param  with_pendant_length Whether or not to include all pendant lengths in the calculation.
 *
 * @return                         Distance value.
 */
double PlacementMap::pairwise_distance (
    const PlacementMap& other, const bool with_pendant_length
) const {
    return pairwise_distance (*this, other, with_pendant_length);
}

/**
 * @brief Calculate the normalized pairwise distance between all placements of the two PlacementMaps.
 *
 * This method calculates the distance between two maps as the normalized sum of the distances
 * between all pairs of pqueries in the map. It is similar to the variance() calculation, which
 * calculates this sum for the squared distances between all pqueries of one map.
 *
 * @param  left                The first PlacementMap to which the distances shall be calculated to.
 * @param  right               The second PlacementMap to which the distances shall be calculated to.
 * @param  with_pendant_length Whether or not to include all pendant lengths in the calculation.
 *
 * @return                         Distance value.
 */
double PlacementMap::pairwise_distance (
    const PlacementMap& map_a, const PlacementMap& map_b, const bool with_pendant_length
) {
    // TODO outsource this comparator (and other occurences, in merge and in placement mapt set
    // and maybe more) to the tree class! maybe call it compatible() or so
    auto comparator = [] (
        PlacementTree::ConstIteratorPreorder& it_l,
        PlacementTree::ConstIteratorPreorder& it_r
    ) {
        return it_l.node()->name                      == it_r.node()->name                      &&
               it_l.node()->index()                   == it_r.node()->index()                   &&
               it_l.edge()->edge_num                  == it_r.edge()->edge_num                  &&
               it_l.edge()->primary_node()->index()   == it_r.edge()->primary_node()->index()   &&
               it_l.edge()->secondary_node()->index() == it_r.edge()->secondary_node()->index();
    };

    if (!PlacementTree::equal(map_a.tree(), map_b.tree(), comparator)) {
        LOG_WARN << "Calculating pairwise distance on different reference trees not possible.";
        return -1.0;
    }

    // Init.
    double sum = 0.0;

    // Create PqueryPlain objects for every placement and copy all interesting data into it.
    // This way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    std::vector<PqueryPlain> pqueries_a = map_a.plain_queries();
    std::vector<PqueryPlain> pqueries_b = map_b.plain_queries();

    // Calculate a matrix containing the pairwise distance between all nodes. This way, we
    // do not need to search a path between placements every time. We use the tree of the first map
    // here, ignoring branch lengths on tree b.
    // FIXME this might be made better by using average or so in the future.
    Matrix<double>* node_distances = map_a.tree().node_distance_matrix();

    for (const PqueryPlain& pqry_a : pqueries_a) {
        for (const PqueryPlain& pqry_b : pqueries_b) {
            sum += pquery_distance(pqry_a, pqry_b, *node_distances, with_pendant_length);
        }
    }

    // Return normalized value.
    return sum / map_a.placement_mass() / map_b.placement_mass();
}

// =================================================================================================
//     Variance
// =================================================================================================

/**
 * @brief Calculate the variance of the placements on a tree.
 *
 * The variance is a measure of how far a set of items is spread out in its space
 * (http://en.wikipedia.org/wiki/variance). In many cases, it can be measured using the mean of the
 * items. However, when considering placements on a tree, this does not truly measure how far they
 * are from each other. Thus, this algorithm applies a different method of calculating the variance
 * in terms of squared deviations of all items from each other:
 * \f$ Var(X) = \frac{1}{n^2} \sum_{i=1}^n \sum_{j=1}^n \frac{1}{2}(x_i - x_j)^2 \f$,
 * where \f$ (x_i - x_j) \f$ denotes the distance between two placements.
 *
 * According to the formula above, each pair of placements is evaluated twice, and subsequently
 * their distance need to be halfed when being added to the sum of distanaces. Instead of that,
 * we calculate the distance for each pair only once, thus are able skip half the calculations, and
 * of course skip the division by two.
 *
 * Furthermore, the normalizing factor \f$ \frac{1}{n^2} \f$ of the variance usually contains the
 * number of elements being processed. However, as the placements are weighted by their
 * `like_weight_ratio`, we instead calculate `n` as the sum of the `like_weight_ratio` of all
 * placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this
 * number will be equal to the number of pqueries (and thus be equal to the usual case of using the
 * number of elements). However, as this is not required (placements with small ratio can be
 * dropped, so that their sum per pquery is less than 1.0), we cannout simply use the count.
 */
double PlacementMap::variance(const bool with_pendant_length) const
{
    // Init.
    double variance = 0.0;

    // create PqueryPlain objects for every placement and copy all interesting data into it.
    // this way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    std::vector<PqueryPlain> vd_pqueries = plain_queries();

    // also, calculate a matrix containing the pairwise distance between all nodes. this way, we
    // do not need to search a path between placements every time.
    Matrix<double>* node_distances = tree_->node_distance_matrix();

#ifdef PTHREADS

    // prepare storage for thread data.
    int num_threads = Options::get().number_of_threads();
    std::vector<double>      partials(num_threads, 0.0);
    std::vector<std::thread> threads;

    // start all threads.
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(std::bind (
            &PlacementMap::variance_thread, this,
            i, num_threads, &vd_pqueries, node_distances, with_pendant_length,
            &partials[i]
        ));
        // threads[i] = new std::thread ();
    }

    // wait for all threads to finish, collect their results.
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
        variance += partials[i];
    }

#else

    // do a pairwise calculation on all placements.
    // int progress    = 0;
    for (const PqueryPlain& pqry_a : vd_pqueries) {
        // LOG_PROG(++progress, vd_pqueries.size()) << "of Variance() finished.";
        variance += variance_partial(pqry_a, vd_pqueries, *node_distances, with_pendant_length);
    }

#endif

    // Calculate normalizing factor. Should be the same value as given by placement_mass(),
    // however this calculation is probably faster.
    double mass = 0.0;
    for (const auto& pqry : vd_pqueries) {
        for (const auto& place : pqry.placements) {
            mass += place.like_weight_ratio;
        }
    }

    // cleanup, then return the normalized value.
    delete node_distances;
    return ((variance / mass) / mass);
}

/**
 * @brief Internal function that calculates the sum of distances for the variance that is
 * contributed by a subset of the pqueries. See variance() for more information.
 *
 * This function is intended to be called by variance() -- it is not a stand-alone function.
 * It takes an offset and an incrementation value and does an interleaved loop over the pqueries,
 * similar to the sequential version for calculating the variance.
 */
void PlacementMap::variance_thread (
    const int                       offset,
    const int                       incr,
    const std::vector<PqueryPlain>* pqrys,
    const Matrix<double>*           node_distances,
    const bool                      with_pendant_length,
    double*                         partial
) const {
    // Use internal variables to avoid false sharing.
    assert(*partial == 0.0);
    double tmp_partial = 0.0;

    // Iterate over the pqueries, starting at offset and interleaved with incr for each thread.
    for (size_t i = offset; i < pqrys->size(); i += incr) {
        // LOG_PROG(i, pqrys->size()) << "of Variance() finished (Thread " << offset << ").";

        const PqueryPlain& pqry_a = (*pqrys)[i];
        tmp_partial += variance_partial(pqry_a, *pqrys, *node_distances, with_pendant_length);
    }

    // Return the results.
    *partial = tmp_partial;
}

/**
 * @brief Internal function that calculates the sum of distances contributed by one pquery for
 * the variance. See variance() for more information.
 *
 * This function is intended to be called by variance() or variance_thread() -- it is not a
 * stand-alone function.
 */
double PlacementMap::variance_partial (
    const PqueryPlain&              pqry_a,
    const std::vector<PqueryPlain>& pqrys_b,
    const Matrix<double>&           node_distances,
    const bool                      with_pendant_length
) const {
    double partial = 0.0;

    for (const PqueryPlain& pqry_b : pqrys_b) {
        // Check whether it is same pquery (a=b, nothing to do, as their distance is zero),
        // or a pair of pqueries that was already calculated (a>b, skip it also).
        if (pqry_a.index >= pqry_b.index) {
            continue;
        }
        double dist = pquery_distance(pqry_a, pqry_b, node_distances, with_pendant_length);
        partial += dist * dist;
    }

    return partial;
}

// =================================================================================================
//     Dump and Debug
// =================================================================================================

/**
 * @brief Returns a list of all Pqueries with their Placements and Names.
 */
std::string PlacementMap::dump() const
{
    auto print_cell = [] (const std::string& value, size_t width = 0, char justify = 'r') {
        using namespace std;
        stringstream ss;
        ss << fixed << (justify == 'l' ? left : right);
        ss.fill(' ');
        ss.width(width);
        // ss.precision(decDigits); // set # places after decimal
        ss << value;
        return ss.str() + " ";
    };

    // TODO write a simple class for table output. or find a lib...

    std::ostringstream out;
    size_t num_len = static_cast<size_t>(ceil(log10(placement_count())));
    out << print_cell("#", num_len);
    out << print_cell("name", 60, 'l');
    out << print_cell("edge_num", 8);
    out << print_cell("likelihood", 10);
    out << print_cell("like_weight_ratio", 17);
    out << print_cell("proximal_length", 15);
    out << print_cell("pendant_length", 14);
    out << std::endl;

    size_t i = 0;
    for (const auto& pqry : pqueries_) {
        std::string name = pqry->names.size() > 0 ? pqry->names[0]->name : "";
        if (pqry->names.size() > 1) {
            name += " (+" + std::to_string(pqry->names.size() - 1) + ")";
        }

        for (const auto& p : pqry->placements) {
            out << print_cell(std::to_string(i), num_len);
            out << print_cell(name, 60, 'l');
            out << print_cell(std::to_string(p->edge_num), 8);
            out << print_cell(std::to_string(p->likelihood), 10);
            out << print_cell(std::to_string(p->like_weight_ratio), 17);
            out << print_cell(std::to_string(p->proximal_length), 15);
            out << print_cell(std::to_string(p->pendant_length), 14);
        }

        out << std::endl;
        ++i;
    }
    return out.str();
}

/**
 * @brief Returns a simple view of the Tree with information about the Pqueries on it.
 */
std::string PlacementMap::dump_tree() const
{
    auto print_line = [] (typename PlacementTree::ConstIteratorPreorder& it)
    {
        return it.node()->name + ": " + std::to_string(it.edge()->placement_count()) + " placements";
    };
    return TreeView().compact(tree(), print_line);
}

/**
 * @brief Validates the integrity of the pointers, references and data in this Placement object.
 *
 * Returns true iff everything is set up correctly. In case of inconsistencies, the function stops
 * and returns false on the first encountered error.
 *
 * If `check_values` is set to true, also a check on the validity of numerical values is done, for
 * example that the proximal_length is smaller than the corresponding branch_length.
 * If additionally `break_on_values` is set, validate() will stop on the first encountered invalid
 * value. Otherwise it will report all invalid values.
 */
bool PlacementMap::validate (bool check_values, bool break_on_values) const
{
    // check tree
    if (!tree_->validate()) {
        LOG_INFO << "Invalid placement tree.";
        return false;
    }

    // check edges
    EdgeNumMapType edge_num_map;
    size_t edge_place_count = 0;
    for (
        PlacementTree::ConstIteratorEdges it_e = tree_->begin_edges();
        it_e != tree_->end_edges();
        ++it_e
    ) {
        // make sure every edge num is used once only
        PlacementTree::EdgeType* edge = *it_e;
        if (edge_num_map.count(edge->edge_num) > 0) {
            LOG_INFO << "More than one edge has edge_num '" << edge->edge_num << "'.";
            return false;
        }
        edge_num_map.emplace(edge->edge_num, edge);

        // make sure the pointers and references are set correctly
        for (const PqueryPlacement* p : edge->placements) {
            if (p->edge != edge) {
                LOG_INFO << "Inconsistent pointer from placement to edge at edge num '"
                         << edge->edge_num << "'.";
                return false;
            }
            if (p->edge_num != edge->edge_num) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << edge->edge_num << " != " << p->edge_num << "'.";
                return false;
            }
            ++edge_place_count;
        }
    }

    // check pqueries
    size_t pqry_place_count = 0;
    for (const auto& pqry : pqueries_) {
        // use this name for reporting invalid placements.
        std::string name;
        if (pqry->names.size() > 0) {
            name = "'" + pqry->names[0]->name + "'";
        } else {
            name = "(unnamed pquery)";
        }

        // check placements
        if (check_values && pqry->placements.size() == 0) {
            LOG_INFO << "Pquery without any placements at '" << name << "'.";
            if (break_on_values) {
                return false;
            }
        }
        double ratio_sum = 0.0;
        for (const auto& p : pqry->placements) {
            // make sure the pointers and references are set correctly
            if (p->pquery != pqry.get()) {
                LOG_INFO << "Inconsistent pointer from placement to pquery at '" << name << "'.";
                return false;
            }
            int found_placement_on_edge = 0;
            for (const PqueryPlacement* pe : p->edge->placements) {
                if (pe == p.get()) {
                    ++found_placement_on_edge;
                }
            }
            if (p->edge->placements.size() > 0 && found_placement_on_edge == 0) {
                LOG_INFO << "Inconsistency between placement and edge: edge num '"
                         << p->edge->edge_num << "' does not contain pointer to a placement "
                         << "that is referring to that edge at " << name << ".";
                return false;
            }
            if (found_placement_on_edge > 1) {
                LOG_INFO << "Edge num '" << p->edge->edge_num << "' contains a pointer to one "
                         << "of its placements more than once at " << name << ".";
                return false;
            }
            if (p->edge_num != p->edge->edge_num) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << p->edge->edge_num << " != " << p->edge_num
                         << "' at " << name << ".";
                return false;
            }
            // now we know that all references between placements and edges are correct, so this
            // assertion breaks only if we forgot to check some sort of weird inconsistency.
            assert(edge_num_map.count(p->edge_num) > 0);
            ++pqry_place_count;

            // check numerical values
            if (!check_values) {
                continue;
            }
            if (p->like_weight_ratio < 0.0 || p->like_weight_ratio > 1.0) {
                LOG_INFO << "Invalid placement with like_weight_ratio '" << p->like_weight_ratio
                        << "' not in [0.0, 1.0] at " << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->pendant_length < 0.0 || p->proximal_length < 0.0) {
                LOG_INFO << "Invalid placement with pendant_length '" << p->pendant_length
                         << "' or proximal_length '" << p->proximal_length << "' < 0.0 at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->proximal_length > p->edge->branch_length) {
                LOG_INFO << "Invalid placement with proximal_length '" << p->proximal_length
                         << "' > branch_length '" << p->edge->branch_length << "' at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            ratio_sum += p->like_weight_ratio;
        }
        if (check_values && ratio_sum > 1.0) {
            LOG_INFO << "Invalid pquery with sum of like_weight_ratio '" << ratio_sum
                     << "' > 1.0 at " << name << ".";
            if (break_on_values) {
                return false;
            }
        }

        // check names
        if (check_values && pqry->names.size() == 0) {
            LOG_INFO << "Pquery without any names at '" << name << "'.";
            if (break_on_values) {
                return false;
            }
        }
        for (const auto& n : pqry->names) {
            // make sure the pointers and references are set correctly
            if (n->pquery != pqry.get()) {
                LOG_INFO << "Inconsistent pointer from name '" << n->name << "' to pquery.";
                return false;
            }
        }
    }

    if (edge_place_count != pqry_place_count) {
        LOG_INFO << "Inconsistent number of placements on edges (" << edge_place_count
                 << ") and pqueries (" << pqry_place_count << ").";
        return false;
    }

    return true;
}

} // namespace genesis
