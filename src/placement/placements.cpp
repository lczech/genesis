/**
 * @brief Implementation of Placements class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placements.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>

#include "utils/logging.hpp"
#include "utils/math.hpp"

namespace genesis {

// =============================================================================
//     Constructor & Destructor
// =============================================================================

/**
 * @brief Copy constructor.
 */
Placements::Placements (const Placements& other)
{
    clear();

    // use assignment operator to create copy of the tree and metadata.
    tree     = other.tree;
    metadata = other.metadata;

    // copy all data of the tree: do a preorder traversal on both trees in parallel
    PlacementTree::IteratorPreorder it_n = tree.BeginPreorder();
    PlacementTree::IteratorPreorder it_o = other.tree.BeginPreorder();
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
Placements& Placements::operator = (const Placements& other)
{
    // check for self-assignment.
    if (&other == this) {
        return *this;
    }

    // the Placements tmp is a copy of the right hand side object (automatically created using the
    // copy constructor). we can thus simply swap the arrays, and upon leaving the function,
    // tmp is automatically destroyed, so that its arrays are cleared and the data freed.
    Placements tmp(other);
    std::swap(pqueries, tmp.pqueries);
    tree.swap(tmp.tree);
    std::swap(metadata, tmp.metadata);
    return *this;
}

/**
 * @brief Destructor. Calls clear() to delete all data.
 */
Placements::~Placements()
{
    clear();
}

/**
 * @brief Clears all data of this object.
 *
 * The pqueries, the tree and the metadata are deleted.
 */
void Placements::clear()
{
    for (Pquery* pqry : pqueries) {
        delete pqry;
    }
    std::deque<Pquery*>().swap(pqueries);
    tree.clear();
    metadata.clear();
}

/**
 * @brief Returns a mapping of edge_num integers to the corresponding Edge object.
 *
 * This function depends on the tree only and does not involve any pqueries.
 */
Placements::EdgeNumMapType* Placements::EdgeNumMap() const
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

/**
 * @brief Adds the pqueries from another Placements objects to this one.
 */
bool Placements::Merge(Placements& other)
{
    // check for identical topology, taxa names and edge_nums.
    // we do not check here for branch_length, because usuallym those differ slightly.
    auto comparator = [] (PlacementTree::IteratorPreorder& it_l, PlacementTree::IteratorPreorder& it_r)
    {
        return it_l.Node()->name     == it_r.Node()->name     &&
               it_l.Edge()->edge_num == it_r.Edge()->edge_num;
    };
    if (!tree.Equal(other.tree, comparator)) {
        LOG_WARN << "Cannot merge Placements with different reference trees.";
        return false;
    }

    // we need to assign edge pointers to the correct edge objects, so we need a mapping
    EdgeNumMapType* edge_num_map = EdgeNumMap();

    // copy all (o)ld pqueries to (n)ew pqueries
    for (Pquery* opqry : other.pqueries) {
        Pquery* npqry = new Pquery;
        for (PqueryPlacement* op : opqry->placements) {
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
        for (PqueryName* on : opqry->names) {
            PqueryName* nn = new PqueryName(on);
            nn->pquery = npqry;
            npqry->names.push_back(nn);
        }
        this->pqueries.push_back(npqry);
    }
    return true;
}

/**
 * @brief Recalculates the `like_weight_ratio` of the placements of each Pquerie so that their sum
 * is 1.0, while maintaining their ratio to each other.
 */
void Placements::NormalizeWeightRatios()
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
void Placements::RestrainToMaxWeightPlacements()
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
            std::deque<PqueryPlacement*>::iterator it = edge->placements.begin();
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

        // remove all but the max element from placements by creating a new deque and swapping it.
        // this is faster and simpler than removing all but one element.
        std::deque<PqueryPlacement*> np;
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
size_t Placements::PlacementCount()
{
    size_t count = 0;
    for (Pquery* pqry : pqueries) {
        count += pqry->placements.size();
    }
    return count;
}

/**
 * @brief Get the summed mass of all placements on the tree, given by their `like_weight_ratio`.
 */
double Placements::PlacementMass()
{
    double sum = 0.0;
    for (Pquery* pqry : pqueries) {
        for (PqueryPlacement* place : pqry->placements) {
            sum += place->like_weight_ratio;
        }
    }
    return sum;
}

/**
 * @brief Calculates the Earth Movers Distance to another sets of placements on a fixed reference
 * tree.
 */
double Placements::EMD(Placements& right)
{
    return Placements::EMD(*this, right);
}

/**
 * @brief Calculates the Earth Movers Distance between two sets of placements on a fixed reference
 * tree.
 */
double Placements::EMD(Placements& lhs, Placements& rhs)
{
    // keep track of the total resulting distance.
    double distance = 0.0;

    // store a per-node balance of mass. each element in this map contains information about how
    // much placement mass is pushing from the direction of this node towards the root.
    // caveat: the masses that are stored here are already fully pushed towards the root, but are
    // stored here using the node at the lower end of the branch as key.
    std::unordered_map<PlacementTree::NodeType*, double> balance;

    // use the sum of masses as normalization factor for the masses.
    double totalmass_l = lhs.PlacementMass();
    double totalmass_r = rhs.PlacementMass();

    // do a postorder traversal on both trees in parallel. while doing so, move placements
    // from the tips towards the root and store their movement (mass * distance) in balance[].
    // in theory, it does not matter where we start the traversal - however, the positions of the
    // placements are given as "distal_length" on their branch, which always points away from the
    // root. thus, if we decided to traverse from a different node than the root, we would have to
    // take this into account.
    PlacementTree::IteratorPostorder it_l = lhs.tree.BeginPostorder();
    PlacementTree::IteratorPostorder it_r = rhs.tree.BeginPostorder();
    for (
        ;
        it_l != lhs.tree.EndPostorder() && it_r != rhs.tree.EndPostorder();
        ++it_l, ++it_r
    ) {
        // check whether both trees have identical topology. if they have, the ranks of all nodes
        // are the same. however, if not, at some point their ranks will differ.
        if (it_l.Node()->Rank() != it_r.Node()->Rank()) {
            LOG_WARN << "Calculating EMD on different reference trees not possible.";
            return -1.0;
        }

        // if we are at the last iteration, we reached the root, thus we have moved all masses now
        // and don't need to proceed. if we did, we would count an edge of the root again.
        if (it_l.IsLastIteration()) {
            // we do a check for the mass at the root here for debug purposes.
            double root_mass = 0.0;
            for (
                PlacementTree::NodeType::IteratorLinks n_it = it_l.Node()->BeginLinks();
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

        // move placements around between children, and collect the remaining mass in mass_s.
        // mass_s then contains the rest mass of the subtree that could not be distributed among
        // the children and thus has to be moved upwards.
        double mass_s = 0.0;
        PlacementTree::LinkType* link = it_l.Link()->Next();
        while (link != it_l.Link()) {
            // we do postorder traversal, so we have seen the child nodes of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link->Outer()->Node()));

            mass_s += balance[link->Outer()->Node()];
            link = link->Next();
        }

        // we now start a "normal" EMD caluclation on the current edge. for this, we store the
        // masses of all placements sorted by their position on the branch.
        std::multimap<double, double> edge_balance;

        // add all placements of the branch from the left tree (using positive mass)...
        for (PqueryPlacement* place : it_l.Edge()->placements) {
            // TODO consider like_weight_ratio here!
            distance += place->pendant_length / totalmass_l;
            edge_balance.emplace(place->distal_length, +1.0 / totalmass_l);
        }

        // ... and the branch from the right tree (using negative mass)
        for (PqueryPlacement* place : it_r.Edge()->placements) {
            // TODO consider like_weight_ratio here!
            distance += place->pendant_length / totalmass_r;
            edge_balance.emplace(place->distal_length, -1.0 / totalmass_r);
        }

        // start the EMD with the mass that is left over from the subtrees...
        double cur_pos  = it_l.Edge()->branch_length;
        double cur_mass = mass_s;

        // ... and move it along the branch, balancing it with the placements found on the branches.
        // this is basically a standard EMD calculation along the branch.
        std::multimap<double, double>::reverse_iterator rit;
        for (rit = edge_balance.rbegin(); rit != edge_balance.rend(); ++rit) {
            distance += std::abs(cur_mass) * (cur_pos - rit->first);
            cur_mass += rit->second;
            cur_pos   = rit->first;
        }

        // finally, move the rest to the end of the branch and store its mass in balance[],
        // so that it can be used for the nodes further up in the tree.
        distance += std::abs(cur_mass) * cur_pos;
        balance[it_l.Node()] = cur_mass;
    }

    // check whether we are done with both trees.
    if (it_l != lhs.tree.EndPostorder() || it_r != rhs.tree.EndPostorder()) {
        LOG_WARN << "Inconsistent reference trees in EMD calculation.";
        return -1.0;
    }

    return distance;
}

/**
 * @brief Calculate the Center of Gravity of the placements on a tree.
 */
void Placements::COG()
{
    // store a balance of mass per link, so that each element contains the mass that lies
    // in the direction of this link
    std::unordered_map<PlacementTree::LinkType*, double> balance;

    // do a postorder traversal
    for (
        PlacementTree::IteratorPostorder it = this->tree.BeginPostorder();
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
            mass += place->pendant_length + place->distal_length;
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

/**
 * @brief Calculate the Variance of the placements on a tree.
 *
 * The variance is a measure of how far a set of items is spread out
 * (http://en.wikipedia.org/wiki/Variance). In many cases, it can be measured using the mean of the
 * items. However, when considering placements on a tree, this does not truly measure how far they
 * are from each other. Thus, this algorithm applies a different method of calculating the variance
 * in terms of squared deviations of all items from each other:
 * \f$ \operatorname{Var}(X) = \frac{1}{n^2} \sum_{i=1}^n \sum_{j=1}^n \frac{1}{2}(x_i - x_j)^2 \f$,
 * where \f$ (x_i - x_j) \f$ denotes the distance between two placements.
 *
 * This distance is calculated as the shortest path between the two placements. This includes the
 * pendant_length of both, as well as their position on the branches. There are three cases that
 * might occur:
 *
 *   1. Both placements are on the same branch.
 *      In this case, their distance is caluclated as the sum of their pendant_lengths and their
 *      difference in distal_lengths.
 *
 *   2. The path between the placements includes the root.
 *      The distance of a placement from its neighbouring nodes is mostly given in form of the
 *      distal_length, which is the distance of the placement to the node (at the end of its branch)
 *      that lies in direction of the root. Thus, there is an implicit notion of a root, that we
 *      need to consider. If the path between two placements contains the root, we can directly
 *      calculate their distance as the distance between the two distal nodes plus distal_lengths
 *      and pendant_lengths of both placements. We call this the distal-distal case.
 *
 *   3. The root is not part of the path between the placements.
 *      This case means that one of the two placements lies on the path between the other placement
 *      and the root -- thus, the path between the placements does not contain the root.
 *      The distance between the placements cannot be calculated using the distal_lengths directly,
 *      but we need to get the proximal_length (away from the root) of the inner placement first.
 *      This is simply the difference between branch_length and distal_length of that placement.
 *      Of course, this case comes in two flavours, because both placements can be the inner or
 *      outer one. They are called proximal-distal case and distal-proximal case, respectively.
 *
 * The first case is easy to detect by comparing the edges. However, distinguishing between the
 * latter two cases is expensive, as it involves finding the path to the root for both placements.
 * To speed this up, we instead use a distance matrix that is calculated in the beginning of the
 * algorithm and contains the pairwise distances between all nodes of the tree. Using this, we do
 * not need to find paths between placements, but simply go to the nodes at the end of the branches
 * of the placements and do a lookup for those nodes.
 *
 * With this technique, we can calculate the distances between the placements for all
 * three cases (distal-distal, proximal-distal and distal-proximal) cheaply. The wanted distance is
 * then simply the minimum of those three distances. This is correct, because the two wrong cases
 * will always produce an overestimation of the distance.
 *
 * This distance is normalized using the `like_weight_ratio` of both placements, before
 * summing it up to calculate the variance.
 *
 * Furthermore, the normalizing factor \f$ \frac{1}{n^2} \f$ of the variance usually contains the
 * number of elements being processed. However, as the placements are weighted by their
 * `like_weight_ratio`, we instead calculate `n` as the sum of the `like_weight_ratio` of all
 * placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this
 * number will be equal to the number of pqueries (and thus be equal to the usual case of using the
 * number of elements). However, as this is not required (placements with small ratio can be dropped,
 * so that their sum per pquery is less than 1.0), we need to calculate this number manually here.
 */
double Placements::Variance() const
{
    Matrix<double>* distances = tree.NodeDistanceMatrix();
    double variance = 0.0;
    double count    = 0.0;

    /*
    int progress    = 0;
    LOG_DBG << "starting...";
    // do a pairwise comparision of all (!) placements.
    for (Pquery* pqry_a : pqueries) {
        //~ LOG_DBG1 << "a progress " << progress << " of " << pqueries.size();
        LOG_PROG(++progress, pqueries.size()) << "of Variance() finished.";
        //~ LOG_DBG1 << "b progress " << progress << " of " << pqueries.size();

        for (PqueryPlacement* place_a : pqry_a->placements) {
            count    += place_a->like_weight_ratio;
            variance += VariancePartial(place_a, distances);
        }
    }
    LOG_DBG << "finished.";
    //*/

    //*
    LOG_DBG << "supported num of threads: " << std::thread::hardware_concurrency();
    LOG_DBG << "starting threads...";

#   define NT 4

    std::thread* threads[NT];
    double       partials[NT];
    double       counts[NT];

    int block_size = pqueries.size() / NT;
    LOG_DBG << "block size " << block_size;

    for (int i = 0; i < NT; ++i) {
        LOG_DBG1 << "starting thread " << i;
        partials[i] = 0.0;
        counts[i] = 0;
        //~ threads[i] = new std::thread(std::bind(&Placements::VarianceThread, this, i * block_size, block_size, distances, &partials[i], &counts[i]));
        threads[i] = new std::thread(std::bind(&Placements::VarianceThread, this, i, NT, distances, &partials[i], &counts[i]));
    }

    LOG_DBG << "waiting for threads...";

    for (int i = 0; i < NT; ++i) {
        LOG_DBG1 << "waiting for thread " << i << ", v " << variance;
        threads[i]->join();
        variance += partials[i];
        count    += counts[i];
        LOG_DBG1 << "done with thread " << i << ", v " << variance;
    }

    LOG_DBG << "done, v " << variance << ", count " << count;
    //*/

    delete distances;

    double tmp = ((variance / count) / count) / 2;
    LOG_DBG << "variance " << variance << ", tmp " << tmp;
    return tmp;
}

/**
 * @brief
 */
void Placements::VarianceThread (const int offset, const int incr, const Matrix<double>* distances, double* partial, double* count) const
{
    int progress = 0;

    LOG_DBG2 << "thread: offset(" << offset << "), incr(" << incr << ") copying placements...";
    Placements cpy(*this);

    LOG_DBG2 << "thread: offset(" << offset << "), incr(" << incr << ") started with partial " << *partial;
    for (size_t i = offset; i < cpy.pqueries.size(); i += incr) {
    //~ for (size_t i = offset; i < offset + incr; ++i) {
        LOG_PROG(++progress, cpy.pqueries.size()) << "of Variance() finished (offset " << offset << ", incr " << incr << ").";

        Pquery* pqry_a = cpy.pqueries[i];
        for (PqueryPlacement* place_a : pqry_a->placements) {
            *count   += place_a->like_weight_ratio;
            *partial += VariancePartial(place_a, distances, cpy.pqueries);
        }
    }
    LOG_DBG2 << "thread: offset(" << offset << "), incr(" << incr << ") finished with partial " << *partial;
}

/**
 * @brief Calculates the sum of distances contributed by one placement for the variance.
 * See Variance() for more information.
 *
 * This function is intended to make the implementation of a threaded version of the calculation
 * feasible.
 */
double Placements::VariancePartial(const PqueryPlacement* place_a, const Matrix<double>* distances, const std::deque<Pquery*>& mypqueries) const
{
    double variance = 0.0;
    int node_a, node_b;

    for (Pquery* pqry_b : mypqueries) {
        for (PqueryPlacement* place_b : pqry_b->placements) {
            // same placement
            if (place_a == place_b) {
                continue;
            }

            // same branch case
            if (place_a->edge == place_b->edge) {
                variance += place_a->pendant_length
                         +  std::abs(place_a->distal_length - place_b->distal_length)
                         +  place_b->pendant_length;
                continue;
            }

            // distal-distal case
            node_a = place_a->edge->PrimaryNode()->Index();
            node_b = place_b->edge->PrimaryNode()->Index();
            double dd = place_a->pendant_length + place_a->distal_length
                      + (*distances)(node_a, node_b)
                      + place_b->distal_length + place_b->pendant_length;

            // proximal-distal case
            node_a = place_a->edge->SecondaryNode()->Index();
            node_b = place_b->edge->PrimaryNode()->Index();
            double pd = place_a->pendant_length
                      + place_a->edge->branch_length - place_a->distal_length
                      + (*distances)(node_a, node_b)
                      + place_b->distal_length + place_b->pendant_length;

            // distal-proximal case
            node_a = place_a->edge->PrimaryNode()->Index();
            node_b = place_b->edge->SecondaryNode()->Index();
            double dp = place_a->pendant_length + place_a->distal_length
                      + (*distances)(node_a, node_b)
                      + place_b->edge->branch_length - place_b->distal_length
                      + place_b->pendant_length;

            // find min of the three cases, normalize it to the weight ratios (we do this
            // here to minimize the number of operations executed) and add it to the variance.
            double min = std::min(dd, std::min(pd, dp));
            min *= place_a->like_weight_ratio * place_b->like_weight_ratio;
            variance += min * min;
        }
    }

    return variance;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Returns a list of all Pqueries with their %Placements and Names.
 */
std::string Placements::Dump() const
{
    std::ostringstream out;
    for (Pquery* pqry : pqueries) {
        for (PqueryName* n : pqry->names) {
            out << n->name;
            if (n->multiplicity != 0.0) {
                out << " (" << n->multiplicity << ")";
            }
            out << "\n";
        }
        for (PqueryPlacement* p : pqry->placements) {
            out << p->edge_num << ": ";
            if (p->likelihood != 0.0 || p->like_weight_ratio != 0.0) {
                out << p->likelihood << "|" << p->like_weight_ratio << " ";
            }
            if (p->parsimony != 0.0) {
                out << p->parsimony << " ";
            }
            out << p->distal_length << "|" << p->pendant_length << "\n";
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
 * example that the distal_length is smaller than the corresponding branch_length.
 * If additionally `break_on_values` is set, Validate() will stop on the first encountered invalid
 * value. Otherwise it will report all invalid values.
 */
bool Placements::Validate (bool check_values, bool break_on_values) const
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
        for (PqueryPlacement* p : edge->placements) {
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
    for (Pquery* pqry : pqueries) {
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
        for (PqueryPlacement* p : pqry->placements) {
            // make sure the pointers and references are set correctly
            if (p->pquery != pqry) {
                LOG_INFO << "Inconsistent pointer from placement to pquery at '" << name << "'.";
                return false;
            }
            int found_placement_on_edge = 0;
            for (PqueryPlacement* pe : p->edge->placements) {
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
            if (p->pendant_length < 0.0 || p->distal_length < 0.0) {
                LOG_INFO << "Invalid placement with pendant_length '" << p->pendant_length
                         << "' or distal_length '" << p->distal_length << "' < 0.0 at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->distal_length > p->edge->branch_length) {
                LOG_INFO << "Invalid placement with distal_length '" << p->distal_length
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
        for (PqueryName* n : pqry->names) {
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
