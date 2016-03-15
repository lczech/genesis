/**
 * @brief Implementation of Sample class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <unordered_map>
#include <utility>

#include "tree/default/distances.hpp"
#include "tree/distances.hpp"
#include "tree/operators.hpp"
#include "tree/tree_view.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Constructor & Destructor
// =================================================================================================

/**
 * @brief Copy constructor.
 */
Sample::Sample( Sample const& other )
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

        // TODO once the edge and node data are copyable and tree supports full data copies, these are not needed any longer.
        it_n.edge()->data.placements.clear();
        it_n.edge()->data.branch_length = it_o.edge()->data.branch_length;
        it_n.edge()->data.edge_num = it_o.edge()->data.edge_num;

        it_n.node()->data.name = it_o.node()->data.name;
    }

    // the trees are copies. they should take equal iterations to finish a traversal.
    assert(it_n == tree_->end_preorder() && it_o == other.tree_->end_preorder());

    // copy all (o)ther pqueries to (n)ew pqueries
    // TODO this is very similar to merge(). make this code shared somehow.
    auto en_map = edge_num_map();
    for (const auto& opqry : other.pqueries_) {
        auto npqry = make_unique<Pquery>();

        for (const auto& op : opqry->placements) {
            auto np = make_unique<PqueryPlacement>(*op);

            np->edge   = en_map[np->edge_num];
            np->edge->data.placements.push_back(np.get());
            np->pquery = npqry.get();
            npqry->placements.push_back(std::move(np));
        }
        for (const auto& on : opqry->names) {
            auto nn = make_unique<PqueryName>(*on);

            nn->pquery = npqry.get();
            npqry->names.push_back(std::move(nn));
        }

        pqueries_.push_back(std::move(npqry));
    }
}

/**
 * @brief Move constructor.
 */
// Sample::Sample( Sample&& other ) noexcept
// {
//     swap(other);
// }

Sample::Sample( Sample&& other ) noexcept
    : pqueries_( std::move(other.pqueries_))
    , tree_(     std::move(other.tree_))
    , metadata(  std::move(other.metadata))
{
    other.pqueries_.clear();
    if( other.tree_ ) {
        other.tree_->clear();
    }
    other.metadata.clear();
}

/**
 * @brief Assignment operator. See Copy constructor for details.
 */
Sample& Sample::operator = (const Sample& other)
{
    // check for self-assignment.
    if (&other == this) {
        return *this;
    }

    // the Sample tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    Sample tmp(other);
    std::swap(pqueries_, tmp.pqueries_);
    tree_->swap(*tmp.tree_);
    std::swap(metadata, tmp.metadata);
    return *this;
}

Sample& Sample::operator= (Sample&& other) noexcept
{
    swap(other);
    return *this;
}

Sample::~Sample()
{
    // We are going to destroy the Sample. Let's speed it up!
    // TODO make swapping work propery. then, this should work, too
    // (currently, it gives a segfault if a moved-from Sample is destroyed).
    // if( tree_ ) {
    //     detach_pqueries_from_tree();
    // }

    // Instead, use a weird half-copied solution for now:

    for (auto const& pqry : pqueries_) {
        for( auto const& place : pqry->placements ) {
            place->edge = nullptr;
        }
    }
}

void Sample::swap (Sample& other) noexcept
{
    using std::swap;
    swap(pqueries_, other.pqueries_);
    swap(tree_,     other.tree_);
    swap(metadata,  other.metadata);
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Creats an empty Pquery, adds it to the Sample and returns a pointer to it.
 *
 * The returned pointer can then be used to add Placements and Names to the Pquery.
 */
Pquery* Sample::add_pquery()
{
    pqueries_.push_back(make_unique<Pquery>());
    return pqueries_.back().get();
}

/**
 * @brief Adds the pqueries from another Sample objects to this one.
 *
 * For this method to succeed, the Samples need to have the same topology, including identical
 * edge_nums and node names.
 *
 * The resulting tree is the original one of the Sample on which this method was called. If
 * instead the average branch length tree is needed, see SampleSet::merge_all().
 */
bool Sample::merge(const Sample& other)
{
    // Check for identical topology, taxa names and edge_nums.
    // We do not check here for branch_length, because usually those differ slightly.
    auto node_comparator = [] (
        const PlacementTree::NodeType& node_l,
        const PlacementTree::NodeType& node_r
    ) {
        return node_l.data.name == node_r.data.name;
    };

    auto edge_comparator = [] (
        const PlacementTree::EdgeType& edge_l,
        const PlacementTree::EdgeType& edge_r
    ) {
        return edge_l.data.edge_num == edge_r.data.edge_num;
    };

    if( ! tree::equal<PlacementTree, PlacementTree >(
        *tree_, *other.tree_, node_comparator, edge_comparator
    )) {
        LOG_WARN << "Cannot merge Samples with different reference trees.";
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
            np->edge->data.placements.push_back(np.get());
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
void Sample::clear()
{
    pqueries_.clear();
    tree_ = std::make_shared<PlacementTree>();
    metadata.clear();
}

/**
 * @brief Clears all placements of this Sample.
 *
 * All pqueries are deleted. However, the Tree and metadata are left as they are, thus this is a
 * useful method for simulating placements: Take a copy of a given map, clear its placements, then
 * generate new ones using PlacementSimulator.
 */
void Sample::clear_placements()
{
    for (auto it = tree_->begin_edges(); it != tree_->end_edges(); ++it) {
        (*it)->data.placements.clear();
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
Sample::EdgeNumMapType Sample::edge_num_map() const
{
    auto en_map = EdgeNumMapType();
    for (
        PlacementTree::ConstIteratorEdges it = tree_->begin_edges();
        it != tree_->end_edges();
        ++it
    ) {
        const auto& edge = *it;
        assert(en_map.count(edge->data.edge_num) == 0);
        en_map.emplace(edge->data.edge_num, edge.get());
    }
    return en_map;
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
std::vector<PqueryPlain> Sample::plain_queries() const
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

            place.branch_length        = oplace->edge->data.branch_length;
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
void Sample::normalize_weight_ratios()
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
void Sample::restrain_to_max_weight_placements()
{
    detach_pqueries_from_tree();

    for (auto& pqry : pqueries_) {
        // Initialization.
        double             max_w = -1.0;
        PqueryPlacement*   max_p = nullptr;
        // PlacementTreeEdge* max_e = nullptr;

        for (auto& place : pqry->placements) {
            // Find the maximum of the weight ratios in the placements of this pquery.
            if (place->like_weight_ratio > max_w) {
                max_w = place->like_weight_ratio;
                max_p = place.get();
                // max_e = place->edge;
            }

            // place->edge = nullptr;

            // Delete the reference from the edge to the current placement. We will later add the
            // one that points to the remaining (max weight) placement back to its edge.
            // std::vector<PqueryPlacement*>::iterator it = place->edge->data.placements.begin();
            // for (; it != place->edge->data.placements.end(); ++it) {
            //     if (*it == place.get()) {
            //         break;
            //     }
            // }

            // Assert that the edge actually contains a reference to this pquery. If not,
            // this means that we messed up somewhere else while adding/removing placements...
            // assert(it != place->edge->data.placements.end());
            // place->edge->data.placements.erase(it);
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
        // max_p->edge = max_e;
        // max_p->edge->data.placements.push_back(max_p);

        // Also, set the like_weight_ratio to 1.0, because we do not have any other placements left.
        max_p->like_weight_ratio = 1.0;
    }

    reattach_pqueries_to_tree();
}

/**
 * @brief Delete all connecting pointers between the Pquery Placements and their edges on the tree.
 *
 * Each Placement has a pointer to its edge, and each edge has a vector of pointers to all
 * Placements that point to that edge.
 *
 * By default, each placement deletes these connections when being destroyed. This process includes
 * a linear search through the vector of the edge. If many Placements need to be deleted at the
 * same time, this is a bottleneck in speed, as it results in quadratic time complexity in the
 * number of placements.
 *
 * Thus, for operations that do a lot of Placement deletions (see restrain_to_max_weight_placements
 * or collect_duplicate_pqueries as examples), it is cheaper to first delete all pointers using
 * this method, and later restore the remaining ones using reattach_pqueries_to_tree.
 *
 * Caveat: While the pqueries are detached, the edge pointers of the placements and the placement
 * vector of the edges are empty and shall not be used.
 */
void Sample::detach_pqueries_from_tree()
{
    for( auto it = tree().begin_edges(); it != tree().end_edges(); ++it ) {
        (*it)->data.placements.clear();
    }
    for (auto const& pqry : pqueries_) {
        for( auto const& place : pqry->placements ) {
            assert( place->edge != nullptr );
            place->edge = nullptr;
        }
    }
}

/**
 * @brief Restore all connecting pointers between the Pquery Placements and their edges on the tree.
 *
 * See detach_pqueries_from_tree for more information.
 */
void Sample::reattach_pqueries_to_tree()
{
    auto enm = edge_num_map();
    for (auto const& pqry : pqueries_) {
        for( auto const& place : pqry->placements ) {
            assert( enm.count(place->edge_num) == 1 );
            assert( place->edge == nullptr );
            enm[place->edge_num]->data.placements.push_back(place.get());
            place->edge = enm[place->edge_num];
        }
    }
}

// =================================================================================================
//     Placement Mass
// =================================================================================================

/**
 * @brief Get the total number of placements in all pqueries.
 */
size_t Sample::placement_count() const
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
double Sample::placement_mass() const
{
    double sum = 0.0;
    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
    }
    return sum;
}

// TODO outsource the histogram methods to measures, but introduce a histogram class to utils or math before!

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
std::vector<int> Sample::closest_leaf_depth_histogram() const
{
    std::vector<int> hist;

    // Get a vector telling us the depth from each node to its closest leaf node.
    auto depths = closest_leaf_depth_vector(tree());

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
std::vector<int> Sample::closest_leaf_distance_histogram (
    const double min, const double max, const int bins
) const {
    std::vector<int> hist(bins, 0);
    double bin_size = (max - min) / bins;

    // get a vector telling us the distance from each node to its closest leaf node.
    auto dists = closest_leaf_distance_vector(tree());

    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length
                      + place->proximal_length
                      + dists[place->edge->primary_node()->index()].second;
            double ds = place->pendant_length
                      + place->edge->data.branch_length
                      - place->proximal_length
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
std::vector<int> Sample::closest_leaf_distance_histogram_auto (
    double& min, double& max, const int bins
) const {
    std::vector<int> hist(bins, 0);

    // we do not know yet where the boundaries of the histogram lie, so we need to store all values
    // first and find their min and max.
    std::vector<double> distrib;
    double min_d = 0.0;
    double max_d = 0.0;

    // get a vector telling us the distance from each node to its closest leaf node.
    auto dists = closest_leaf_distance_vector(tree());

    // calculate all distances from placements to their closest leaf and store them.
    for (const auto& pqry : pqueries_) {
        for (const auto& place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length
                      + place->proximal_length
                      + dists[place->edge->primary_node()->index()].second;
            double ds = place->pendant_length
                      + place->edge->data.branch_length
                      - place->proximal_length
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
//     Dump and Debug
// =================================================================================================

/**
 * @brief Returns a simple view of the Tree with information about the Pqueries on it.
 */
std::string Sample::dump_tree() const
{
    auto print_line = [] (typename PlacementTree::ConstIteratorPreorder& it)
    {
        return it.node()->data.name + " [" + std::to_string(it.edge()->data.edge_num) + "]" ": "
            + std::to_string(it.edge()->data.placement_count()) + " placements";
    };
    return tree::TreeView().compact< PlacementTree >( tree(), print_line );
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
bool Sample::validate (bool check_values, bool break_on_values) const
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
        PlacementTree::EdgeType* edge = (*it_e).get();
        if (edge_num_map.count(edge->data.edge_num) > 0) {
            LOG_INFO << "More than one edge has edge_num '" << edge->data.edge_num << "'.";
            return false;
        }
        edge_num_map.emplace(edge->data.edge_num, edge);

        // make sure the pointers and references are set correctly
        for (const PqueryPlacement* p : edge->data.placements) {
            if (p->edge != edge) {
                LOG_INFO << "Inconsistent pointer from placement to edge at edge num '"
                         << edge->data.edge_num << "'.";
                return false;
            }
            if (p->edge_num != edge->data.edge_num) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << edge->data.edge_num << " != " << p->edge_num << "'.";
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
            for (const PqueryPlacement* pe : p->edge->data.placements) {
                if (pe == p.get()) {
                    ++found_placement_on_edge;
                }
            }
            if (p->edge->data.placements.size() > 0 && found_placement_on_edge == 0) {
                LOG_INFO << "Inconsistency between placement and edge: edge num '"
                         << p->edge->data.edge_num << "' does not contain pointer to a placement "
                         << "that is referring to that edge at " << name << ".";
                return false;
            }
            if (found_placement_on_edge > 1) {
                LOG_INFO << "Edge num '" << p->edge->data.edge_num << "' contains a pointer to one "
                         << "of its placements more than once at " << name << ".";
                return false;
            }
            if (p->edge_num != p->edge->data.edge_num) {
                LOG_INFO << "Inconsistent edge_num between edge and placement: '"
                         << p->edge->data.edge_num << " != " << p->edge_num
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
            if (p->proximal_length > p->edge->data.branch_length) {
                LOG_INFO << "Invalid placement with proximal_length '" << p->proximal_length
                         << "' > branch_length '" << p->edge->data.branch_length << "' at "
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

} // namespace placement
} // namespace genesis
