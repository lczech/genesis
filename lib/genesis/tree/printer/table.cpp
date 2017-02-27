/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/tree/printer/table.hpp"

#include "genesis/tree/tree.hpp"
// #include "genesis/lib/utils/text/table.hpp"

#include <assert.h>
#include <sstream>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Print Table
// =================================================================================================

void PrinterTable::print( std::ostream& out, Tree const& tree )
{
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

std::string PrinterTable::print( Tree const& tree )
{
    std::ostringstream res;
    print( res, tree );
    return res.str();
}

} // namespace tree
} // namespace genesis
