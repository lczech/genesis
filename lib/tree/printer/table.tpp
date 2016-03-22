/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

// #include "lib/utils/text/table.hpp"

#include <assert.h>
#include <sstream>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Print Table
// =================================================================================================

template <typename TreeType>
void PrinterTable::print (
    std::ostream&   out,
    TreeType const& tree
) {
    /*
    // nodes
    auto node_table = utils::Table();
    node_table.add_column("Node");
    node_table.add_column("Main Link").justify(utils::Table::Column::Justification::kRight);
    node_table.add_column("...");

    for (size_t i = 0; i < tree.node_count(); ++i) {
        node_table << std::to_string( i )
                   << std::to_string( tree.node_at(i)->link()->index() )
                   << tree.node_at(i)->dump();
    }
    out << utils::simple_layout()(node_table) << "\n";
    */

    // nodes
    for (size_t i = 0; i < tree.node_count(); ++i) {
        out << "Node " << i
            << " \t Main Link: " << tree.node_at(i).link().index()
            << " \t " << tree.node_at(i).dump() << "\n";
    }
    out << "\n";

    // edges
    for (size_t i = 0; i < tree.edge_count(); ++i) {
        out << "Edge " << i
            << " \t Link P: " << tree.edge_at(i).primary_link().index()
            << " \t Link S: " << tree.edge_at(i).secondary_link().index()
            << " \t " << tree.edge_at(i).dump() << "\n";
    }
    out << "\n";

    // links
    for (size_t i = 0; i < tree.link_count(); ++i) {
        out << "Link " << i
            << "  \t Next: "  << tree.link_at(i).next().index()
            << " \t Outer: " << tree.link_at(i).outer().index()
            << " \t Node: "  << tree.link_at(i).node().index()
            << " \t Edge: "  << tree.link_at(i).edge().index()
            << " \t " << tree.link_at(i).dump()
            << "\n";
    }
}

template <typename TreeType>
std::string PrinterTable::print ( TreeType const& tree )
{
    std::ostringstream res;
    print( res, tree );
    return res.str();
}

} // namespace tree
} // namespace genesis
