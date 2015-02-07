/**
 * @brief Implementation of Placements class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placements.hpp"

#include <assert.h>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <unordered_map>

#include "utils/logging.hpp"
#include "utils/math.hpp"

namespace genesis {

// =============================================================================
//     Constructor & Destructor
// =============================================================================

void Placements::clear()
{
    for (Pquery* pqry : pqueries) {
        delete pqry;
    }
    std::deque<Pquery*>().swap(pqueries);
    tree.clear();
    metadata.clear();
}

Placements::~Placements()
{
    clear();
}

/**
 * @brief Returns a mapping of edge_num integers to the corresponding Edge object.
 */
Placements::EdgeNumMapType* Placements::EdgeNumMap()
{
    EdgeNumMapType* edge_num_map = new EdgeNumMapType();
    for (
        PlacementTree::IteratorEdges it = tree.BeginEdges();
        it != tree.EndEdges();
        ++it
    ) {
        PlacementTree::EdgeType* edge = *it;
        assert(edge_num_map->count(edge->data.edge_num) == 0);
        edge_num_map->emplace(edge->data.edge_num, edge);
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
        return it_l.Node()->data.name     == it_r.Node()->data.name     &&
               it_l.Edge()->data.edge_num == it_r.Edge()->data.edge_num;
    };
    if (!tree.Equal(other.tree, comparator)) {
        LOG_WARN << "Cannot merge Placements with different reference trees.";
        return false;
    }

    // we need to assign edge pointers to the correct edge objects, so we need a mapping
    EdgeNumMapType* edge_num_map = EdgeNumMap();

    // copy all pqueries
    for (Pquery* opqry : other.pqueries) {
        Pquery* npqry = new Pquery;
        for (PqueryPlacement* op : opqry->placements) {
            PqueryPlacement* np = new PqueryPlacement(op);

            // assuming that the trees have identical topology (checked at the beginning of this
            // function), there will be an edge for every placement. if this assertion fails,
            // something broke the integrity of our in memory representation of the data.
            assert(edge_num_map->count(np->edge_num) > 0);
            np->edge = (*edge_num_map)[np->edge_num];
            np->edge->data.placements.push_back(np);
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
 * @brief Removes all placements but the most likely one from all pqueries.
 *
 * Pqueries can contain multiple placements on different branches. For example, the EPA algorithm
 * of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by
 * default. The property `like_weight_ratio` weights those placement positions so that it's sum over
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
            std::deque<PqueryPlacement*>::iterator it = edge->data.placements.begin();
            for (; it != edge->data.placements.end(); ++it) {
                if (*it == place) {
                    break;
                }
            }
            // assert that the edge actually contains a reference to this pquery. if not,
            // this means that we messed up somewhere else while adding/removing placements...
            assert(it != edge->data.placements.end());
            edge->data.placements.erase(it);
        }

        // we do not allow empty placement objects (should we?)
        assert(max_w > -1.0);

        // remove all but the max element from placements by creating a new deque and swapping it.
        // this is faster and simpler than removing all but one element.
        std::deque<PqueryPlacement*> np;
        np.push_back(max_p);
        pqry->placements.swap(np);

        // now add back the reference from the edge to the pquery.
        // assert that we now have a single placement in the pquery (the most likely one).
        assert(pqry->placements.size() == 1 && pqry->placements[0] == max_p);
        max_p->edge->data.placements.push_back(max_p);

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
        // TODO do not check branch length, but names here (similar to what merge does)
        if (it_l.Edge()->data.branch_length != it_r.Edge()->data.branch_length ||
            it_l.Edge()->data.edge_num      != it_r.Edge()->data.edge_num
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
        for (PqueryPlacement* place : it_l.Edge()->data.placements) {
            // TODO consider like_weight_ratio here!
            distance += place->pendant_length / totalmass_l;
            edge_balance.emplace(place->distal_length, +1.0 / totalmass_l);
        }

        // ... and the branch from the right tree (using negative mass)
        for (PqueryPlacement* place : it_r.Edge()->data.placements) {
            // TODO consider like_weight_ratio here!
            distance += place->pendant_length / totalmass_r;
            edge_balance.emplace(place->distal_length, -1.0 / totalmass_r);
        }

        // start the EMD with the mass that is left over from the subtrees...
        double cur_pos  = it_l.Edge()->data.branch_length;
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

            mass += balance[link] * it.Edge()->data.branch_length;
            link  = link->Next();
        }

        // add up the masses of placements on the current branch
        for (PqueryPlacement* place : it.Edge()->data.placements) {
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
        LOG_DBG1 << "a " << p_link->Node()->data.name;
        p_mass = -1.0;
        for (
            PlacementTree::NodeType::IteratorLinks it_l = p_link->Node()->BeginLinks();
            it_l != p_link->Node()->EndLinks();
            ++it_l
        ) {
            LOG_DBG2 << it_l.Node()->data.name << " " << balance[it_l.Link()];
            if (balance[it_l.Link()] > p_mass) {
                p_link = it_l.Link();
            }
        }
        LOG_DBG1 << "b " << p_link->Node()->data.name;
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
        //~ mass += balance[link] * it.Edge()->data.branch_length;
        //~ link = link->Next();
    //~ }

    for (auto pair : balance) {
        LOG_DBG1 << pair.first->Node()->data.name << ": " << pair.second << "\n";
        //~ distance += std::abs(pair.second);
    }
}

/**
 * @brief Calculate the Variance of the placements on a tree.
 */
double Placements::Variance()
{
    Matrix<double>* distances = tree.NodeDistanceMatrix();
    double variance = 0.0;

    for (Pquery* pqry_a : pqueries) {
        for (PqueryPlacement* place_a : pqry_a->placements) {
            //~ p.distal_length;

            for (Pquery* pqry_b : pqueries) {
                for (PqueryPlacement* place_b : pqry_b->placements) {
                    //~ p.distal_length;
                    LOG_DBG << place_a->distal_length << " " << place_b->distal_length;
                }
            }

        }
    }

    LOG_DBG << distances->Dump();
    LOG_DBG << "Variance: " << variance;
    delete distances;
    return 0.0;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Returns a list of all Pqueries with their %Placements and Names.
 */
std::string Placements::Dump()
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
bool Placements::Validate (bool check_values, bool break_on_values)
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
        PlacementTree::IteratorEdges it_e = tree.BeginEdges();
        it_e != tree.EndEdges();
        ++it_e
    ) {
        // make sure every edge num is used once only
        PlacementTree::EdgeType* edge = *it_e;
        if (edge_num_map.count(edge->data.edge_num) > 0) {
            LOG_INFO << "More than one edge has edge_num '" << edge->data.edge_num << "'.";
            return false;
        }
        edge_num_map.emplace(edge->data.edge_num, edge);

        // make sure the pointers and references are set correctly
        for (PqueryPlacement* p : edge->data.placements) {
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
            for (PqueryPlacement* pe : p->edge->data.placements) {
                if (pe == p) {
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
            if (p->pendant_length < 0.0 || p->distal_length < 0.0) {
                LOG_INFO << "Invalid placement with pendant_length '" << p->pendant_length
                         << "' or distal_length '" << p->distal_length << "' < 0.0 at "
                         << name << ".";
                if (break_on_values) {
                    return false;
                }
            }
            if (p->distal_length > p->edge->data.branch_length) {
                LOG_INFO << "Invalid placement with distal_length '" << p->distal_length
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
