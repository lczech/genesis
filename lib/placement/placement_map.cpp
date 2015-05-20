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
#include <thread>
#include <unordered_map>

#ifdef PTHREADS
#    include <thread>
#endif

#include "utils/logging.hpp"
#include "utils/matrix.hpp"
#include "utils/options.hpp"

namespace genesis {

// =============================================================================
//     Constructor & Destructor
// =============================================================================

/**
 * @brief Copy constructor.
 */
PlacementMap::PlacementMap (const PlacementMap& other)
{
    clear();

    // use assignment operator to create copy of the tree and metadata.
    tree     = other.tree;
    metadata = other.metadata;

    // copy all data of the tree: do a preorder traversal on both trees in parallel
    PlacementTree::IteratorPreorder it_n = tree.BeginPreorder();
    PlacementTree::ConstIteratorPreorder it_o = other.tree.BeginPreorder();
    for (
        ;
        it_n != tree.EndPreorder() && it_o != other.tree.EndPreorder();
        ++it_n, ++it_o
    ) {
        // the trees are copies of each other, they need to have the same rank. otherwise,
        // the copy constructor is not working!
        assert(it_n.Node()->Rank() == it_o.Node()->Rank());

        it_n.Edge()->placements.clear();
        it_n.Edge()->branch_length = it_o.Edge()->branch_length;
        it_n.Edge()->edge_num = it_o.Edge()->edge_num;

        it_n.Node()->name = it_o.Node()->name;
    }

    // the trees are copies. they should take equal iterations to finish a traversal.
    assert(it_n == tree.EndPreorder() && it_o == other.tree.EndPreorder());

    // copy all (o)ther pqueries to (n)ew pqueries
    EdgeNumMapType* edge_num_map = EdgeNumMap();
    for (Pquery* opqry : other.pqueries) {
        Pquery* npqry = new Pquery();
        pqueries.push_back(npqry);

        for (PqueryPlacement* op : opqry->placements) {
            PqueryPlacement* np = new PqueryPlacement(op);

            np->edge   = (*edge_num_map)[np->edge_num];
            np->edge->placements.push_back(np);
            np->pquery = npqry;
            npqry->placements.push_back(np);
        }
        for (PqueryName* on : opqry->names) {
            PqueryName* nn = new PqueryName(on);
            nn->pquery = npqry;
            npqry->names.push_back(nn);
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
    std::swap(pqueries, tmp.pqueries);
    tree.swap(tmp.tree);
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

/**
 * @brief Clears all data of this object.
 *
 * The pqueries, the tree and the metadata are deleted.
 */
void PlacementMap::clear()
{
    for (Pquery* pqry : pqueries) {
        delete pqry;
    }
    std::vector<Pquery*>().swap(pqueries);
    tree.clear();
    metadata.clear();
}

/**
 * @brief Returns a mapping of edge_num integers to the corresponding Edge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
PlacementMap::EdgeNumMapType* PlacementMap::EdgeNumMap() const
{
    EdgeNumMapType* edge_num_map = new EdgeNumMapType();
    for (
        PlacementTree::ConstIteratorEdges it = tree.BeginEdges();
        it != tree.EndEdges();
        ++it
    ) {
        PlacementTree::EdgeType* edge = *it;
        assert(edge_num_map->count(edge->edge_num) == 0);
        edge_num_map->emplace(edge->edge_num, edge);
    }
    return edge_num_map;
}

// TODO add option for averaging branch_length
// TODO write another merge function (static) that takes multiple placements and outputs a new placements object.
/**
 * @brief Adds the pqueries from another PlacementMap objects to this one.
 */
bool PlacementMap::Merge(const PlacementMap& other)
{
    // check for identical topology, taxa names and edge_nums.
    // we do not check here for branch_length, because usually those differ slightly.
    auto comparator = [] (
        PlacementTree::ConstIteratorPreorder& it_l,
        PlacementTree::ConstIteratorPreorder& it_r
    ) {
        return it_l.Node()->name     == it_r.Node()->name     &&
               it_l.Edge()->edge_num == it_r.Edge()->edge_num;
    };

    if (!tree.Equal(other.tree, comparator)) {
        LOG_WARN << "Cannot merge PlacementMap with different reference trees.";
        return false;
    }

    // we need to assign edge pointers to the correct edge objects, so we need a mapping
    EdgeNumMapType* edge_num_map = EdgeNumMap();

    // copy all (o)ther pqueries to (n)ew pqueries
    for (const Pquery* opqry : other.pqueries) {
        Pquery* npqry = new Pquery;
        for (const PqueryPlacement* op : opqry->placements) {
            PqueryPlacement* np = new PqueryPlacement(op);

            // assuming that the trees have identical topology (checked at the beginning of this
            // function), there will be an edge for every placement. if this assertion fails,
            // something broke the integrity of our in memory representation of the data.
            assert(edge_num_map->count(np->edge_num) > 0);
            np->edge = (*edge_num_map)[np->edge_num];
            np->edge->placements.push_back(np);
            np->pquery = npqry;
            npqry->placements.push_back(np);
        }
        for (const PqueryName* on : opqry->names) {
            PqueryName* nn = new PqueryName(on);
            nn->pquery = npqry;
            npqry->names.push_back(nn);
        }
        this->pqueries.push_back(npqry);
    }
    return true;
}

/**
 * @brief Recalculates the `like_weight_ratio` of the placements of each Pquery so that their sum
 * is 1.0, while maintaining their ratio to each other.
 */
void PlacementMap::NormalizeWeightRatios()
{
    for (Pquery* pqry : pqueries) {
        double sum = 0.0;
        for (PqueryPlacement* place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
        for (PqueryPlacement* place : pqry->placements) {
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
void PlacementMap::RestrainToMaxWeightPlacements()
{
    for (Pquery* pqry : pqueries) {
        // init
        double           max_w = -1.0;
        PqueryPlacement* max_p;

        for (PqueryPlacement* place : pqry->placements) {
            // find the maximum of the weight ratios in the placements of this pquery
            if (place->like_weight_ratio > max_w) {
                max_w = place->like_weight_ratio;
                max_p = place;
            }

            // delete the reference from the edge to the current placement. we will later add the
            // one that points to the remaining (max weight) placement.
            PlacementTree::EdgeType* edge = place->edge;
            std::vector<PqueryPlacement*>::iterator it = edge->placements.begin();
            for (; it != edge->placements.end(); ++it) {
                if (*it == place) {
                    break;
                }
            }

            // assert that the edge actually contains a reference to this pquery. if not,
            // this means that we messed up somewhere else while adding/removing placements...
            assert(it != edge->placements.end());
            edge->placements.erase(it);
        }

        // remove all but the max element from placements by creating a new vector and swapping it.
        // this is faster and simpler than removing all but one element.
        std::vector<PqueryPlacement*> np;
        np.push_back(max_p);
        pqry->placements.swap(np);

        // np now contains the old placements. delete them (except the one we still need).
        for (PqueryPlacement* op : np) {
            if (op != max_p) {
                delete op;
            }
        }

        // now add back the reference from the edge to the pquery.
        // assert that we now have a single placement in the pquery (the most likely one).
        assert(pqry->placements.size() == 1 && pqry->placements[0] == max_p);
        max_p->edge->placements.push_back(max_p);

        // also, set the like_weight_ratio to 1.0, because we do not have any other placements left.
        max_p->like_weight_ratio = 1.0;
    }
}

// =============================================================================
//     Placement Mass
// =============================================================================

/**
 * @brief Get the total number of placements in all pqueries.
 */
size_t PlacementMap::PlacementCount() const
{
    size_t count = 0;
    for (const Pquery* pqry : pqueries) {
        count += pqry->placements.size();
    }
    return count;
}

/**
 * @brief Get the summed mass of all placements on the tree, given by their `like_weight_ratio`.
 */
double PlacementMap::PlacementMass() const
{
    double sum = 0.0;
    for (const Pquery* pqry : pqueries) {
        for (const PqueryPlacement* place : pqry->placements) {
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
std::vector<int> PlacementMap::ClosestLeafDepthHistogram() const
{
    std::vector<int> hist;

    // get a vector telling us the depth from each node to its closest leaf node.
    PlacementTree::NodeIntVectorType depths = tree.ClosestLeafDepthVector();

    for (const Pquery* pqry : pqueries) {
        for (const PqueryPlacement* place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            int dp = depths[place->edge->PrimaryNode()->Index()].second;
            int ds = depths[place->edge->SecondaryNode()->Index()].second;
            unsigned int ld = std::min(dp, ds);

            // put the closer one into the histogram, resize if necessary.
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
 * `pendant_length` and `proximal_length` of the placements into account. If the distances is outside
 * of the interval [min,max], the counter of the first/last bin is incremented respectively.
 *
 * Example:
 *
 *     double min      =  0.0;
 *     double max      = 20.0;
 *     int    bins     = 25;
 *     double bin_size = (max - min) / bins;
 *     std::vector<int> hist = ClosestLeafDistanceHistogram (min, max, bins);
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 * %
 */
std::vector<int> PlacementMap::ClosestLeafDistanceHistogram (
    const double min, const double max, const int bins
) const {
    std::vector<int> hist(bins, 0);
    double bin_size = (max - min) / bins;

    // get a vector telling us the distance from each node to its closest leaf node.
    PlacementTree::NodeDoubleVectorType dists = tree.ClosestLeafDistanceVector();

    for (const Pquery* pqry : pqueries) {
        for (const PqueryPlacement* place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length + place->proximal_length
                      + dists[place->edge->PrimaryNode()->Index()].second;
            double ds = place->pendant_length + place->edge->branch_length - place->proximal_length
                      + dists[place->edge->SecondaryNode()->Index()].second;
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
 * @brief Returns the same type of histogram as ClosestLeafDistanceHistogram(), but automatically
 * determines the needed boundaries.
 *
 * See ClosestLeafDistanceHistogram() for general information about what this function does. The
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
 *     std::vector<int> hist = ClosestLeafDistanceHistogram (min, max, bins);
 *     double bin_size = (max - min) / bins;
 *     LOG_INFO << "Histogram boundaries: [" << min << "," << max << ").";
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; " << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 *
 * It has a slightly higher time and memory consumption than the non-automatic version
 * ClosestLeafDistanceHistogram(), as it needs to process the values twice in order to find their
 * min and max.
 */
std::vector<int> PlacementMap::ClosestLeafDistanceHistogramAuto (
    double& min, double& max, const int bins
) const {
    std::vector<int> hist(bins, 0);

    // we do not know yet where the boundaries of the histogram lie, so we need to store all values
    // first and find their min and max.
    std::vector<double> distrib;
    double min_d = 0.0;
    double max_d = 0.0;

    // get a vector telling us the distance from each node to its closest leaf node.
    PlacementTree::NodeDoubleVectorType dists = tree.ClosestLeafDistanceVector();

    // calculate all distances from placements to their closest leaf and store them.
    for (const Pquery* pqry : pqueries) {
        for (const PqueryPlacement* place : pqry->placements) {
            // try both nodes at the end of the placement's edge and see which one is closer
            // to a leaf.
            double dp = place->pendant_length + place->proximal_length
                      + dists[place->edge->PrimaryNode()->Index()].second;
            double ds = place->pendant_length + place->edge->branch_length - place->proximal_length
                      + dists[place->edge->SecondaryNode()->Index()].second;
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
        assert(bin >=0 && bin < bins);
        ++hist[bin];
    }

    // report the min and max values to the calling function and return the histogram.
    min = min_d;
    max = max_d;
    return hist;
}

/**
 * @brief Calculates the Earth Movers Distance to another sets of placements on a fixed reference
 * tree.
 */
double PlacementMap::EMD(const PlacementMap& right, const bool with_pendant_length) const
{
    return PlacementMap::EMD(*this, right, with_pendant_length);
}

/**
 * @brief Calculates the Earth Movers Distance between two sets of placements on a fixed reference
 * tree.
 */
double PlacementMap::EMD(const PlacementMap& lhs, const PlacementMap& rhs, const bool with_pendant_length)
{
    // keep track of the total resulting distance.
    double distance = 0.0;

    // store a per-node balance of mass. each element in this map contains information about how
    // much placement mass is pushing from the direction of this node towards the root.
    // caveat: the masses that are stored here are already fully pushed towards the root, but are
    // stored here using the node at the lower end of the branch as key.
    std::unordered_map<const PlacementTree::NodeType*, double> balance;

    // use the sum of masses as normalization factor for the masses.
    double totalmass_l = lhs.PlacementMass();
    double totalmass_r = rhs.PlacementMass();

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
    PlacementTree::ConstIteratorPostorder it_l = lhs.tree.BeginPostorder();
    PlacementTree::ConstIteratorPostorder it_r = rhs.tree.BeginPostorder();
    for (
        ;
        it_l != lhs.tree.EndPostorder() && it_r != rhs.tree.EndPostorder();
        ++it_l, ++it_r
    ) {
        LOG_DBG << "\033[1;31miteration at node " << it_l.Node()->index_ << ": " << it_l.Node()->name << "\033[0m";
        LOG_DBG << "current distance " << distance;

        // check whether both trees have identical topology. if they have, the ranks of all nodes
        // are the same. however, if not, at some point their ranks will differ.
        if (it_l.Node()->Rank() != it_r.Node()->Rank()) {
            LOG_WARN << "Calculating EMD on different reference trees not possible.";
            return -1.0;
        }

        // if we are at the last iteration, we reached the root, thus we have moved all masses now
        // and don't need to proceed. if we did, we would count an edge of the root again.
        if (it_l.IsLastIteration()) {
            LOG_DBG1 << "last iteration";
            // we do a check for the mass at the root here for debug purposes.
            double root_mass = 0.0;
            for (
                PlacementTree::NodeType::ConstIteratorLinks n_it = it_l.Node()->BeginLinks();
                n_it != it_l.Node()->EndLinks();
                ++n_it
            ) {
                assert(balance.count(n_it.Link()->Outer()->Node()));
                root_mass += balance[n_it.Link()->Outer()->Node()];
            }

            LOG_DBG << "Mass at root: " << root_mass;

            continue;
        }

        // check whether the data on both reference trees is the same. this has to be done after the
        // check for last iteration / root node, because we don't want to check this for the root.
        if (it_l.Node()->name     != it_r.Node()->name ||
            it_l.Edge()->edge_num != it_r.Edge()->edge_num
        ) {
            LOG_WARN << "Inconsistent reference trees in EMD calculation.";
            return -1.0;
        }

        // we now start a "normal" EMD caluclation on the current edge. for this, we store the
        // masses of all placements sorted by their position on the branch.
        std::multimap<double, double> edge_balance;
        LOG_DBG1 << "placing on branch...";

        // add all placements of the branch from the left tree (using positive mass)...
        for (PqueryPlacement* place : it_l.Edge()->placements) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio * place->pendant_length / totalmass_l;
            }
            edge_balance.emplace(place->proximal_length, +place->like_weight_ratio / totalmass_l);

            LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_l edge   " << it_l.Edge()->index_;
            LOG_DBG2 << "added dist  " << place->like_weight_ratio * place->pendant_length / totalmass_l;
            LOG_DBG2 << "new dist    " << distance;
            LOG_DBG2 << "emplaced at " << place->proximal_length << ": " << +place->like_weight_ratio / totalmass_l;
            LOG_DBG2;
        }

        // ... and the branch from the right tree (using negative mass)
        for (PqueryPlacement* place : it_r.Edge()->placements) {
            if (with_pendant_length) {
                distance += place->like_weight_ratio * place->pendant_length / totalmass_r;
            }
            edge_balance.emplace(place->proximal_length, -place->like_weight_ratio / totalmass_r);

            LOG_DBG2 << "placement   " << place->pquery->names[0]->name;
            LOG_DBG2 << "it_r edge   " << it_r.Edge()->index_;
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
        PlacementTree::LinkType* link = it_l.Link()->Next();
        while (link != it_l.Link()) {
            // we do postorder traversal, so we have seen the child nodes of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link->Outer()->Node()));

            mass_s += balance[link->Outer()->Node()];
            link = link->Next();
        }
        LOG_DBG1 << "subtrees mass_s " << mass_s;
        LOG_DBG1 << "entering standard emd part...";

        // start the EMD with the mass that is left over from the subtrees...
        double cur_pos  = it_l.Edge()->branch_length;
        double cur_mass = mass_s;

        LOG_DBG1 << "cur_pos  " << cur_pos;
        LOG_DBG1 << "cur_mass " << cur_mass;

        // ... and move it along the branch, balancing it with the placements found on the branches.
        // this is basically a standard EMD calculation along the branch.
        std::multimap<double, double>::reverse_iterator rit;
        for (rit = edge_balance.rbegin(); rit != edge_balance.rend(); ++rit) {
            LOG_DBG2 << "at " << rit->first << " with " << rit->second;
            assert(cur_pos >= rit->first);
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
        balance[it_l.Node()] = cur_mass;

        LOG_DBG1 << "added dist " << std::abs(cur_mass) * cur_pos;
        LOG_DBG1 << "new dist   " << distance;
        LOG_DBG1 << "balance at node " << it_l.Node()->index_ << ": " << it_l.Node()->name << " = " << cur_mass;
        LOG_DBG1 << "finished standard emd part";
        LOG_DBG1;
    }

    // check whether we are done with both trees.
    if (it_l != lhs.tree.EndPostorder() || it_r != rhs.tree.EndPostorder()) {
        LOG_WARN << "Inconsistent reference trees in EMD calculation.";
        return -1.0;
    }

    LOG_DBG << "final distance: " << distance;
    Logging::max_level(ll);

    return distance;
}

/**
 * @brief Calculate the Center of Gravity of the placements on a tree.
 */
void PlacementMap::COG() const
{
    // store a balance of mass per link, so that each element contains the mass that lies
    // in the direction of this link
    std::unordered_map<PlacementTree::LinkType*, double> balance;

    // do a postorder traversal
    for (
        PlacementTree::ConstIteratorPostorder it = this->tree.BeginPostorder();
        it != this->tree.EndPostorder();
        ++it
    ) {
        // node does not have a corresponding edge (eg the root)
        if (!it.Edge()) {
            continue;
        }

        double mass = 0.0;

        // add up the masses from children
        PlacementTree::LinkType* link = it.Link()->Next();
        while (link != it.Link()) {
            // we do postorder traversal, so we have seen the child links of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link));

            mass += balance[link] * it.Edge()->branch_length;
            link  = link->Next();
        }

        // add up the masses of placements on the current branch
        for (PqueryPlacement* place : it.Edge()->placements) {
            mass += place->pendant_length + place->proximal_length;
        }

        assert(balance.count(it.Link()->Outer()) == 0);
        balance[it.Link()->Outer()] = mass;
    }

    PlacementTree::LinkType* p_prev = tree.RootLink();
    PlacementTree::LinkType* p_link = tree.RootLink();
    double                   p_mass = -1.0;
    bool                     found  = false;
    while (!found) {
        LOG_DBG1 << "a " << p_link->Node()->name;
        p_mass = -1.0;
        for (
            PlacementTree::NodeType::IteratorLinks it_l = p_link->Node()->BeginLinks();
            it_l != p_link->Node()->EndLinks();
            ++it_l
        ) {
            LOG_DBG2 << it_l.Node()->name << " " << balance[it_l.Link()];
            if (balance[it_l.Link()] > p_mass) {
                p_link = it_l.Link();
            }
        }
        LOG_DBG1 << "b " << p_link->Node()->name;
        p_link = p_link->Outer();
        if (p_link == p_prev) {
            found = true;
            break;
        }
        p_prev = p_link;
    }

    //~ PlacementTree::LinkType* link = tree.RootLink();
    //~ while (link != it.Link()) {
        //~ // we do postorder traversal, so we have seen the child links of the current node,
        //~ // which means, they should be in the balance list already.
        //~ assert(balance.count(link));
//~
        //~ mass += balance[link] * it.Edge()->branch_length;
        //~ link = link->Next();
    //~ }

    for (auto pair : balance) {
        LOG_DBG1 << pair.first->Node()->name << ": " << pair.second << "\n";
        //~ distance += std::abs(pair.second);
    }
}

// =============================================================================
//     Variance
// =============================================================================

/**
 * @brief Calculate the Variance of the placements on a tree.
 *
 * The variance is a measure of how far a set of items is spread out
 * (http://en.wikipedia.org/wiki/Variance). In many cases, it can be measured using the mean of the
 * items. However, when considering placements on a tree, this does not truly measure how far they
 * are from each other. Thus, this algorithm applies a different method of calculating the variance
 * in terms of squared deviations of all items from each other:
 * \f$ Var(X) = \frac{1}{n^2} \sum_{i=1}^n \sum_{j=1}^n \frac{1}{2}(x_i - x_j)^2 \f$,
 * where \f$ (x_i - x_j) \f$ denotes the distance between two placements.
 *
 * This distance is calculated as the shortest path between the two placements. This includes the
 * pendant_length of both, as well as their position on the branches. There are three cases that
 * might occur:
 *
 *   1. Both placements are on the same branch.
 *      In this case, their distance is caluclated as the sum of their pendant_lengths and their
 *      difference in proximal_lengths.
 *
 *   2. The path between the placements includes the root.
 *      The distance of a placement from its neighbouring nodes is mostly given in form of the
 *      proximal_length, which is the distance of the placement to the node (at the end of its
 *      branch) that lies in direction of the root. Thus, there is an implicit notion of a root,
 *      that we need to consider. If the path between two placements contains the root, we can
 *      directly calculate their distance as the distance between the two promixal nodes plus
 *      proximal_lengths and pendant_lengths of both placements. We call this the promixal-promixal
 *      case.
 *
 *   3. The root is not part of the path between the placements.
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the proximal_lengths
 *      directly, but we need to get the distal_length (away from the root) of the inner placement
 *      first. This is simply the difference between branch_length and proximal_length of that
 *      placement. Of course, this case comes in two flavours, because both placements can be the
 *      inner or outer one. They are called proximal-distal case and distal-proximal case,
 *      respectively.
 *
 * The first case is easy to detect by comparing the edges. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of the
 * algorithm and contains the pairwise distances between all nodes of the tree. Using this, we do
 * not need to find paths between placements, but simply go to the nodes at the end of the branches
 * of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance
 * is then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the variance.
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
 * dropped, so that their sum per pquery is less than 1.0), we need to calculate this number
 * manually here.
 */
double PlacementMap::Variance() const
{
    // init
    double variance = 0.0;
    double count    = 0.0;

    // create a VarianceData object for every placement and copy all interesting data into it.
    // this way, we won't have to do all the pointer dereferencing during the actual calculations,
    // and furthermore, the data is close in memory. this gives a tremendous speedup!
    size_t index = 0;
    std::vector<VarianceData> vd_placements;
    vd_placements.reserve(PlacementCount());
    for (const Pquery* pqry : this->pqueries) {
        for (const PqueryPlacement* place : pqry->placements) {
            VarianceData vdp;
            vdp.index                = index++;
            vdp.edge_index           = place->edge->Index();
            vdp.primary_node_index   = place->edge->PrimaryNode()->Index();
            vdp.secondary_node_index = place->edge->SecondaryNode()->Index();
            vdp.pendant_length       = place->pendant_length;
            vdp.proximal_length      = place->proximal_length;
            vdp.branch_length        = place->edge->branch_length;
            vdp.like_weight_ratio    = place->like_weight_ratio;
            vd_placements.push_back(vdp);
        }
    }

    // also, calculate a matrix containing the pairwise distance between all nodes. this way, we
    // do not need to search a path between placements every time.
    Matrix<double>* node_distances = tree.NodeDistanceMatrix();

#ifdef PTHREADS

    // prepare storage for thread data.
    int num_threads = Options::number_of_threads;
    std::vector<double>       partials(num_threads, 0.0);
    std::vector<double>       counts  (num_threads, 0.0);
    std::vector<std::thread*> threads (num_threads, nullptr);

    // start all threads.
    for (int i = 0; i < num_threads; ++i) {
        threads[i] = new std::thread (std::bind (
            &PlacementMap::VarianceThread, this,
            i, num_threads, &vd_placements, node_distances, &partials[i], &counts[i]
        ));
    }

    // wait for all threads to finish, collect their results.
    for (int i = 0; i < num_threads; ++i) {
        threads[i]->join();
        variance += partials[i];
        count    += counts[i];
    }

#else

    // do a pairwise calculation on all placements.
    int progress    = 0;
    for (const VarianceData& place_a : vd_placements) {
        LOG_PROG(++progress, vd_placements.size()) << "of Variance() finished.";
        variance += VariancePartial(place_a, vd_placements, *node_distances);
        count    += place_a.like_weight_ratio;
    }

#endif

    // cleanup, then return the normalized value.
    delete node_distances;
    return ((variance / count) / count);
}

/**
 * @brief Internal function that calculates the sum of distances for the variance that is
 * contributed by a subset of the placements. See Variance() for more information.
 *
 * This function is intended to be called by Variance() -- it is not a stand-alone function.
 * It takes an offset and an incrementation value and does an interleaved loop over the placements,
 * similar to the sequential version for calculating the variance.
 */
void PlacementMap::VarianceThread (
    const int                        offset,
    const int                        incr,
    const std::vector<VarianceData>* pqrys,
    const Matrix<double>*            node_distances,
    double*                          partial,
    double*                          count
) const {
    // use internal variables to avoid false sharing.
    assert(*partial == 0.0 && *count == 0.0);
    double tmp_partial = 0.0;
    double tmp_count   = 0.0;

    // iterate over the pqueries, starting at offset and interleaved with incr for each thread.
    for (size_t i = offset; i < pqrys->size(); i += incr) {
        LOG_PROG(i, pqrys->size()) << "of Variance() finished (Thread " << offset << ").";

        const VarianceData& place_a = (*pqrys)[i];
        tmp_partial += VariancePartial(place_a, *pqrys, *node_distances);
        tmp_count   += place_a.like_weight_ratio;
    }

    // return the results.
    *partial = tmp_partial;
    *count   = tmp_count;
}

/**
 * @brief Internal function that calculates the sum of distances contributed by one placement for
 * the variance. See Variance() for more information.
 *
 * This function is intended to be called by Variance() or VarianceThread() -- it is not a
 * stand-alone function.
 */
double PlacementMap::VariancePartial (
    const VarianceData&              place_a,
    const std::vector<VarianceData>& pqrys_b,
    const Matrix<double>&            node_distances
) const {
    double sum = 0.0;
    double dd, pd, dp, min;

    for (const VarianceData& place_b : pqrys_b) {
        // check whether it is same placement (a=b, nothing to do, as their distance is zero),
        // or a pair of placements that was already calculated (skip it also).
        if (place_a.index >= place_b.index) {
            continue;
        }

        // same branch case
        if (place_a.edge_index == place_b.edge_index) {
            sum += place_a.pendant_length
                +  std::abs(place_a.proximal_length - place_b.proximal_length)
                +  place_b.pendant_length;
            continue;
        }

        // proximal-proximal case
        dd = place_a.pendant_length + place_a.proximal_length
           + node_distances(place_a.primary_node_index, place_b.primary_node_index)
           + place_b.proximal_length + place_b.pendant_length;

        // proximal-distal case
        pd = place_a.pendant_length + place_a.branch_length - place_a.proximal_length
           + node_distances(place_a.secondary_node_index, place_b.primary_node_index)
           + place_b.proximal_length + place_b.pendant_length;

        // distal-proximal case
        dp = place_a.pendant_length + place_a.proximal_length
           + node_distances(place_a.primary_node_index, place_b.secondary_node_index)
           + place_b.branch_length - place_b.proximal_length + place_b.pendant_length;

        // find min of the three cases and normalize it to the weight ratios.
        min  = std::min(dd, std::min(pd, dp));
        min *= place_a.like_weight_ratio * place_b.like_weight_ratio;
        sum += min * min;
    }

    return sum;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Returns a list of all Pqueries with their Placements and Names.
 */
std::string PlacementMap::Dump() const
{
    std::ostringstream out;
    for (const Pquery* pqry : pqueries) {
        for (const PqueryName* n : pqry->names) {
            out << "Placement: \"" << n->name << "\"";
            if (n->multiplicity != 0.0) {
                out << " (" << n->multiplicity << ")";
            }
            out << "\n";
        }
        for (const PqueryPlacement* p : pqry->placements) {
            out << "at Edge num: " << p->edge_num << " (edge index " << p->edge->index_ << "). ";
            if (p->likelihood != 0.0 || p->like_weight_ratio != 0.0) {
                out << "\tLikelihood: " << p->likelihood;
                out << ", Ratio: " << p->like_weight_ratio << " ";
            }
            if (p->parsimony != 0.0) {
                out << "\tParsimony: " << p->parsimony << " ";
            }
            out << "\tProximal Length: " << p->proximal_length;
            out << ", Pendant Length: " << p->pendant_length << "\n";
        }
        out << "\n";
    }
    return out.str();
}

/**
 * @brief Validates the integrity of the pointers, references and data in this Placement object.
 *
 * Returns true iff everything is set up correctly. In case of inconsistencies, the function stops
 * and returns false on the first encountered error.
 *
 * If `check_values` is set to true, also a check on the validity of numerical values is done, for
 * example that the proximal_length is smaller than the corresponding branch_length.
 * If additionally `break_on_values` is set, Validate() will stop on the first encountered invalid
 * value. Otherwise it will report all invalid values.
 */
bool PlacementMap::Validate (bool check_values, bool break_on_values) const
{
    // check tree
    if (!tree.Validate()) {
        LOG_INFO << "Invalid placement tree.";
        return false;
    }

    // check edges
    EdgeNumMapType edge_num_map;
    size_t edge_place_count = 0;
    for (
        PlacementTree::ConstIteratorEdges it_e = tree.BeginEdges();
        it_e != tree.EndEdges();
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
    for (const Pquery* pqry : pqueries) {
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
        for (const PqueryPlacement* p : pqry->placements) {
            // make sure the pointers and references are set correctly
            if (p->pquery != pqry) {
                LOG_INFO << "Inconsistent pointer from placement to pquery at '" << name << "'.";
                return false;
            }
            int found_placement_on_edge = 0;
            for (const PqueryPlacement* pe : p->edge->placements) {
                if (pe == p) {
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
        for (const PqueryName* n : pqry->names) {
            // make sure the pointers and references are set correctly
            if (n->pquery != pqry) {
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
