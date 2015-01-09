/**
 * @brief Implementation of Placements class
 *
 * @file
 * @ingroup placement
 */

#include "placement/placements.hpp"

#include <assert.h>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <unordered_map>

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

// =============================================================================
//     Placement Weight
// =============================================================================

double Placements::PlacementWeightSum()
{
    double sum = 0.0;
    for (
        PlacementTree::IteratorEdges it = tree.BeginEdges();
        it != tree.EndEdges();
        ++it
    ) {
        sum += (*it)->data.PlacementWeight();
    }
    return sum;
}

double Placements::EMD(Placements& right)
{
    std::unordered_map<PlacementTree::NodeType*, double> balance;

    // use sum of weights as normalization factor for the weights
    double weightsum_l = this->PlacementWeightSum();
    double weightsum_r = right.PlacementWeightSum();

    // do a postorder traversal on both trees in parallel. while doing so, move placements
    // from the tips towards the root and store their movment (mass * distance) in balance.
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
        // if both nodes do not have a corresponding edge (eg the root), we skip them.
        if (!it_l.Edge() && !it_r.Edge()) {
            continue;
        }
        // however, if only one of them has an edge but the other not, that's an error.
        if (!it_l.Edge() || !it_r.Edge()) {
            LOG_WARN << "Inconsistent reference trees in EMD calculation.";
            return -1.0;
        }

        // move placements around between children
        double weight_s = 0.0;
        PlacementTree::LinkType* link = it_l.Link()->Next();
        while (link != it_l.Link()) {
            // we do postorder traversal, so we have seen the child nodes of the current node,
            // which means, they should be in the balance list already.
            assert(balance.count(link->Outer()->Node()));

            weight_s += balance[link->Outer()->Node()];
            link = link->Next();
        }

        // weight_s now contains the rest weight of the subtree that could not be distributed among
        // the children. move it up the current branch:
        weight_s *= it_l.Edge()->data.branch_length;

        double weight_l = it_l.Edge()->data.PlacementWeight() / weightsum_l;
        double weight_r = it_r.Edge()->data.PlacementWeight() / weightsum_r;

        balance[it_l.Node()] = weight_s + weight_l - weight_r;

        LOG_DBG << "Node " << it_l.Node()->data.name << "   #qrys " << it_l.Edge()->data.pqueries.size() << "|" << it_r.Edge()->data.pqueries.size() << std::setprecision(5) << std::fixed << "   weight_s " << weight_s << "\t weight_l " << weight_l << "\t weight_r " << weight_r << "\t balance " << balance[it_l.Node()];

        //~ double weight_c = 0.0;
        //~ if (balance.count(it_l.Node())) {
            //~ weight_c =  * it_l.Edge()->data.branch_length;
        //~ }
//~
        //~ if (balance.count(it_l.Node())) {
//~
        //~ } else {
            //~ balance[it_l.Node()] =
        //~ }
//~
        //~ PlacementTree::NodeType* target = it_l.Edge()->PrimaryNode();
        //~ if (balance.count(target)) {
            //~ balance[target] += weight_c + weight_l - weight_r;
        //~ } else {
            //~ balance[target]  = weight_c + weight_l - weight_r;
        //~ }

        //~ LOG_DBG1 << it_l->Dump() << (it_l.Edge() ? "   From '" + it_l.Edge()->PrimaryNode()->data.name + "' to '" + it_l.Edge()->SecondaryNode()->data.name + "'" + "   #qrys: " + std::to_string(it_l.Edge()->data.pqueries.size()) : "");
    }

    double dist = 0.0;
    for (auto pair : balance) {
        LOG_DBG1 << pair.first->data.name << ": " << pair.second << "\n";
        dist += std::abs(pair.second);
    }
    LOG_DBG1 << "  dist: " << dist;
    return dist;
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
    for (Pquery* pqry : pqueries) {
        for (Pquery::Placement& p : pqry->placements) {
            if (p.edge_num != p.edge->data.edge_num) {
                return false;
            }
            // TODO add check for other way round: reference from edge to pqry
        }
    }
    return true;
}

} // namespace genesis
