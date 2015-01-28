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

#include "placement/jplace_parser.hpp"
#include "utils/logging.hpp"

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
}

Placements::~Placements()
{
    clear();
}

bool Placements::FromJplaceFile (const std::string&  fn)
{
    return JplaceParser::ProcessFile(fn, *this);
}

bool Placements::FromJplaceString (const std::string&  jplace)
{
    return JplaceParser::ProcessString(jplace, *this);
}

bool Placements::FromJsonLexer (const JsonLexer&    lexer)
{
    return JplaceParser::ProcessLexer(lexer, *this);
}

bool Placements::FromJsonDocument (const JsonDocument& doc)
{
    return JplaceParser::ProcessDocument(doc, *this);
}

bool Placements::Merge(Placements& other)
{
    // TODO identical data so far checks for branch length and edge num, but not placements on the edge.
    // TODO if it did, this function would never return true for two different placements...
    if (!tree.HasIdenticalTopology(other.tree) || !tree.HasIdenticalData(other.tree)) {
        LOG_WARN << "Cannot merge Placements with different reference trees.";
        return false;
    }

    for (Pquery* pqry : other.pqueries) {
        Pquery* npqry = new Pquery;
        *npqry = *pqry;
        this->pqueries.push_back(npqry);
    }
    return true;
}

// =============================================================================
//     Placement Mass
// =============================================================================

/**
 * @brief Get the summed mass of all placements on the tree.
 */
double Placements::PlacementMassSum()
{
    double sum = 0.0;
    for (
        PlacementTree::IteratorEdges it = tree.BeginEdges();
        it != tree.EndEdges();
        ++it
    ) {
        sum += (*it)->data.PlacementMass();
    }
    return sum;
}

/**
 * @brief Calculates the Earth Movers Distance between two sets of placements on a fixed reference
 * tree.
 */
double Placements::EMD(Placements& right)
{
    // keep track of the total resulting distance
    double distance = 0.0;

    // store a per-node balance of mass
    std::unordered_map<PlacementTree::NodeType*, double> balance;

    // use sum of masses as normalization factor for the masss
    double totalmass_l = this->PlacementMassSum();
    double totalmass_r = right.PlacementMassSum();

    // do a postorder traversal on both trees in parallel. while doing so, move placements
    // from the tips towards the root and store their movement (mass * distance) in balance.
    PlacementTree::IteratorPostorder it_l = this->tree.BeginPostorder();
    PlacementTree::IteratorPostorder it_r = right.tree.BeginPostorder();
    for (
        ;
        it_l != this->tree.EndPostorder() && it_r != right.tree.EndPostorder();
        ++it_l, ++it_r
    ) {
        // check whether both trees have identical topology.
        if (it_l->Rank() != it_r->Rank()) {
            LOG_WARN << "Calculating EMD on different reference trees not possible.";
            return -1.0;
        }

        // if we are at the last iteration, we reached the root, thus we have moved all masses now
        // and don't need to proceed. if we did, we would count an edge of the root again.
        if (it_l.IsLastIteration()) {
            continue;
        }

        // if only one of them has an edge but the other not, that's an error.
        if (it_l.Edge()->data.branch_length != it_r.Edge()->data.branch_length ||
            it_l.Edge()->data.edge_num      != it_r.Edge()->data.edge_num
        ) {
            LOG_WARN << "Inconsistent reference trees in EMD calculation.";
            return -1.0;
        }

        // move placements around between children, collect remaining mass in mass_s.
        // mass_s then contains the rest mass of the subtree that could not be distributed among
        // the children.
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

        // add all placements of the left branch...
        for (
            PlacementEdgeData::IteratorPlacements it_p = it_l.Edge()->data.BeginPlacements();
            it_p != it_l.Edge()->data.EndPlacements();
            ++it_p
        ) {
            if (it_p->pendant_length < 0.0 || it_p->distal_length < 0.0) {
                LOG_INFO << "Tree contains placement with pendant_length or distal_length < 0.0 "
                         << "at node '" << it_l.Node()->data.name << "'.";
            }
            if (it_p->distal_length > it_l.Edge()->data.branch_length) {
                LOG_INFO << "Tree contains placement with distal_length > branch_length "
                         << "at node '" << it_l.Node()->data.name << "'.";
            }

            distance += it_p->pendant_length / totalmass_l;
            edge_balance.emplace(it_p->distal_length, +1.0 / totalmass_l);
        }

        // ... and the right branch
        for (
            PlacementEdgeData::IteratorPlacements it_p = it_r.Edge()->data.BeginPlacements();
            it_p != it_r.Edge()->data.EndPlacements();
            ++it_p
        ) {
            if (it_p->pendant_length < 0.0 || it_p->distal_length < 0.0) {
                LOG_INFO << "Tree contains placement with pendant_length or distal_length < 0.0 "
                         << "at node '" << it_r.Node()->data.name << "'.";
            }
            if (it_p->distal_length > it_r.Edge()->data.branch_length) {
                LOG_INFO << "Tree contains placement with distal_length > branch_length "
                         << "at node '" << it_r.Node()->data.name << "'.";
            }

            distance += it_p->pendant_length / totalmass_r;
            edge_balance.emplace(it_p->distal_length, -1.0 / totalmass_r);
        }

        // start with the mass that is left over from the subtrees...
        double cur_pos  = it_l.Edge()->data.branch_length;
        double cur_mass = mass_s;

        // ... and move it along the branch, balancing it with the placements found on the branch.
        // this is basically a standard EMD calculation along the branch
        std::multimap<double, double>::reverse_iterator rit;
        for (rit = edge_balance.rbegin(); rit != edge_balance.rend(); ++rit) {
            distance += std::abs(cur_mass) * (cur_pos - rit->first);
            cur_mass += rit->second;
            cur_pos   = rit->first;
        }

        // finally, move the rest to the end of the branch and store its mass in balance[]
        distance += std::abs(cur_mass) * cur_pos;
        balance[it_l.Node()] = cur_mass;
    }

    // check whether we are done with both trees
    if (it_l != this->tree.EndPostorder() || it_r != right.tree.EndPostorder()) {
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
        for (
            PlacementEdgeData::IteratorPlacements it_p = it.Edge()->data.BeginPlacements();
            it_p != it.Edge()->data.EndPlacements();
            ++it_p
        ) {
            mass += it_p->pendant_length + it_p->distal_length;
        }

        assert(balance.count(it->Link()->Outer()) == 0);
        balance[it->Link()->Outer()] = mass;
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

double Placements::Variance()
{
    return 0.0;
}

// =============================================================================
//     Dump and Debug
// =============================================================================

std::string Placements::DumpAll()
{
    return DumpPqueries() + DumpTree();
}

std::string Placements::DumpPqueries()
{
    std::ostringstream out;
    for (Pquery* pqry : pqueries) {
        for (Pquery::Name& n : pqry->names) {
            out << n.name;
            if (n.multiplicity != 0.0) {
                out << " (" << n.multiplicity << ")";
            }
            out << "\n";
        }
        for (Pquery::Placement& p : pqry->placements) {
            out << p.edge_num << ": ";
            if (p.likelihood != 0.0 || p.like_weight_ratio != 0.0) {
                out << p.likelihood << "|" << p.like_weight_ratio << " ";
            }
            if (p.parsimony != 0.0) {
                out << p.parsimony << " ";
            }
            out << p.distal_length << "|" << p.pendant_length << "\n";
        }
        out << "\n";
    }
    return out.str();
}

std::string Placements::DumpTree()
{
    return tree.DumpAll();
}

bool Placements::Validate()
{
    //~ for (Pquery* pqry : pqueries) {
        //~ for (Pquery::Placement& p : pqry->placements) {
            //~ if (p.edge_num != p.edge->data.edge_num) {
                //~ return false;
            //~ }
            // TODO add check for other way round: reference from edge to pqry

            //~ if (p.pendant_length < 0.0 || p.distal_length < 0.0) {
                //~ LOG_INFO << "Tree contains placement with pendant_length or distal_length < 0.0 "
                         //~ << "at node '" << it.Node()->data.name << "'.";
            //~ }
            //~ if (p.distal_length > it.Edge()->data.branch_length) {
                //~ LOG_INFO << "Tree contains placement with distal_length > branch_length "
                         //~ << "at node '" << it.Node()->data.name << "'.";
            //~ }
        //~ }
    //~ }
    return true;
}

} // namespace genesis
