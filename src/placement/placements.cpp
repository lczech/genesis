/**
 * @brief Implementation of Placements class
 *
 * @file
 * @ingroup placement
 */

#include "placement/placements.hpp"

#include <sstream>

namespace genesis {

// =============================================================================
//     Placements
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

} // namespace genesis
